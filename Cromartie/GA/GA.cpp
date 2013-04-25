#include "GA.h"
#include "GeneticOperator.h"
#include "TournamentSelection.h"
#include "../GenericEvents.h"
#include "../ScoreHelper.h"
#include "../Stats.h"
#include "../EventManager.h"
#include "../HypothalamusEvents.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Settings.h"


GA::GA(void) : currentStateIndex(0), stateChanges(0)
{
}


GA::~GA(void)
{
}

void GA::onMorph(IEventDataPtr e)
{
	std::tr1::shared_ptr<UnitMorphEvent> pEventData = std::tr1::static_pointer_cast<UnitMorphEvent>(e);
	BWAPI::Unit* unit = pEventData->m_Unit;

	if (unit->getType() == BWAPI::UnitTypes::Protoss_Assimilator)
	{
		/*BWAPI::Broodwar->sendText("Changing state");
		BWAPI::Broodwar->sendText(unit->getType().getName().c_str());*/
		changeState();
	}
}

void GA::onUnitCompleteEvent(IEventDataPtr e)
{
	std::tr1::shared_ptr<UnitCompleteEvent> pEventData = std::tr1::static_pointer_cast<UnitCompleteEvent>(e);
	BWAPI::Unit* unit = pEventData->m_Unit;

	if (unit->getPlayer() == BWAPI::Broodwar->self() &&
		unit->getType().isBuilding() == true &&
		unit->getType().isResourceContainer() == false &&
		unit->getType() != BWAPI::UnitTypes::Protoss_Pylon)
	{
		if (stateChanges < 1)
		{
			stateChanges++;
			BWAPI::Broodwar->sendText("Skipping state change");

			stateExecutor.executeState(getCurrentState());
			return;
		}

		//BWAPI::Broodwar->sendText("Changing state");
		//BWAPI::Broodwar->sendText(unit->getType().getName().c_str());
		changeState();
	}
}

void GA::changeState()
{
	getCurrentState().setFitness(fitness(ScoreHelper::getPlayerScore(), ScoreHelper::getOpponentScore()));
	currentStateIndex++;

	if (currentStateIndex > 49)
		return;

	if(!stateExecutor.executeState(getCurrentState()))
		changeState();
}

State& GA::getCurrentState()
{
	return getCurrentChromosome().getState(currentStateIndex);
}

double GA::fitness(int score, int opponentScore)
{
	double fitness;

	// Hvis nu vi st�r i det f�rste state, er formlen lidt anderledes
	if (currentStateIndex == 0)
	{
		fitness = (double)score / ((double)score + (double)opponentScore);
	}
	else
	{
		fitness = ((double)score / ((double)score + (double)opponentScore)) -
			((double)prev_score / ((double)prev_score + (double)prev_opponentScore));
	}

	prev_score = score;
	prev_opponentScore = opponentScore;

	return fitness;
}

void GA::onGameEnd(bool winner, int score, int scoreOpponent, int elapsedTime, int maxElapsedTime)
{
	double fitness = 0;
	if (winner)
	{
		fitness = (double)score / ((double)scoreOpponent);
	}
	else
	{
		fitness = ((double)elapsedTime / (double)maxElapsedTime) * ((double)score / ((double)scoreOpponent));
	}

	fitness *= 10000;

	Chromosome& chromo = getCurrentChromosome();
	chromo.setFitness(fitness);

	bool nonTestedChromosomeFound = false;
	for (size_t i = 0; i < population.size(); i++)
	{
		if (population.at(i).getFitness() == -999)
		{
			nonTestedChromosomeFound = true;
			break;
		}
	}
	if (nonTestedChromosomeFound == false)
	{
		status = 2; // 2 = finishedGeneration
	}

	savePopulation();
	saveGAStatus();
	Stats::logPop(population, elapsedTime, winner);
}

void GA::onStarcraftStart()
{
	loadGAStatus();

	if (status == 0) // 0 = FirstRun
	{
		generateInitialPopulation(POP_SIZE);
		//status = 1; // 1 = running
	}
	else if (status == 1) // 1 = running
	{
		loadPopulation();
	}
	else if (status == 2) // 2 = finishedGeneration
	{
		loadPopulation();
		createNextGeneration();
		status = 1;
	}

	for (size_t i = 0; i < population.size(); i++)
	{
		if (population.at(i).getFitness() == -999)
		{
			currentChromosomeIndex = i;
			break;
		}
	}

	BWAPI::Broodwar->sendText(static_cast<std::ostringstream*>( &(std::ostringstream() << currentChromosomeIndex) )->str().c_str());
}

Chromosome& GA::getCurrentChromosome()
{
	return population.at(currentChromosomeIndex);
}

void GA::loadPopulation()
{
	population = db.selectAllChromosomes();

	if (population.size() == 0)
		BWAPI::Broodwar->sendText("Could not load chromosomes");
}

void GA::savePopulation()
{
	if(status == 0) 
	{ 
		db.insertChromosomes(population); 
		status = 1; 
	}
	else 
	{ 
		db.updateChromosomes(population); 
	}
}

void GA::createNextGeneration()
{
	// Replace this class if you want another selection aglorithm
	TournamentSelection ts;
	ts.selectAndMutate(population);
	db.insertAndReplaceChromosomes(population);
}

void GA::generateInitialPopulation(int size)
{
	for (int i = 0; i < size; i++)
	{
		Chromosome c =  GeneticOperator::RandomChromosome();
		population.push_back(c);
	}
}

void GA::makeGAStatusFile()
{
	std::ofstream myfile ("status.txt");
  if (myfile.is_open())
  {
    myfile << "0\n"; // Status: FirstRun
    myfile.close();
  }
  else std::cout << "Unable to open GA state file";
}

void GA::saveGAStatus()
{
	std::ostringstream convert;

	std::ofstream myfile ("status.txt");
  if (myfile.is_open())
  {
	myfile << static_cast<std::ostringstream*>( &(std::ostringstream() << status) )->str() << "\n";
    myfile.close();
  }
  else std::cout << "Unable to open GA state file";
}

void GA::loadGAStatus()
{
	// If there is no state.txt file, we make it
	std::ifstream fileExists("status.txt");
	if (!fileExists) {
		makeGAStatusFile();
	}
	fileExists.close();

	// Parsing the state.txt file
	std::string line;
  std::ifstream myfile ("status.txt");
  if (myfile.is_open())
  {
	  // Read the status of the GA
	  myfile.good();
      std::getline (myfile,line);
	   status = atoi(line.c_str());

      myfile.close();
  }
  else std::cout << "Unable to open file"; 

	// Read next chromosome index
	// Read status: FirstRun, Running, FinishedGeneration, Finished

}