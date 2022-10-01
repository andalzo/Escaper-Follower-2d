#include "Follower.h"

namespace Simulation2d::Net
{
	Follower::Follower()
	{
		sAppName = "Follower2d";
		m_sObject2dDesc.type = Flight::Object2d::Type::Follower;
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
		
		if (!m_bIsTest)
		{

		}
		else
		{
			if (!m_sObject2dDesc.bPounced)
			{
				bool isEscaperClicked = false;
				if (GetMouse(0).bHeld)
				{
					isEscaperClicked = !isEscaperClicked;
				}

				m_WayPointMission.Execute();
				m_setTargetPositions.insert(m_sObject2dEscaperDesc.Position);
				m_ForceFollowMission.SetTargetPosition(m_sObject2dEscaperDesc.Position);
				m_ForceFollowMission.Execute();
				
				DrawWorldObjects(isEscaperClicked);
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
	}
	void Follower::HandleTestForceFollowMission()
	{	

		std::vector<olc::vf2d> vectTargetPositions;
		
		vectTargetPositions.push_back({ 30.0f, 40.0f });
		vectTargetPositions.push_back({ 170.0f, 40.0f });
		vectTargetPositions.push_back({ 170.0f, 120.0f });
		
		m_WayPointMission.SetObject2d(&m_sObject2dEscaperDesc);
		m_WayPointMission.SetWayPoints(vectTargetPositions);

		m_bIsTest = true;

		HandleNormalStart();

	}
	void Follower::HandleTestPounceMission()
	{
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
	void Follower::DrawWorldObjects(const bool& isEscaperClicked)
	{
		Clear(olc::DARK_BLUE);

		if (isEscaperClicked)
		{
			for (const auto& Point : m_setTargetPositions)
			{
				tv.DrawCircle(Point, 1.0f);
			}
		}
		
		tv.DrawCircle(m_sObject2dDesc.Position, m_sObject2dDesc.fRadius, olc::GREEN);
		tv.DrawCircle(m_sObject2dEscaperDesc.Position, m_sObject2dDesc.fRadius, olc::RED);

		if (m_sObject2dDesc.Velocity.mag2() > 0)
			tv.DrawLine(m_sObject2dDesc.Position,
				m_sObject2dDesc.Position + m_sObject2dDesc.Velocity.norm() * m_sObject2dDesc.fRadius, 
				olc::MAGENTA);

		if (m_sObject2dEscaperDesc.Velocity.mag2() > 0)
			tv.DrawLine(m_sObject2dEscaperDesc.Position, 
				m_sObject2dEscaperDesc.Position + m_sObject2dEscaperDesc.Velocity.norm() * m_sObject2dEscaperDesc.fRadius, 
				olc::MAGENTA);
	}
}
