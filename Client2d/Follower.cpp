#include "Follower.h"

namespace Simulation2d::Net
{
	Follower::Follower()
	{
		sAppName = "Follower2d";
		
		m_sObject2dDesc.type = Flight::Object2d::Type::Follower;
		m_sObject2dDesc.Color = olc::GREEN;
	}
	
	bool Follower::OnUserCreate()
	{
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1, 1 });

		if (Connect("192.168.1.51", 60000))
		{
			return true;
		}
		return false;
	}

	bool Follower::OnUserUpdate(float fElapsedTime)
	{

		bool bIsDrawTargetRoute = false;
		if (GetMouse(0).bHeld)
		{
			bIsDrawTargetRoute = !bIsDrawTargetRoute;
		}
		
		if (m_bIsTest)
		{
			if (m_bIsTestForceFollow)
			{

				m_WayPointMission.Execute();
				m_setTargetPositions.insert(m_mapObjects.at(m_nTargetObjectID).Position);
				m_ForceFollowMission.SetTargetPosition(m_mapObjects.at(m_nTargetObjectID).Position);
				m_ForceFollowMission.Execute();
			}
			else
			{
				m_EscapeMission.Execute();
				m_setTargetPositions.insert(m_mapObjects.at(m_nTargetObjectID).Position);
				m_ForceFollowMission.SetTargetPosition(m_mapObjects.at(m_nTargetObjectID).Position);
				m_ForceFollowMission.Execute();

			}

			DrawWorldObjects(bIsDrawTargetRoute);

			return true; //We finished creating this frame succesfully in normal client-server mode
		}
		else
		{
			//Handle Incoming Messages
			HandleIncomingMessages();

			if (m_bWaitingForConnection)
			{
				Clear(olc::DARK_BLUE);
				DrawString({ 10,10 }, "Waiting To Connect...", olc::WHITE);
				return true;
			}

			//State Machine
			switch (m_ActiveMission)
			{
			case FollowerMissions::ObserveMission:
				OnObserveMission();
				break;
			case FollowerMissions::ForceFollowMission:
				OnForceFollowMission();
				break;
			case FollowerMissions::PounceMission:
				OnPounceMission();
				break;
			default:
				std::cerr << "\n[ERROR]:Impossible active mission state for follower.\n";
				std::exit(-1);
				break;
			}

			//Drawing the objects
			DrawWorldObjects(bIsDrawTargetRoute);

			//Final update message to the server
			message<SimMsg> msg;
			msg.header.id = SimMsg::Simulation_UpdateObject;
			msg << m_mapObjects.at(m_nObjectId);
			Send(msg);

			return true; //We finished creating this frame succesfully in test mode
		}
		
	}

	void Follower::HandleIncomingMessages()
	{
		if (IsConnected())
		{
			while (!InComing().empty())
			{
				message<SimMsg> msg = InComing().pop_front().msg;
				switch (msg.header.id)
				{
					case SimMsg::Client_Accepted:
					{
						std::cout << "Server accepted client - you're in!\n";
						message<SimMsg> msg;
						msg.header.id = SimMsg::Client_RegisterWithServer;
						msg << m_sObject2dDesc;
						Send(msg);
						break;
					}
					case SimMsg::Client_AssignID:
					{
						msg >> m_nObjectId;
						std::cout << "Assigned Client ID: " << m_nObjectId << "\n";
						break;
					}

					case SimMsg::Simulation_AddObject:
					{
						Flight::Object2d sNewObject;
						msg >> sNewObject;
						if (sNewObject.nUniqueID == m_nObjectId)
						{
							//Now we exist in the game world
							m_bWaitingForConnection = false;
							m_sObject2dDesc = sNewObject;
							AssignObjectID(m_sObject2dDesc.nUniqueID);
							break;
						}
						
						m_mapObjects.insert_or_assign(sNewObject.nUniqueID, sNewObject);
						break;
					}
					case SimMsg::Simulation_RemoveObject:
					{
						uint32_t nRemovalId = 0;
						msg >> nRemovalId;
						m_mapObjects.erase(nRemovalId);
						break;
					}
					case SimMsg::Simulation_UpdateObject:
					{
						Flight::Object2d sUpdatedObject;
						msg >> sUpdatedObject;
						m_mapObjects.insert_or_assign(sUpdatedObject.nUniqueID, sUpdatedObject);
						break;
					}
					case SimMsg::Client_PounceStart:
					{
						uint32_t id;
						msg >> id;
						m_mapObjects.at(id).bPounced = true;
					}
					case SimMsg::Client_PounceCancel:
					{
						uint32_t id;
						msg >> id;
						m_mapObjects.at(id).bPounced = false;
					}
					case SimMsg::Client_PounceSuccesful:
					{
						uint32_t id;
						msg >> id;
						m_mapObjects.erase(id);
					}
				}
			}
		}
	}

	void Follower::AssignObjectID(const uint32_t& id)
	{
		m_nObjectId = id;
		m_mapObjects.insert_or_assign(m_nObjectId, m_sObject2dDesc);
		m_ForceFollowMission.SetObject2d(&m_mapObjects.at(m_nObjectId));
	}

	void Follower::OnObserveMission()
	{
		m_strMode = "Observing...";

		if (m_mapObjects.empty())
		{
			m_ActiveMission = FollowerMissions::ObserveMission;
			return;
		}

		uint32_t nPossibleTargetID;
		bool isEscaperFound = false;
		for (const auto& object : m_mapObjects)
		{
			if (object.second.type == Flight::Object2d::Type::Escaper)
			{
				bool isEscaperFound = true;
				nPossibleTargetID = object.second.nUniqueID;
			}
		}

		if (isEscaperFound)
		{
			m_ActiveMission = FollowerMissions::ForceFollowMission;
			m_nTargetObjectID = nPossibleTargetID;
			return;
		}
		else
		{
			m_ActiveMission = FollowerMissions::ObserveMission;
			return;
		}
		
	}

	void Follower::OnForceFollowMission()
	{
		m_strMode = "Force Following...";

		m_ForceFollowMission.SetTargetPosition(m_mapObjects.at(m_nTargetObjectID).Position);
		
		m_ForceFollowMission.Execute();
		
		if (m_ForceFollowMission.IsForceFollowSucces())
		{
			m_ActiveMission = FollowerMissions::PounceMission;
			m_mapObjects.at(m_nTargetObjectID).bPounced = true;

			message<SimMsg> msgPounceStarted;
			msgPounceStarted.header.id = SimMsg::Client_PounceStart;
			msgPounceStarted << m_nTargetObjectID;
			Send(msgPounceStarted);

			m_tpPounceStart = std::chrono::system_clock::now();
		}
		else
		{
			m_ActiveMission = FollowerMissions::ForceFollowMission;
		}
	}

	void Follower::OnPounceMission()
	{
		m_strMode = "Trying To Pounce..";

		std::chrono::time_point<std::chrono::system_clock> tpPossiblePounceEnd = std::chrono::system_clock::now();
		int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(tpPossiblePounceEnd - m_tpPounceStart).count();
		if (duration >= 240 && IsTargetInPounceArea())
		{
			m_mapObjects.at(m_nTargetObjectID).bPounceSucces = true;
			
			message<SimMsg> msgPounceFinished;
			msgPounceFinished.header.id = SimMsg::Client_PounceSuccesful;
			msgPounceFinished << m_nTargetObjectID;
			Send(msgPounceFinished);
			
			m_ActiveMission = FollowerMissions::ObserveMission;
			return;
		}
		else
		{
			m_ForceFollowMission.SetTargetPosition(m_mapObjects.at(m_nTargetObjectID).Position);
			m_ForceFollowMission.Execute();

			if (!IsTargetInPounceArea())
			{
				m_ActiveMission = FollowerMissions::ForceFollowMission;
				message<SimMsg> msgPounceCanceled;
				msgPounceCanceled.header.id = SimMsg::Client_PounceCancel;
				msgPounceCanceled << m_nTargetObjectID;
				Send(msgPounceCanceled);

			}
		}
	}

	bool Follower::IsTargetInPounceArea()
	{
		olc::vf2d vf2dDistance = m_mapObjects.at(m_nObjectId).Position - m_mapObjects.at(m_nTargetObjectID).Position;
		float fDistance = vf2dDistance.mag();
		return (fDistance <= 3 * m_mapObjects.at(m_nObjectId).fRadius);
	}

	void Follower::DrawWorldObjects(const bool& bIsDrawTargetRoute)
	{

		Clear(olc::DARK_BLUE);

		DrawString({ 10,10 }, m_strMode, olc::WHITE);

		if (bIsDrawTargetRoute)
		{
			for (const auto& Point : m_setTargetPositions)
			{
				tv.DrawCircle(Point, 0.5f);
			}
		}

		for (const auto& object : m_mapObjects)
		{
			

			
			tv.DrawCircle(object.second.Position, object.second.fRadius, object.second.Color);
			
			if (object.second.Velocity.mag2() > 0)
			{
				tv.DrawLine(object.second.Position,
					object.second.Position + object.second.Velocity.norm() * object.second.fRadius,
					olc::MAGENTA);
			}
		
		}

		
	}


	void Follower::HandleUserEntry()
	{
		while (m_bWaitingForUserEntry)
		{

			olc::vf2d vf2dStartPosition;

			std::cout << "Enter starting x position of object:\n";

			std::cin.clear();
			if (std::cin >> vf2dStartPosition.x)
			{
				std::cout << "Enter starting y position of object:\n";
				std::cin.clear();
				if (std::cin >> vf2dStartPosition.y)
				{
					m_sObject2dDesc.Position = vf2dStartPosition;

					std::cout << "Enter max speed for your object: \n";
					float fMaxSpeed;

					std::cin.clear();
					if (std::cin >> fMaxSpeed)
					{
						m_ForceFollowMission.SetMaxSpeed(fMaxSpeed);

						std::cout << "Choice one of the following:\n";
						std::cout << "Test ForceFollowMission[t]\n";
						std::cout << "Normal Start[n]\n";
						char answer;
						std::cin.clear();
						if (std::cin >> answer)
						{
							switch (answer)
							{
							case 't':
								HandleTestForceFollowMission();
								break;
							case 'n':
								StartSimulation();
								break;
							default:
								std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
								break;
							}
						}
						else
						{
							std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
						}
					}
					else
					{
						std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
					}
				}
				else
				{
					std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
				}
			}
			else
			{
				std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
			}

		}
	}

	void Follower::HandleTestWithWayPointMission()
	{
		std::vector<olc::vf2d> WayPoints;

		std::cout << "Enter below limit of number of waypoints you want to produce :\n";
		int numberOfWayPointsBelowLimit = 0;
		if (std::cin >> numberOfWayPointsBelowLimit)
		{
			std::cout << "Enter upper limit of number of waypoints you want to produce :\n";
			int numberOfWayPointsUpperLimit = 0;

			if (std::cin >> numberOfWayPointsUpperLimit)
			{
				WayPoints = Utility::CreateWayPoints(numberOfWayPointsBelowLimit, numberOfWayPointsUpperLimit);
			}
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}

		//We should create fake escaper object
		Flight::Object2d sTarget;
		sTarget.Color = olc::RED;
		
		//.. and insert this into our object map with specific id
		m_nTargetObjectID = 0;
		m_mapObjects.insert_or_assign(m_nTargetObjectID, sTarget);

		// .. and we need to feed waypoint mission with that object2d
		m_WayPointMission.SetObject2d(&m_mapObjects.at(m_nTargetObjectID));
		m_WayPointMission.SetWayPoints(WayPoints);

		//We should add our follower object with specific id
		AssignObjectID(1);
		
		//We need to test in waypoint mode, this is boilerplot but eases the readability
		m_bIsTestForceFollow = true;

		StartSimulation();
	}

	// BUG: There is a bug with this test, don not draw follower object [Importance: Mid]
	void Follower::HandleTestWithEscapeMission()
	{

		//We should create fake escaper object
		Flight::Object2d sTarget;
		sTarget.Color = olc::RED;

		//.. and insert this into our object map with specific id
		m_nTargetObjectID = 0;
		m_mapObjects.insert_or_assign(m_nTargetObjectID, sTarget);

		// .. and we need to feed escaper mission with that object2d
		m_EscapeMission.SetObject2d(&m_mapObjects.at(m_nTargetObjectID));

		//We should add our follower object with specific id
		AssignObjectID(1);

		//We need to test in escape mode, so we need to set this bool true
		m_bIsTestForceFollow = false;

		StartSimulation();
	}


	void Follower::HandleTestForceFollowMission()
	{
		std::cout << "Which mission do you want the escaper to be in ?\n";
		std::cout << "WayPoint[w]\nEscape[e]\n";
		char answer;
		std::cin.clear();
		if (std::cin >> answer)
		{
			switch (answer)
			{
			case 'w':
				HandleTestWithWayPointMission();
				m_bIsTest = true;
				break;
			case 'e':
				HandleTestWithEscapeMission();
				m_bIsTest = true;
				break;
			default:
				std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
				break;
			}
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}

	}

	void Follower::StartSimulation()
	{
		std::cout << "The simulation will start. Do you agree?[y/n]\n";
		char answer;
		std::cin.clear();
		if (std::cin >> answer)
		{
			switch (answer)
			{
			case 'y':
				m_bWaitingForUserEntry = false;
				break;
			case 'n':
				m_bWaitingForUserEntry = true;
				break;
			default:
				std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
			}
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}

	}
}

