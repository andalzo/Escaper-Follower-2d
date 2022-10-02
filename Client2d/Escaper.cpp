#include "Escaper.h"

namespace Simulation2d::Net
{
	Escaper::Escaper()
	{
		sAppName = "Escaper2d";
		m_sObject2dDesc.type = Flight::Object2d::Type::Escaper;
		m_sObject2dDesc.Color = olc::RED;
	}
	bool Escaper::OnUserCreate()
	{
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1, 1 });

		if (Connect("192.168.1.51", 60000))
		{
			return true;
		}
		return false; //TODO
	}
	
	bool Escaper::OnUserUpdate(float fElapsedTime)
	{
		if (m_bIsTest)
		{
			if (m_bIsTestWayPoint)
			{
				m_WayPointMission.Execute();

			}
			else
			{
				m_EscapeMission.Execute();
			}
			DrawWorldObjects();

			return true;
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

			switch (m_ActiveMission)
			{
			case WayPointMission:
				OnWayPointMission();
				break;
			case EscapeMission:
				OnEscapeMission();
				break;
			case DestroyMission:
				OnDestroyMission();
				break;
			default:
				std::cerr << "\n[ERROR]:Impossible active mission state for escaper.\n";
				std::exit(-1);
				break;
			}

			DrawWorldObjects();

			//Final update message to the server
			message<SimMsg> msg;
			msg.header.id = SimMsg::Simulation_UpdateObject;
			msg << m_mapObjects.at(m_nObjectId);
			Send(msg);

			return true;
		}
		
	}
	
	void Escaper::HandleUserEntry()
	{
		//TODO: Make simple gui for this entry using lightweight gui library
		while (m_bWaitingForUserEntry)
		{
			std::cout << "Enter the default speed of vehicle: \n";
			float fSpeed = 4.0f;
			std::cin.clear();
			if (std::cin >> fSpeed)
			{
				m_sObject2dDesc.SpeedPerFrame = fSpeed;
				std::cout << "What do you want to do?\n";
				std::cout << "Test WayPointMission[w]\n";
				std::cout << "Test EscapeMission[e]\n";
				std::cout << "Normal Start[n](need network)\n";
				char answer;
				std::cin.clear();
				int limBelow, limUpper;
				if (std::cin >> answer)
				{
					switch (answer)
					{
					case 'w':
						HandleUserEntryWayPoint();
						break;
					case 'e':
						HandleUserEntryEscape();
						break;
					case 'n':
						limBelow = Utility::RandomNumber(5, 12);
						limUpper = Utility::RandomNumber(12, 30);
						HandleRandomWayPoint(limBelow, limUpper);
						StartSimulation();
						break;
					default:
						std::cout << "[WARNING]: You entered inappropriate value\n";
						break;
					}
				}
				else
				{
					std::cout << "[WARNING]: You entered inappropriate value\n";
				}
			}
			else
			{
				std::cout << "[WARNING]: You entered inappropriate value\n";
			}
		}

	}

	void Escaper::HandleIncomingMessages()
	{

	}
	void Escaper::AssignObjectID(const uint32_t& id)
	{
		m_nObjectId = id;
		m_mapObjects.insert_or_assign(m_nObjectId, m_sObject2dDesc);
		m_WayPointMission.SetObject2d(&m_mapObjects.at(m_nObjectId));
		m_EscapeMission.SetObject2d(&m_mapObjects.at(m_nObjectId));
	}
	void Escaper::OnWayPointMission()
	{
		if (m_mapObjects.at(m_nObjectId).bPounced)
		{
			m_ActiveMission = EscaperMissions::EscapeMission;
			return;
		}

		m_WayPointMission.Execute();
	}
	void Escaper::OnEscapeMission()
	{
		if (m_mapObjects.at(m_nObjectId).bPounceSucces)
		{
			m_ActiveMission = EscaperMissions::DestroyMission;
			return;
		}

		m_WayPointMission.Execute();
	}
	void Escaper::OnDestroyMission()
	{
		Disconnect(); //Disconnect from server
		olc_Terminate(); // Close the application
	}
	void Escaper::DrawWorldObjects()
	{
		Clear(olc::DARK_BLUE);
		
		if (!m_mapObjects.at(m_nObjectId).bPounced)
		{
			// Draw WayPoints
			for (const auto& wayPoint : m_WayPointMission.GetWayPoints())
			{
				tv.DrawCircle(wayPoint, 8.0);
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
	
	void Escaper::HandleUserEntryWayPoint()
	{
		m_bIsTestWayPoint = true;
		std::cout << "Please select entry mode:\n";
		std::cout << "Manual Entry (Give waypoints manually)[m]\n";
		std::cout << "Random Entry (Choices waypoints randomly)[r]\n";
		std::cout << "Test Simulation (Choices waypoints predefined)[t]\n";
		char cEntry;
		std::cin.clear();
		if (std::cin >> cEntry)
		{
			switch (cEntry)
			{
			case 'm':
			{
				HandleManualUserEntryWayPoint();
				break;
			}
			case 'r':
			{
				HandleRandomUserEntryWayPoint();
				break;
			}
			case 't':
			{
				HandleSimulationEntryWayPoint();
				break;
			}
			default:
			{
				std::cout << "[WARNING]: You entered inappropriate value\n";
				break;
			}
			}
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value\n";
		}
	}
	void Escaper::HandleManualUserEntryWayPoint()
	{
		std::cout << "Enter the number of waypoints you want:\n";
		int numberOfWayPoints = 0;
		std::cin.clear();
		if (std::cin >> numberOfWayPoints)
		{
			for (int i = 0; i < numberOfWayPoints; i++)
			{
				olc::vf2d point;
				std::cout << "\nEnter x" << i + 1 << ":";
				std::cin >> point.x;
				std::cout << "\nEnter y" << i + 1 << ":";
				std::cin >> point.y;
				m_WayPointMission.AddWayPoint(point);
			}
			AssignObjectID(0);
			StartSimulation();
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}
	}
	void Escaper::HandleRandomUserEntryWayPoint()
	{
		std::cout << "Enter below limit of number of waypoints you want to produce :\n";
		int limBelow = 0;
		std::cin.clear();
		if (std::cin >> limBelow)
		{
			std::cout << "Enter upper limit of number of waypoints you want to produce :\n";
			int limUpper = 0;
			std::cin.clear();
			if (std::cin >> limUpper)
			{
				HandleRandomWayPoint(limBelow, limUpper);
				AssignObjectID(0);
				StartSimulation();
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
	
	void Escaper::HandleSimulationEntryWayPoint()
	{
		//WPM Simulation Ýnitializition Here
		m_WayPointMission.AddWayPoint({ 30.0f,40.0f });
		m_WayPointMission.AddWayPoint({ 170.0f,40.0f });
		m_WayPointMission.AddWayPoint({ 170.0f,120.0f });
		AssignObjectID(0);
		StartSimulation();
	}
	
	void Escaper::HandleUserEntryEscape()
	{
		m_bIsTestWayPoint = false;
		AssignObjectID(0);
		StartSimulation();
	}

	void Escaper::HandleRandomWayPoint(const int& limBelow, const int& limUpper)
	{
		std::vector<olc::vf2d> WayPoints;
		WayPoints = Utility::CreateWayPoints(limBelow, limUpper);
		m_WayPointMission.SetWayPoints(WayPoints);
	}

	void Escaper::StartSimulation()
	{
		std::cout << "The simulation for escaper will start.Do you agree?[y/n]\n";
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
				std::cout << "[WARNING]:You entered inappropriate value.Try again from start.\n";
				break;
			}
		}
		else
		{
			std::cout << "[WARNING]:You entered inappropriate value.Try again from start.\n";
		}
	}
}

