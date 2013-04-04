#include "Cromartie.h"

#include <iostream>

#include "ScoreHelper.h"

#include "DrawBuffer.h"
#include "TerrainAnalysis.h"
#include "UnitTracker.h"
#include "BaseTracker.h"
#include "BuildingPlacer.h"
#include "TaskManager.h"
#include "ResourceTracker.h"
#include "ResourceManager.h"
#include "TrainTask.h"
#include "ConstructionTask.h"
#include "MorphTask.h"
#include "WallTracker.h"
#include "BuildOrderManager.h"
#include "PlayerTracker.h"
#include "BorderTracker.h"
#include "ScoutManager.h"
#include "SupplyManager.h"
#include "MacroManager.h"
#include "ExpansionManager.h"
#include "UnitInformation.h"
#include "UnitHelper.h"
#include "LatencyTracker.h"
#include "PylonPowerTracker.h"
#include "MapHelper.h"
#include "PathFinder.h"
#include "Logger.h"
#include "GameProgressDetection.h"
#include "AOEThreatTracker.h"
#include "SquadManager.h"
#include "BlockedPathManager.h"
#include "GameMemory.h"
#include "EventManager.h"
#include "GenericEvents.h"
#include "FastDelegate.h"
#include "Hypothalamus.h"
#include "HypothalamusEvents.h"

using namespace fastdelegate;


Cromartie::Cromartie()
	: mOnBegin(false)
	, mLeavingGame(0)
	, mSaidGG(false)
{
	using namespace BWAPI::Races;
	using namespace BWAPI::UnitTypes;
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;

	int ID_1 = 0;
	int ID_2 = 0;
	int ID_3 = 0;

	BuildOrder dtRush(Protoss, BuildOrderID::LoadIn, "Fast Reaver");

	//Army Behaviour
	dtRush.addArmyBehaviour(ArmyBehaviour::Defensive);

	// Build Order
			dtRush.addItem(Protoss_Probe, 4, TaskType::Highest);
	ID_1 =	dtRush.addItem(Protoss_Pylon, 1); //Pylon on 8
			dtRush.addItem(Protoss_Probe, CB(ID_1, CallBackType::onDispatched), 2); //probage

	ID_1 =	dtRush.addItem(Protoss_Gateway, CB(ID_1, CallBackType::onDispatched)); //Gateway on 10
	ID_2 =	dtRush.addItem(Protoss_Assimilator, CB(ID_1, CallBackType::onDispatched)); //gas on 11

			dtRush.addOrder(Order::TrainWorkers, CB(ID_2, CallBackType::onDispatched));
	ID_2 =	dtRush.addItem(Protoss_Cybernetics_Core, CB(ID_1, CallBackType::onDispatched)); //core on 13
			dtRush.addOrder(Order::Scout, CB(ID_1, CallBackType::onStarted));

	ID_1 =	dtRush.addItem(Protoss_Pylon, TaskType::Supply, CB(ID_2, CallBackType::onDispatched)); //pylon on 15
			dtRush.addItem(Protoss_Dragoon, 4); //dragoon on 18

	ID_1 =	dtRush.addItem(Protoss_Pylon, TaskType::Supply, CB(ID_2, CallBackType::onDispatched)); //pylon on 21
	
	ID_2 =	dtRush.addItem(Protoss_Robotics_Facility, CB(ID_1, CallBackType::onDispatched)); // Robo Fac on 26
	ID_1 =	dtRush.addItem(Protoss_Pylon, TaskType::Supply, CB(ID_2, CallBackType::onDispatched)); //pylon on 29
	
	ID_2 =	dtRush.addItem(Protoss_Robotics_Support_Bay, CB(ID_1, CallBackType::onDispatched)); //support bay on 34
	
			dtRush.addArmyBehaviour(ArmyBehaviour::Aggresive);

			dtRush.addOrder(Order::SupplyManager, CB(ID_2, CallBackType::onDispatched));
			dtRush.addOrder(Order::RefineryManager, CB(ID_2, CallBackType::onDispatched));
			dtRush.addOrder(Order::MacroArmyProduction, CB(ID_2, CallBackType::onDispatched));
			
	dtRush.addNextBuild(BuildOrderID::Nexus, 24*60*3);
	dtRush.addProduce(Protoss_Dragoon, 1);
	dtRush.addProduce(Protoss_Reaver, 1, 1000);

	loadedBO = dtRush;
}

