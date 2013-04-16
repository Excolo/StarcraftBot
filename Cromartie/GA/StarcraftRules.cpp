#include "StarcraftRules.h"
#include "../Settings.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
extern boost::random::mt19937 randomGen;
std::tr1::shared_ptr<CombatGene> StarcraftRules::getValidCombatGene(const State& s, bool& found)
{
	// Get all valid units
	std::vector<BWAPI::UnitType> validUnits = getValidUnits(s);

	// If not valid units exist, we return NULL
	if (validUnits.size() == 0)
	{
		std::tr1::shared_ptr<CombatGene> cg( new CombatGene() );
		found = false;
		return cg;
	}
	else
	{
		found = true;
	}

	// Choose a random unit
	boost::random::uniform_int_distribution<> dist(0, validUnits.size()-1);
	int randomNrGene = dist(randomGen);
	BWAPI::UnitType randomUnit= validUnits.at(randomNrGene);

	// Choose a random number of that unit
	boost::random::uniform_int_distribution<> dist2(1, 10);
	int randomAmount = dist2(randomGen);

	// Create a CombatGene and set the unit type and amount
	std::tr1::shared_ptr<CombatGene> cg( new CombatGene(randomUnit, randomAmount) );

	return cg;
}

std::tr1::shared_ptr<ResearchGene> StarcraftRules::getValidResearchGene(const State& s, bool& found)
{
	// Get all valid upgrades
	std::vector<std::tr1::shared_ptr<BWAPI::Type>> validUpgrades = getValidUpgrades(s);

	// If no valid upgrades exists, we return NULL
	if (validUpgrades.size() == 0)
	{
		std::tr1::shared_ptr<ResearchGene> rg( new ResearchGene() );
		found = false;
		return rg;
	}
	else
	{
		found = true;
	}

	// Choose a random upgrade type
	boost::random::uniform_int_distribution<> dist(0, validUpgrades.size()-1);
	int randomNrGene = dist(randomGen);

	// Create the research gene and assign the upgrade
	std::tr1::shared_ptr<ResearchGene> rg( new ResearchGene(validUpgrades.at(randomNrGene)) );
	return rg;
}

std::tr1::shared_ptr<AttackGene> StarcraftRules::getValidAttackGene(const State& s, bool& found)
{
	// Choose a random number between 0 and 9
	boost::random::uniform_int_distribution<> dist(0, 9);
	int randomNrGene = dist(randomGen);

	// If 0, 1 or 2, create an attack gene
	if (randomNrGene < 3)
	{
		std::tr1::shared_ptr<AttackGene> ag( new AttackGene() );
		found = true;
		return ag;
	}
	// If not, do not create an attack gene
	else
	{
		std::tr1::shared_ptr<AttackGene> ag( new AttackGene() );
		found = false;
		return ag;
	}
}

std::tr1::shared_ptr<BuildGene> StarcraftRules::getValidBuildGene(const State& s)
{
	// Get all valid buildings
	std::vector<BWAPI::UnitType> validBuildings = getValidBuildings(s);

	// Choose a random building
	boost::random::uniform_int_distribution<> dist(0, validBuildings.size()-1);
	int randomNrGene = dist(randomGen);
	BWAPI::UnitType randomBuilding = validBuildings.at(randomNrGene);

	// Create the build gene and assign the random building to the gene
	std::tr1::shared_ptr<BuildGene> bg( new BuildGene(randomBuilding) );
	bg->name = randomBuilding.getName();

	return bg;
}

std::vector<std::tr1::shared_ptr<BWAPI::Type>> StarcraftRules::getValidUpgrades(const State& s)
{
	std::vector<std::tr1::shared_ptr<BWAPI::Type>> validUpgrades;

	// Loop through all buildings that has been build in order to determine which buildings are valid to build
	for (int i = 0; i < s.getBuildingSequence().size(); i++)
	{
		BWAPI::UnitType ut = s.getBuildingSequence().at(i);

		if (ut == BWAPI::UnitTypes::Protoss_Forge )
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Protoss_Ground_Weapons));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Protoss_Ground_Armor));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Protoss_Plasma_Shields));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Protoss_Air_Weapons));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Protoss_Air_Armor));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Singularity_Charge));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Robotics_Support_Bay)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Reaver_Capacity));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Scarab_Damage));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Gravitic_Drive));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Observatory)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Gravitic_Boosters));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Sensor_Array));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Leg_Enhancements));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Templar_Archives)
		{
			
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Hallucination));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Psionic_Storm));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Maelstrom));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Mind_Control));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Arbiter_Tribunal)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Khaydarin_Core));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Recall));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Stasis_Field));
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Fleet_Beacon)
		{
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::TechTypes::Disruption_Web));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Apial_Sensors));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Gravitic_Thrusters));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Carrier_Capacity));
			validUpgrades.push_back(std::tr1::shared_ptr<BWAPI::Type>(&BWAPI::UpgradeTypes::Argus_Jewel));
		}
	}


	return validUpgrades;
}

