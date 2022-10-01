#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "ForceFollowMission.h"
#include "PounceMission.h"

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
	private:
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		uint32_t m_nObjectID = 0;
		Flight::Object2d m_sObject2dDesc;
		std::vector<olc::vf2d> m_vectTargetPositions;
		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserInput = true;
		Flight::ForceFollowMission m_ForceFollowMission;
		Flight::PounceMission m_PounceMission;
		
		void HandleUserEntry();
		void HandleTestForceFollowMission();
		void HandleTestPounceMission();
		void HandleNormalStart();
	};
}