Cromartie::~Cromartie(void)
{
	delete eventRecorderListener;
}

void Cromartie::onStart()
{
	BWAPI::Broodwar->sendText("Cromartie 1.0 Operational");

	BWAPI::Broodwar->setLatCom(false);
	BWAPI::Broodwar->setCommandOptimizationLevel(1);

	if(BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Protoss)
		BWAPI::Broodwar->sendText("Cromartie is a Protoss only bot");

	if(BWAPI::Broodwar->getRevision() != BWAPI::BWAPI_getRevision())
	{
		BWAPI::Broodwar->sendText("This version of Cromartie was compiled for BWAPI rev. %d", BWAPI::BWAPI_getRevision());
		BWAPI::Broodwar->sendText("But BWAPI rev. %d is currently running", BWAPI::Broodwar->getRevision());
	}

	BWAPI::Broodwar->setLocalSpeed(0);

	BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

	
	registerListeners();
}

void Cromartie::onEnd(bool isWinner)
{
	BWAPI::Broodwar->sendText("Ending GA...");
	_ga.onGameEnd(isWinner, ScoreHelper::getPlayerScore(), ScoreHelper::getOpponentScore(), BWAPI::Broodwar->getFrameCount(), 60*60*24);
	BWAPI::Broodwar->sendText("GA Ended");

	BuildOrderManager::Instance().onEnd(isWinner);
	GameMemory::Instance().onEnd();
}

void Cromartie::onFrame()
{
	// Enqueue bwapi events
	for each(BWAPI::Event bwapiEvent in BWAPI::Broodwar->getEvents())
	{
		if(bwapiEvent.getType() == BWAPI::EventType::UnitDiscover)
			EQUEUE( new UnitDiscoveredEvent(bwapiEvent.getUnit()));
		if(bwapiEvent.getType() == BWAPI::EventType::UnitDestroy)
			EQUEUE( new UnitDestroyedEvent(bwapiEvent.getUnit()));
		if(bwapiEvent.getType() == BWAPI::EventType::UnitComplete)
			EQUEUE( new UnitCompleteEvent(bwapiEvent.getUnit()));
		if(bwapiEvent.getType() == BWAPI::EventType::UnitMorph)
			EQUEUE( new UnitMorphEvent(bwapiEvent.getUnit()));
	}

	if(!mOnBegin)
	{
		BWAPI::Broodwar->sendText("Starting GA...");
		_ga.onStarcraftStart();
		BWAPI::Broodwar->sendText("GA started!");
		mOnBegin = true;
		EQUEUE( new OnStartEvent() );
		EQUEUE( new PauseBuildOrderEvent() );
		//EQUEUE( new AddBuildOrderEvent(&loadedBO));

		EQUEUE( new ToggleOrderEvent(Order::SupplyManager) );
		EQUEUE( new ToggleOrderEvent(Order::TrainWorkers) );
		EQUEUE( new ToggleOrderEvent(Order::MacroArmyProduction) );
		EQUEUE( new ToggleOrderEvent(Order::Scout) );
		EQUEUE( new AddProductionEvent(BWAPI::UnitTypes::Protoss_Zealot) );

		EQUEUE( new BuildUnitEvent(BWAPI::UnitTypes::Protoss_Gateway) );

		EQUEUE( new ToggleDebugInfoEvent() );
	}
	

	EQUEUE( new OnUpdateEvent() );
	EventManager::Instance().update();

	if(GameProgressDetection::Instance().shouldGG())
	{
		if(mLeavingGame == 0)
			mLeavingGame = BWAPI::Broodwar->getFrameCount();
	}
	else
		mLeavingGame = 0;

	if(mLeavingGame != 0)
	{
		if(BWAPI::Broodwar->getFrameCount() - mLeavingGame > 24 && !mSaidGG)
		{
			mSaidGG = true;
			BWAPI::Broodwar->sendText("gg");
		}
		else if(BWAPI::Broodwar->getFrameCount() - mLeavingGame > 80)
			BWAPI::Broodwar->leaveGame();
	}
}

void Cromartie::onSendText(std::string text) 
{
	Hypothalamus::Instance().vocalCommand(text);

	BWAPI::Broodwar->sendText(text.c_str());
}

void Cromartie::onPlayerLeft(Player player)
{
	PlayerTracker::Instance().onPlayerLeft(player);
}

#define ADDLISTENER(instance, functionPointer, eventType) EventManager::Instance().AddListener(MakeDelegate(instance, functionPointer), eventType);

