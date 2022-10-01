#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "ForceFollowMission.h"
#include "PounceMission.h"
#include "WayPointMission.h"
#include <set>

namespace Simulation2d::Net
{
	enum class FollowerUserEntry : int
	{
		TestForceFollowMission = 1,
		TestPounceMission,
		NormalStart
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
		
		
		std::set<olc::vf2d> m_setTargetPositions;
		Flight::Object2d m_sObject2dEscaperDesc;
		Flight::WayPointMission m_WayPointMission;

		bool m_bIsTest = false;
		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserEntry = true;
		
		Flight::ForceFollowMission m_ForceFollowMission;
		Flight::PounceMission m_PounceMission;

		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };
		

		void HandleTestForceFollowMission();
		void HandleTestPounceMission();
		void HandleNormalStart();

		void DrawWorldObjects(const bool&);
	};
}



