#include "GA.h"
#include "GeneticOperator.h"
#include "TournamentSelection.h"
#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream> 

GA::GA(void) : currentStateIndex(0), currentState(std::vector<UnitType>())
{
}


GA::~GA(void)
{
}

void GA::onUnitComplete(UnitType unit, int score, int opponentScore)
{
//	if (unit.isBuilding())
//	{
		currentState.setFitness(fitness(currentState, score, opponentScore));
//	}
	
	// GeneExecuter.ExecuteState(state);
}

State GA::getCurrentState()
{
	return currentChromosome.getStates().at(currentStateIndex);
}

double GA::fitness(State state, int score, int opponentScore)
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
		fitness = (double)score / ((double)score + (double)scoreOpponent);
	}
	else
	{
		fitness = ((double)elapsedTime / (double)maxElapsedTime) * ((double)score / ((double)score + (double)scoreOpponent));
	}

	currentChromosome.setFitness(fitness);
	savePopulation();
}

void GA::onStarcraftStart()
{
	loadGAStatus();
	loadPopulation();

	bool nonTestedChromosomeFound = false;
	for (int i = 0; i < population.size(); i++)
	{
		if (population.at(i).getFitness() == -999)
		{
			currentChromosomeIndex = i;
			currentChromosome = population.at(i);
			nonTestedChromosomeFound = true;
		}
	}
	if (nonTestedChromosomeFound == false)
	{
		status = 2; // 2 = finishedGeneration
	}

	if (status == 0) // 0 = FirstRun
	{
		generateInitialPopulation(50);
		status = 1; // 1 = running
	}
	else if (status == 1) // 1 = running
	{
		// Do nothing
	}
	else if (status == 2) // 2 = finishedGeneration
	{
		createNextGeneration();
	}
}

void GA::loadPopulation()
{
	DatabaseManager db;

	population = db.selectAllChromosomes();
}

void GA::savePopulation()
{
	DatabaseManager db;

	db.eraseDatabaseContent();

	db.insertChromosomes(population);
}

void GA::createNextGeneration()
{
	// Replace this class if you want another selection aglorithm
	TournamentSelection ts;
	ts.selectAndMutate(population);
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