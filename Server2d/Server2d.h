#pragma once
#include "simulation2d_net.h"
#include "common2d.h"

namespace Simulation2d::Net
{
	class Server2d : public server_interface<SimMsg>
	{
	public:
		Server2d(uint16_t nPort); // Setting up station with suitable port
	protected:
		bool OnClientConnect(std::shared_ptr<connection<SimMsg>> client) override;
		void OnClientValidated(std::shared_ptr<connection<SimMsg>> client) override;
		void OnClientDisconnect(std::shared_ptr<connection<SimMsg>> client) override;
		
		//message handler function of object-to-object and server-to-object async
		void OnMessage(std::shared_ptr<connection<SimMsg>> client, message<SimMsg>& msg) override; 
	private:
		std::unordered_map<uint32_t, Flight::Object2d> m_mapObjects;
		std::vector<uint32_t> m_vectGarbageIDs;
	};
}



