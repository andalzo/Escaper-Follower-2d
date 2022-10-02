#pragma once
#include "simulation2d_net.h"
#include "common2d.h"
#include "WayPointMission.h"
#include "WaypointCreator.h"
#include "EscapeMission.h"

namespace Simulation2d::Net
{
	enum EscaperMissions
	{
		WayPointMission,
		EscapeMission,
		DestroyMission
	};

	class Escaper : public olc::PixelGameEngine, public client_interface<SimMsg>
	{
	public: //Methods
		Escaper();

		void HandleUserEntry();
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;

	private: // Members
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		
		uint32_t m_nObjectId = 0;
		Flight::Object2d m_sObject2dDesc;
		EscaperMissions m_ActiveMission = EscaperMissions::WayPointMission;


		bool m_bWaitingForConnection = true;
		bool m_bWaitingForUserEntry = true;
		bool m_bIsTest = false;
		bool m_bIsTestWayPoint = false;
		
		Flight::WayPointMission m_WayPointMission;
		Flight::EscapeMission m_EscapeMission;
		

		olc::TileTransformedView tv;
		olc::vi2d m_vWorldSize = { Flight::World_X_Limit, Flight::World_Y_Limit };

		void HandleIncomingMessages();
		
		void AssignObjectID(const uint32_t& id);

		void OnWayPointMission();
		void OnEscapeMission();
		void OnDestroyMission();
		void DrawWorldObjects();

		void HandleUserEntryWayPoint();
		void HandleManualUserEntryWayPoint();
		void HandleRandomUserEntryWayPoint();
		void HandleSimulationEntryWayPoint();

		void HandleUserEntryEscape();

		void HandleRandomWayPoint(const int& limBelow, const int& limUpper);

		void StartSimulation();
	};
}




