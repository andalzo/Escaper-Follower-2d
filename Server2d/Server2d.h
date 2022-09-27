#pragma once
#include "cevheri_net.h"
#include "Common/common2d.h"

namespace Cevheri::Net
{
	class GroundStationServer : public server_interface<MsgTypes>
	{
	public:
		GroundStationServer(uint16_t nPort); // Setting up station with suitable port
		std::unordered_map<uint32_t, Flight::Drone> m_mapDroneRoster; //Drones data the server
		std::vector<uint32_t> m_vGarbageIDs; //The vector of ids of drones disconnected, to infrom other drones

	protected:
		void OnClientValidated(std::shared_ptr<connection<MsgTypes>> client) override; //Validation override, async
		bool OnClientConnect(std::shared_ptr<connection<MsgTypes>> client) override; //Connection override, async
		void OnClientDisconnect(std::shared_ptr<connection<MsgTypes>> client) override; //Disconnection override, async
		
		//message handler function of drone-to-drone and server-to-drone, async
		void OnMessage(std::shared_ptr<connection<MsgTypes>> client, message<MsgTypes>& msg) override; 
	};
}