std::vector<BWAPI::UnitType> StarcraftRules::getValidUnits(const State& s)
{
	std::vector<BWAPI::UnitType> validUnits;

	// Loop through all buildings that have been build in order to find out which units can be build
	for (int i = 0; i < s.getBuildingSequence().size(); i++)
	{
		BWAPI::UnitType ut = s.getBuildingSequence().at(i);

		if (ut == BWAPI::UnitTypes::Protoss_Gateway)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Zealot);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Dragoon);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Templar_Archives)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Dark_Templar);
			validUnits.push_back(BWAPI::UnitTypes::Protoss_High_Templar);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Robotics_Facility)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Shuttle);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Robotics_Support_Bay)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Reaver);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Observatory)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Observer);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Stargate)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Scout);
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Corsair);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Arbiter_Tribunal)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Arbiter);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Fleet_Beacon)
		{
			validUnits.push_back(BWAPI::UnitTypes::Protoss_Carrier);
		}
	}

	return validUnits;
}

std::vector<BWAPI::UnitType> StarcraftRules::getValidBuildings(const State& s)
{
	std::vector<BWAPI::UnitType> validBuildings;
	
	// These buildings are availabel at all time, so we add them to the list of valid buildings to build
	validBuildings.push_back(BWAPI::UnitTypes::Protoss_Gateway);

	bool assimilatorFound = false;
	bool forgeFound = false;
	bool cyberFound = false;
	bool suportFound = false;
	bool observatoryFound = false;
	bool citadelFound = false;
	int nexusCounter = 0;

	for (int i = 0; i < s.getBuildingSequence().size(); i++)
	{
		BWAPI::UnitType ut = s.getBuildingSequence().at(i);

		if (ut == BWAPI::UnitTypes::Protoss_Assimilator)
		{
			assimilatorFound = true;
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Forge)
		{
			forgeFound = true;
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
		{
			cyberFound = true;
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Observatory)
		{
			observatoryFound = true;
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Robotics_Support_Bay)
		{
			suportFound = true;
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Nexus)
			nexusCounter++;
		else if (ut == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
			citadelFound = true;
	}
	// We loop through each building that has been build. 
	// For each building, we add the buildings that, that building unlocks to the list of valid buildings.
	for (int i = 0; i < s.getBuildingSequence().size(); i++)
	{
		BWAPI::UnitType ut = s.getBuildingSequence().at(i);

		if (ut == BWAPI::UnitTypes::Protoss_Forge)
		{
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Photon_Cannon);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Gateway)
		{
			if (cyberFound == false)
				validBuildings.push_back(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Cybernetics_Core  && assimilatorFound == true)
		{
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Stargate);
			if (citadelFound == false)
				validBuildings.push_back(BWAPI::UnitTypes::Protoss_Citadel_of_Adun);
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Robotics_Facility);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Robotics_Facility && assimilatorFound == true)
		{
			if (suportFound == false)
				validBuildings.push_back(BWAPI::UnitTypes::Protoss_Robotics_Support_Bay);
			if (observatoryFound == false)
				validBuildings.push_back(BWAPI::UnitTypes::Protoss_Observatory);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Stargate  && assimilatorFound == true)
		{
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Fleet_Beacon);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Citadel_of_Adun  && assimilatorFound == true)
		{
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Templar_Archives);
		}
		else if (ut == BWAPI::UnitTypes::Protoss_Templar_Archives  && assimilatorFound == true)
		{
			validBuildings.push_back(BWAPI::UnitTypes::Protoss_Arbiter_Tribunal);
		}
	}

	if (assimilatorFound == false)
	{
		BWAPI::UnitType assimilator = BWAPI::UnitTypes::Protoss_Assimilator;
		validBuildings.push_back(assimilator);
	}
	//if (forgeFound == false)
	//{
	//	BWAPI::UnitType forge = BWAPI::UnitTypes::Protoss_Forge;
	//	validBuildings.push_back(forge);
	//}
	//if (nexusCounter < NEXUS_LIMIT)
	//{
	//	BWAPI::UnitType nexus = BWAPI::UnitTypes::Protoss_Nexus;
	//	validBuildings.push_back(nexus);
	//}

	return validBuildings;
}