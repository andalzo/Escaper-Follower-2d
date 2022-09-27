#include "GroundStationServer.h"

namespace Cevheri::Net
{
	GroundStationServer::GroundStationServer(uint16_t nPort) : server_interface<MsgTypes>(nPort)
	{

	}

	void GroundStationServer::OnClientValidated(std::shared_ptr<connection<MsgTypes>> client)
	{
	}

	bool GroundStationServer::OnClientConnect(std::shared_ptr<connection<MsgTypes>> client)
	{
		return false;
	}

	void GroundStationServer::OnClientDisconnect(std::shared_ptr<connection<MsgTypes>> client)
	{
	}

	void GroundStationServer::OnMessage(std::shared_ptr<connection<MsgTypes>> client, message<MsgTypes>& msg)
	{
	}



}


