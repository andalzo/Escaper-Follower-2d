#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "WayPointMission.h"
#include "WaypointCreator.h"
#include "EscapeMission.h"

namespace Simulation2d::Net
{
	enum EscaperUserEntry : uint8_t
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
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		uint32_t m_nDroneId = 0;
		Flight::Object2d m_sObject2dDesc;
		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserEntry = true;
		Flight::WayPointMission m_WayPointMission;
		Flight::EscapeMission m_EscapeMission;
		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };

		void HandleManualUserEntry();
		void HandleRandomUserEntry();
		void HandleSimulationEntry();
		void HandleUserEntryStartSimulation();
	};
}




