#pragma once

#include <boost/lexical_cast.hpp>

#include "Interface.h"
#include "StringBuilder.h"
#include "BaseEventData.h"
#include "Region.h"
#include "BuildOrder.h"
#include "BuildOrderManager.h"
#include "BuildingPlacer.h"
#include "SquadManager.h"

struct ToggleOrderEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Toggle order event. Type: " << BuildOrderManager::Instance().getOrderName(mOrder).c_str();
    }

	explicit ToggleOrderEvent( Order order)
        : mOrder( order )
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
	Order mOrder;
};
struct BuildUnitEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Build unit event. Type: " << mUnit.getName().c_str();
    }

	explicit BuildUnitEvent( BWAPI::UnitType unit, BuildingLocation location = BuildingLocation::Base)
        : mUnit( unit )
		, mNumber(1)
		, mLocation(location)
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	explicit BuildUnitEvent( BWAPI::UnitType unit, int number, BuildingLocation location = BuildingLocation::Base)
        : mUnit( unit )
		, mNumber(number)
		, mLocation(location)
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
	BWAPI::UnitType mUnit;
	int mNumber;
	BuildingLocation mLocation;
};
struct UpgradeEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Upgrade event. Type: " << mUpgrade.getName().c_str();
    }

	explicit UpgradeEvent( BWAPI::UpgradeType upgrade, int level)
        : mUpgrade( upgrade )
		, mLevel(level)
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
	BWAPI::UpgradeType mUpgrade;
	int mLevel;
};
struct SetArmyBehaviourEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Army behaviour event. Type: " << SquadManager::Instance().getArmyBehaviourName(mArmyBehaviour).c_str();
    }

	explicit SetArmyBehaviourEvent( ArmyBehaviour armyBehaviour)
        : mArmyBehaviour( armyBehaviour )
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
	ArmyBehaviour mArmyBehaviour;
};
struct AddProductionEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Add army production unit. Type: " << mUnit.getName().c_str();
    }

	explicit AddProductionEvent( BWAPI::UnitType unit)
        : mUnit( unit )
		, BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
	BWAPI::UnitType mUnit;
};
struct AttackEvent : public BaseEventData
{
    static const EventType sk_EventType;
    virtual const EventType & GetEventType( void ) const
    {
        return sk_EventType;
    }
	
	virtual const std::string ToString( void ) const
    {
		return StringBuilder() << "Add army production unit. ";
    }

	explicit AttackEvent( )
		: BaseEventData(BWAPI::Broodwar->getFrameCount())
    {
    }
	
};