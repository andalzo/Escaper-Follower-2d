#pragma once
#include "simulation2d_net.h"
#include "common2d.h"

namespace Simulation2d::Net
{
	class GroundStationServer : public server_interface<MsgTypes>
	{
	public:
		GroundStationServer(uint16_t nPort); // Setting up station with suitable port
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects; 
		std::vector<uint32_t> m_vectGarbageIDs;

	protected:
		void OnClientValidated(std::shared_ptr<connection<MsgTypes>> client) override;
		bool OnClientConnect(std::shared_ptr<connection<MsgTypes>> client) override; 
		void OnClientDisconnect(std::shared_ptr<connection<MsgTypes>> client) override;
		
		//message handler function of drone-to-drone and server-to-drone, async
		void OnMessage(std::shared_ptr<connection<MsgTypes>> client, message<MsgTypes>& msg) override; 
	};
}



