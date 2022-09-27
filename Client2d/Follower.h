#pragma once
#include "cevheri_net.h"
#include "Common/common2d.h"

namespace Cevheri::Net
{

	enum class FollowerState : uint8_t
	{
		UserEntry, //in waiting user entry for in being in server
		RandomEntry, //user choiced random entry for my sim
		Simulation //In simulation mode
	};


	class Follower : public olc::PixelGameEngine, public client_interface<MsgTypes>
	{
	public:
		Follower();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;
	private:
		std::unordered_map<uint32_t, Flight::Drone> mapDrones;
		uint32_t nDroneId = 0;
		Flight::Drone sDroneDesc;
		bool bWaitingForConnection = true;

		FollowerState m_State = FollowerState::UserEntry;
		std::unique_ptr<Flight::Mission> ActiveMission;
	};
}



