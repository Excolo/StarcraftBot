#pragma once

#include "Interface.h"
#include "BuildingPlacer.h"

#include <string>
#include "Singleton.h"

#include "SquadManager.h"
#include "BuildOrder.h"
#include "IEventData.h"

// This is the part of the "virtual" brain that controls the most basic urges of Cromartie.
class HypothalamusClass
{
public:

	// Event handlers
	void buildUnitEvent(IEventDataPtr evnt);
	void toggleOrderEvent(IEventDataPtr evnt);
	void upgradeEvent(IEventDataPtr evnt);
	void setArmyBehaviourEvent(IEventDataPtr evnt);
	void addProductionEvent(IEventDataPtr evnt);

	void buildUnit(BWAPI::UnitType unit, BuildingLocation position = BuildingLocation::Base);
	void buildUnit(BWAPI::UnitType unit, TaskType taskType, BuildingLocation position = BuildingLocation::Base);
	void buildUnit(BWAPI::UnitType unit, int number, BuildingLocation position = BuildingLocation::Base);
	void buildUnit(BWAPI::UnitType unit, TaskType taskType, int number, BuildingLocation position = BuildingLocation::Base);
	void upgrade(BWAPI::UpgradeType type, int level);
	void upgrade(BWAPI::TechType type);
	void setArmyBehaviour(ArmyBehaviour armyBehaviour);
	void toggleOrder(Order order);
	void addProduction(BWAPI::UnitType unit);
	void attack();
	void stop();

	void vocalCommand(std::string command);

private:
	std::list<UnitToProduce> mProduces;

	void parseBuildCommand(std::string building);
	void parseTrainCommand(std::string unit);
	void parseOrderCommand(std::string order);
	void parseArmyBehaviourCommand(std::string armyBehaviour);
	void parseProductionCommand(std::string unit);
	void parseUpgradeCommand(std::string upgrade);
};

typedef Singleton<HypothalamusClass> Hypothalamus;