void Cromartie::registerListeners()
{
	// Event log listener
	eventRecorderListener = new EventRecorder("bwapi-data\\logs\\Cromartie_EventLog.txt");
	ADDLISTENER(eventRecorderListener, &EventRecorder::RecordEvent, CatchAllEvents);

	// Basic unit event listeners
	ADDLISTENER(&UnitTracker::Instance(), &UnitTrackerClass::onUnitDiscoverEvent, UnitDiscoveredEvent::sk_EventType);
	ADDLISTENER(&UnitTracker::Instance(), &UnitTrackerClass::onUnitDestroyEvent, UnitDestroyedEvent::sk_EventType);
	
	// Listen to OnStart events
	ADDLISTENER(&WallTracker::Instance(), &WallTrackerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&PlayerTracker::Instance(), &PlayerTrackerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&TerrainAnalysis::Instance(), &TerrainAnalysisClass::analyseBasesAndTerrain, OnStartEvent::sk_EventType);
	ADDLISTENER(&BorderTracker::Instance(), &BorderTrackerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&UnitTracker::Instance(), &UnitTrackerClass::pumpUnitEvents, OnStartEvent::sk_EventType);
	ADDLISTENER(&ScoutManager::Instance(), &ScoutManagerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&MacroManager::Instance(), &MacroManagerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&GameMemory::Instance(), &GameMemoryClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::onBegin, OnStartEvent::sk_EventType);
	ADDLISTENER(&BlockedPathManager::Instance(), &BlockedPathManagerClass::onBegin, OnStartEvent::sk_EventType);
	
	// Listen to Update events
	ADDLISTENER(&UnitTracker::Instance(), &UnitTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&AOEThreatTracker::Instance(), &AOEThreatTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&BlockedPathManager::Instance(), &BlockedPathManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&PylonPowerTracker::Instance(), &PylonPowerTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&UnitInformation::Instance(), &UnitInformationClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&BorderTracker::Instance(), &BorderTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&PlayerTracker::Instance(), &PlayerTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&BaseTracker::Instance(), &BaseTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&BuildingPlacer::Instance(), &BuildingPlacerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&ResourceManager::Instance(), &ResourceManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&SupplyManager::Instance(), &SupplyManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&MacroManager::Instance(), &MacroManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&ExpansionManager::Instance(), &ExpansionManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&LatencyTracker::Instance(), &LatencyTrackerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&GameProgressDetection::Instance(), &GameProgressDetectionClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&ScoutManager::Instance(), &ScoutManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&SquadManager::Instance(), &SquadManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&TaskManager::Instance(), &TaskManagerClass::update, OnUpdateEvent::sk_EventType);
	ADDLISTENER(&DrawBuffer::Instance(), &DrawBufferClass::update, OnUpdateEvent::sk_EventType);
	
	// Build Order Manager listeners
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::changeCurrentBuildEvent, ChangeBuildOrderEvent::sk_EventType);
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::addBuildEvent, AddBuildOrderEvent::sk_EventType);
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::pauseBuild, PauseBuildOrderEvent::sk_EventType);
	ADDLISTENER(&BuildOrderManager::Instance(), &BuildOrderManagerClass::continueBuild, ContinueBuildOrderEvent::sk_EventType);

	// Listen for toggle debug
	ADDLISTENER(&SquadManager::Instance(), &SquadManagerClass::toggleDebug, ToggleDebugInfoEvent::sk_EventType);
	ADDLISTENER(&BaseTracker::Instance(), &BaseTrackerClass::toggleDebug, ToggleDebugInfoEvent::sk_EventType);

	// Hypothalamus event listeners
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::buildUnitEvent, BuildUnitEvent::sk_EventType);
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::toggleOrderEvent, ToggleOrderEvent::sk_EventType);
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::upgradeEvent, UpgradeEvent::sk_EventType);
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::setArmyBehaviourEvent, SetArmyBehaviourEvent::sk_EventType);
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::addProductionEvent, AddProductionEvent::sk_EventType);
	ADDLISTENER(&Hypothalamus::Instance(), &HypothalamusClass::attack, AttackEvent::sk_EventType);

	ADDLISTENER(&_ga, &GA::onUnitCompleteEvent, UnitCompleteEvent::sk_EventType);
	ADDLISTENER(&_ga, &GA::onMorph, UnitMorphEvent::sk_EventType); 
}