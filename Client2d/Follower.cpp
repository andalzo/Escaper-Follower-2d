#include "Follower.h"

namespace Simulation2d::Net
{
	Follower::Follower()
	{
		sAppName = "Follower2d";
		m_sObject2dDesc.type = Flight::Object2d::Type::Follower;
		m_sObject2dDesc.Color = olc::GREEN;
		m_sObject2dDesc.Position = { 250.0f, 250.0f };
		m_ForceFollowMission.SetObject2d(&m_sObject2dDesc);
		m_PounceMission.SetObject2d(&m_sObject2dDesc);
	}
	bool Follower::OnUserCreate()
	{
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1, 1 });

		if (Connect("192.168.1.50", 60000))
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
		
		if (!m_bIsTest)
		{
			//Handle Incoming Messages
			HandleIncomingMessages();
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
			}

			//Drawing the objects
			DrawWorldObjects(bIsDrawTargetRoute);
			
			//Final update message to the server
		}
		else
		{
			if (!m_sObject2dDesc.bPounced)
			{
			
				m_WayPointMission.Execute();
				m_setTargetPositions.insert(m_sTargetObject2dDesc.Position);
				m_ForceFollowMission.SetTargetPosition(m_sTargetObject2dDesc.Position);
				m_ForceFollowMission.Execute();
				
				DrawWorldObjects(bIsDrawTargetRoute);
			}
			else
			{

			}
		}
		

		return true;
	}

	void Follower::HandleUserEntry()
	{
		while(m_bWaitingForUserEntry)
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
						
						std::cout << "\n***FOLLOWER USER INPUT MENU***\n";
						std::cout << "Choice one of the following\n";
						std::cout << "1. Test ForceFollowMission\n";
						std::cout << "2. Test Pounce Mission\n";
						std::cout << "3. Normal Start\n";
						int input;
						std::cin.clear();
						if (std::cin >> input)
						{
							switch (static_cast<FollowerUserEntry>(input))
							{
							case FollowerUserEntry::TestForceFollowMission:
								HandleTestForceFollowMission();
								break;
							case FollowerUserEntry::TestPounceMission:
								HandleTestPounceMission();
								break;
							case FollowerUserEntry::NormalStart:
								HandleNormalStart();
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
	void Follower::HandleTestForceFollowMission()
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
		

		m_sTargetObject2dDesc.Color = olc::RED;
		m_WayPointMission.SetObject2d(&m_sTargetObject2dDesc);
		m_WayPointMission.SetWayPoints(WayPoints);

		m_bIsTest = true;

		HandleNormalStart();

	}
	void Follower::HandleTestPounceMission()
	{
		m_bIsTest = true;
		HandleNormalStart();
	}
	void Follower::HandleNormalStart()
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

	void Follower::HandleIncomingMessages()
	{
	}

	void Follower::OnObserveMission()
	{
	}

	void Follower::OnForceFollowMission()
	{
	}

	void Follower::OnPounceMission()
	{
	}

	// TODO: Need to draw every object,the code needs a touch
	void Follower::DrawWorldObjects(const bool& bIsDrawTargetRoute)
	{

		Clear(olc::DARK_BLUE);

		if (bIsDrawTargetRoute)
		{
			for (const auto& Point : m_setTargetPositions)
			{
				tv.DrawCircle(Point, 0.5f);
			}
		}

		if (!m_bIsTest)
		{
			for (const auto& object : m_mapObjects)
			{
				tv.DrawCircle(object.second.Position, object.second.fRadius, object.second.Color);

				if (object.second.Velocity.mag2() > 0)
					tv.DrawLine(object.second.Position,
						object.second.Position + object.second.Velocity.norm() * object.second.fRadius,
						olc::MAGENTA);
			}

		}
		else
		{
			tv.DrawCircle(m_sObject2dDesc.Position, m_sObject2dDesc.fRadius, m_sObject2dDesc.Color);
			tv.DrawCircle(m_sTargetObject2dDesc.Position, m_sTargetObject2dDesc.fRadius, m_sTargetObject2dDesc.Color);

			if (m_sObject2dDesc.Velocity.mag2() > 0)
				tv.DrawLine(m_sObject2dDesc.Position,
					m_sObject2dDesc.Position + m_sObject2dDesc.Velocity.norm() * m_sObject2dDesc.fRadius,
					olc::MAGENTA);

			if (m_sTargetObject2dDesc.Velocity.mag2() > 0)
				tv.DrawLine(m_sTargetObject2dDesc.Position,
					m_sTargetObject2dDesc.Position + m_sTargetObject2dDesc.Velocity.norm() * m_sTargetObject2dDesc.fRadius,
					olc::MAGENTA);
		}
	}
}
