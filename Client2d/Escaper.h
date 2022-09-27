#pragma once
#include "cevheri_net.h"
#include "common2d.h"
#include "WayPointMission.h"
#include "WaypointCreator.h"
#include "EscapeMission.h"

namespace Cevheri::Net
{
	enum UserEntry : uint8_t
	{
		ManualEntry = 1,
		RandomEntry = 2,
		TestSimulation = 3
	};

	class Escaper : public olc::PixelGameEngine, public client_interface<MsgTypes>
	{
	public: //Methods
		Escaper();

		void HandleUserEntry();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;

	private: // Members
		std::unordered_map<uint32_t, Flight::Drone> m_mapDrones;
		uint32_t m_nDroneId = 0;
		Flight::Drone m_sDroneDesc;
		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserEntry = true;
		Flight::WayPointMission WPM;
		std::unique_ptr<Flight::Mission> m_ActiveMission;
		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };
		std::vector<olc::vf2d> m_WayPoints;

		void HandleManualUserEntry();
		void HandleRandomUserEntry();
		void HandleSimulationEntry();
		void HandleUserEntryStartSimulation(const Flight::WayPointMission& WPM);
	};
}




