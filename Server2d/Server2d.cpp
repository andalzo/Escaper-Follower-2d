#include "Server2d.h"

namespace Simulation2d::Net
{
	Server2d::Server2d(uint16_t nPort) : server_interface<SimMsg>(nPort)
	{

	}
	
	bool Server2d::OnClientConnect(std::shared_ptr<connection<SimMsg>> client)
	{
		// Our library executes validation, so we can allow all in this stage
		return true;
	}

	void Server2d::OnClientValidated(std::shared_ptr<connection<SimMsg>> client)
	{
		//Client passed validation check, so send them a message informing them they can communicate
		message<SimMsg> msg;
		msg.header.id = SimMsg::Client_Accepted;
		MessageClient(client, msg);
	}

	void Server2d::OnClientDisconnect(std::shared_ptr<connection<SimMsg>> client)
	{
		if (client)
		{
			if (m_mapObjects.find(client->GetID()) == m_mapObjects.end())
			{
				// client never added to roster, so just let it disappear
			}
			else
			{
				Flight::Object2d sDisconnectedObject = m_mapObjects.at(client->GetID());
				
				//We delete the object from server map
				std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(sDisconnectedObject.nUniqueID) + "\n";
				m_mapObjects.erase(client->GetID());

				//We need to keep id to send others to inform about this removal
				//We need a vector here because, more than one client can be disconnected at the same time
				m_vectGarbageIDs.push_back(client->GetID());
			}
		}
	}

	void Server2d::OnMessage(std::shared_ptr<connection<SimMsg>> client, message<SimMsg>& msg)
	{
		if (!m_vectGarbageIDs.empty())
		{
			for (uint32_t nDisconnectedobjectId : m_vectGarbageIDs)
			{
				message<SimMsg> msgRemovePlayer;
				msgRemovePlayer.header.id = SimMsg::Simulation_RemoveObject;
				msgRemovePlayer << nDisconnectedobjectId;
				std::cout << "Removing " << nDisconnectedobjectId << "\n";
				MessageAllClients(msgRemovePlayer);
			}
			m_vectGarbageIDs.clear();
		}

		switch (msg.header.id)
		{
			case SimMsg::Client_RegisterWithServer:
			{
				//We get the description of object
				Flight::Object2d sObject2dDesc;
				msg >> sObject2dDesc;

				//We assign this object an unique id
				sObject2dDesc.nUniqueID = client->GetID();
				m_mapObjects.insert_or_assign(sObject2dDesc.nUniqueID, sObject2dDesc);

				//We send this unique ID to ourt client
				message<SimMsg> msgSendID;
				msgSendID.header.id = SimMsg::Client_AssignID;
				msgSendID << sObject2dDesc.nUniqueID;
				MessageClient(client, msgSendID);

				//We send this object to all clients to add their map objects, including this client
				message<SimMsg> msgAddPlayer;
				msgAddPlayer.header.id = SimMsg::Simulation_AddObject;
				msgAddPlayer << sObject2dDesc;
				MessageAllClients(msgAddPlayer);

				//We send all the clients who connected to server to client who has been just connected.
				for (const auto& player : m_mapObjects)
				{
					message<SimMsg> msgAddOtherPlayers;
					msgAddOtherPlayers.header.id = SimMsg::Simulation_AddObject;
					msgAddOtherPlayers << player.second;
					MessageClient(client, msgAddOtherPlayers);
				}

				break;
			}
			case SimMsg::Client_PounceStart:
			{
				//TODO: Inform alla clients except this client that
				//a specific escaper object witn specific id is in pounce mode now
				MessageAllClients(msg, client);
				break;
			}

			case SimMsg::Client_PounceCancel:
			{
				//TODO: Inform alla clients except this client that
				//a specific escaper object witn specific id out of pounce mode now
				MessageAllClients(msg, client);
				break;
			}

			case SimMsg::Client_PounceSuccesful:
			{
				//TODO: Inform alla clients except this client that
				//a specific escaper obkect witn specific id is pounced now
				MessageAllClients(msg, client);
				break;
			}

			case SimMsg::Simulation_UpdateObject:
			{
				// Simply bounce update to everyone except incoming client
				MessageAllClients(msg, client);
				break;
			}
			
		}

	}
}


