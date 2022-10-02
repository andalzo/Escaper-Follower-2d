#pragma once
#include <set>
#include <chrono>
#include "simulation2d_net.h"
#include "common2d.h"
#include "ForceFollowMission.h"
#include "WayPointMission.h"
#include "WayPointCreator.h"
#include "EscapeMission.h"


namespace Simulation2d::Net
{

	enum class FollowerMissions : uint8_t
	{
		ObserveMission = 0,
		ForceFollowMission,
		PounceMission
	};


	class Follower : public olc::PixelGameEngine, public client_interface<SimMsg>
	{
	public:
		Follower();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;
		void HandleUserEntry();
	private:
		
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		
		uint32_t m_nObjectId = 0;
		Flight::Object2d m_sObject2dDesc;

		uint32_t m_nTargetObjectID = 0;
	
		std::set<olc::vf2d> m_setTargetPositions; //The route of target object
		
		//These missions are for simulate and test force follow missiom
		Flight::WayPointMission m_WayPointMission;
		Flight::EscapeMission m_EscapeMission;

		bool m_bIsTest = false;
		bool m_bIsTestForceFollow = true;
		bool m_bWaitingForUserEntry = true;
		bool m_bWaitingForConnection = true;

		std::string m_strMode;
		
		FollowerMissions m_ActiveMission = FollowerMissions::ObserveMission;
		Flight::ForceFollowMission m_ForceFollowMission;
		std::chrono::time_point<std::chrono::system_clock> m_tpPounceStart;

		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };
		
		void HandleIncomingMessages();

		void AssignObjectID(const uint32_t& id);

		void OnObserveMission();
		void OnForceFollowMission();
		void OnPounceMission();

		void DrawWorldObjects(const bool& b);

		void HandleTestForceFollowMission();
		void HandleTestWithWayPointMission();
		void HandleTestWithEscapeMission();
		
		void StartSimulation();
	};
}



