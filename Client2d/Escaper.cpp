#include "Escaper.h"

namespace Simulation2d::Net
{
	Escaper::Escaper()
	{
		sAppName = "Escaper2d";
		m_sObject2dDesc.type = Flight::Object2d::Type::Escaper;
		m_WayPointMission.SetObject2d(&m_sObject2dDesc);
		m_EscapeMission.SetObject2d(&m_sObject2dDesc);

	}
	bool Escaper::OnUserCreate()
	{
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1, 1 });

		if (Connect("192.168.1.50", 60000))
		{
			return true;
		}
		return true; //TODO
	}
	
	bool Escaper::OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::BLACK);

		if (m_sObject2dDesc.bPounced)
		{
			// Draw WayPoints
			for (const auto& wayPoint : m_WayPointMission.GetWayPoints())
			{
				tv.DrawCircle(wayPoint, 8.0);
			}

			m_WayPointMission.Execute();
			//std::cout << "New Position" << m_sObject2dDesc.Position << "\n";
			tv.DrawCircle(m_sObject2dDesc.Position, m_sObject2dDesc.fRadius, olc::GREEN);
			
			if (m_sObject2dDesc.Velocity.mag2() > 0)
				tv.DrawLine(m_sObject2dDesc.Position, m_sObject2dDesc.Position + m_sObject2dDesc.Velocity.norm() * 
				m_sObject2dDesc.fRadius, olc::MAGENTA);
		}
		else
		{
			m_EscapeMission.Execute();
			//std::cout << "New Position" << m_sObject2dDesc.Position << "\n";
			tv.DrawCircle(m_sObject2dDesc.Position, m_sObject2dDesc.fRadius, olc::GREEN);

			if (m_sObject2dDesc.Velocity.mag2() > 0)
				tv.DrawLine(m_sObject2dDesc.Position, m_sObject2dDesc.Position + m_sObject2dDesc.Velocity.norm() *
					m_sObject2dDesc.fRadius, olc::MAGENTA);
		}
		
		return true;
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
				std::cout << "\n***WAYPOINT ENTRY MENU***\n\n";
				std::cout << "Please select number to continue:\n";
				std::cout << "1. Manual Entry (Give waypoints manually)\n";
				std::cout << "2. Random Entry (Choices waypoints randomly)\n";
				std::cout << "3. Test Simulation (Choices waypoints predefined)\n";
				char cEntry;
				std::cin.clear();
				if (std::cin >> cEntry)
				{
					switch (static_cast<EscaperUserEntry>(static_cast<uint8_t>(cEntry) - 48))
					{
					case ManualEntry:
					{
						HandleManualUserEntry();
						break;
					}
					case RandomEntry:
					{
						HandleRandomUserEntry();
						break;
					}
					case TestSimulation:
					{
						HandleSimulationEntry();
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
			else
			{
				std::cout << "[WARNING]: You entered inappropriate value\n";
			}	
		}

	}
	void Escaper::HandleManualUserEntry()
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
			HandleUserEntryStartSimulation();
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}
	}
	void Escaper::HandleRandomUserEntry()
	{
		std::cout << "Enter below limit of number of waypoints you want to produce :\n";
		int numberOfWayPointsBelowLimit = 0;
		if (std::cin >> numberOfWayPointsBelowLimit)
		{
			std::cout << "Enter upper limit of number of waypoints you want to produce :\n";
			int numberOfWayPointsUpperLimit = 0;

			if (std::cin >> numberOfWayPointsUpperLimit)
			{
				std::vector<olc::vf2d> WayPoints;
				WayPoints = Utility::CreateWayPoints(numberOfWayPointsBelowLimit, numberOfWayPointsUpperLimit);
				m_WayPointMission.SetWayPoints(WayPoints);
				HandleUserEntryStartSimulation();
			}	
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}
	}
	
	void Escaper::HandleSimulationEntry()
	{
		//WPM Simulation Ýnitializition Here
		m_WayPointMission.AddWayPoint({ 30.0f,40.0f });
		m_WayPointMission.AddWayPoint({ 170.0f,40.0f });
		m_WayPointMission.AddWayPoint({ 170.0f,120.0f });
		HandleUserEntryStartSimulation();
	}
	
	void Escaper::HandleUserEntryStartSimulation()
	{
		std::cout << "The way point mission created and assigned. The simulation will start.\n";
		std::cout << "Do you agree?[y/n]\n";
		char input;
		if (std::cin >> input)
		{
			if (input == 'y')
			{
				m_bWaitingForUserEntry = false;
				//TODO: This is ugly to handle missions inside Escaper, maybe better organization of mission classes 
				//with singletons and enum approach work better

			}
			else if (input == 'n')
			{
				m_bWaitingForUserEntry = true;
			}
			else
			{
				std::cout << "[WARNING]:You entered inappropriate value. Try again from start.\n";
			}
		}
		else
		{
			std::cout << "[WARNING]:You entered inappropriate value.Try again from start.\n";
		}
	}
}

