#include "Escaper.h"

namespace Cevheri::Net
{
	Escaper::Escaper()
	{
		sAppName = "Escaper2d";
		m_sDroneDesc.type = Flight::Drone::Type::Escaper;
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

		if (!m_sDroneDesc.bPounced)
		{
			// Draw WayPoints
			for (const auto& wayPoint : m_WayPoints)
			{
				tv.DrawCircle(wayPoint, 8.0);
			}

			WPM.ExecutePerFrame();
			m_sDroneDesc = WPM.GetDrone();
			std::cout << "New Position" << m_sDroneDesc.Position << "\n";
			tv.DrawCircle(m_sDroneDesc.Position, m_sDroneDesc.fRadius, olc::GREEN);
			
			if (m_sDroneDesc.Velocity.mag2() > 0)
				tv.DrawLine(m_sDroneDesc.Position, m_sDroneDesc.Position + m_sDroneDesc.Velocity.norm() * m_sDroneDesc.fRadius, olc::MAGENTA);
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
				m_sDroneDesc.SpeedPerFrame = fSpeed;
				std::cout << "\n***WAYPOINT ENTRY MENU***\n\n";
				std::cout << "Please select number to continue:\n";
				std::cout << "1. Manual Entry (Give waypoints manually)\n";
				std::cout << "2. Random Entry (Choices waypoints randomly)\n";
				std::cout << "3. Test Simulation (Choices waypoints predefined)\n";
				char cEntry;
				std::cin.clear();
				if (std::cin >> cEntry)
				{
					switch (static_cast<UserEntry>(static_cast<uint8_t>(cEntry) - 48))
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
			Flight::WayPointMission WPM;
			WPM.SetDrone(std::make_shared<Flight::Drone>(m_sDroneDesc));
			for (int i = 0; i < numberOfWayPoints; i++)
			{
				olc::vf2d point;
				std::cout << "\nEnter x" << i + 1 << ":";
				std::cin >> point.x;
				std::cout << "\nEnter y" << i + 1 << ":";
				std::cin >> point.y;
				WPM.AddWayPoint(point);
			}
			WPM.SetIfAchievable(true);
			HandleUserEntryStartSimulation(WPM);
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
				Flight::WayPointMission WPM;
				WPM.SetDrone(std::make_shared<Flight::Drone>(m_sDroneDesc));
				WPM.SetWayPoints(WayPoints);
				WPM.SetIfAchievable(true);
				HandleUserEntryStartSimulation(WPM);
			}	
		}
		else
		{
			std::cout << "[WARNING]: You entered inappropriate value. Try again from start.\n";
		}
	}
	
	void Escaper::HandleSimulationEntry()
	{
		Flight::WayPointMission WPM;
		WPM.SetDrone(std::make_shared<Flight::Drone>(m_sDroneDesc));
		//WPM Simulation Ýnitializition Here
		WPM.AddWayPoint({ 30.0f,40.0f });
		WPM.AddWayPoint({ 170.0f,40.0f });
		WPM.AddWayPoint({ 170.0f,120.0f });
		WPM.SetIfAchievable(true);
		HandleUserEntryStartSimulation(WPM);
	}
	
	void Escaper::HandleUserEntryStartSimulation(const Flight::WayPointMission& WPM)
	{
		std::cout << "The way point mission created and assigned. The simulation will start.\n";
		std::cout << "Do you agree?[y/n]\n";
		char input;
		if (std::cin >> input)
		{
			if (input == 'y')
			{
				m_bWaitingForUserEntry = false;
				m_WayPoints = WPM.GetWayPoints();
				this->WPM = WPM; 
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

