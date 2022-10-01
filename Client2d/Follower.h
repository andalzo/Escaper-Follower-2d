#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "ForceFollowMission.h"
#include "PounceMission.h"
#include "WayPointMission.h"
#include "WayPointCreator.h"
#include <set>

namespace Simulation2d::Net
{
	enum class FollowerUserEntry : uint8_t
	{
		TestForceFollowMission = 1,
		TestPounceMission,
		NormalStart
	};

	enum class FollowerMissions : uint8_t
	{
		ObserveMission = 0,
		ForceFollowMission,
		PounceMission
	};


	class Follower : public olc::PixelGameEngine, public client_interface<MsgTypes>
	{
	public:
		Follower();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;
		void HandleUserEntry();
	private:
		
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		
		uint32_t m_nObjectID = 0;
		Flight::Object2d m_sObject2dDesc;

		uint32_t m_nTargetObjectID = 0;
		Flight::Object2d m_sTargetObject2dDesc; //The copy of target object
	
		std::set<olc::vf2d> m_setTargetPositions; //The route of target object
		
		//This mission is for simulate and test force follow missiom
		Flight::WayPointMission m_WayPointMission;

		bool m_bIsTest = false;
		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserEntry = true;
		
		FollowerMissions m_ActiveMission = FollowerMissions::ObserveMission;
		Flight::ForceFollowMission m_ForceFollowMission;
		Flight::PounceMission m_PounceMission;

		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };
		

		void HandleTestForceFollowMission();
		void HandleTestPounceMission();
		void HandleNormalStart();

		void HandleIncomingMessages();
		void OnObserveMission();
		void OnForceFollowMission();
		void OnPounceMission();

		void DrawWorldObjects(const bool&);
	};
}



