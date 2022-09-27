#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "ForceFollowMission.h"
#include "PounceMission.h"

namespace Simulation2d::Net
{

	class Follower : public olc::PixelGameEngine, public client_interface<MsgTypes>
	{
	public:
		Follower();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;
	private:
		std::unordered_map<uint32_t, Flight::Object2d> mapObjects;
		uint32_t nObjectID = 0;
		Flight::Object2d sObject2dDesc;
		bool bWaitingForConnection = true;
		Flight::ForceFollowMission m_ForceFollowMission;
		Flight::PounceMission m_PounceMission;
	};
}



