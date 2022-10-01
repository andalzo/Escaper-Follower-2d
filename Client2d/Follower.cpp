#include "Follower.h"

namespace Simulation2d::Net
{
	Follower::Follower()
	{
		sAppName = "Follower2d";
		m_sObject2dDesc.type = Flight::Object2d::Type::Follower;
	}
	bool Follower::OnUserCreate()
	{
		if (Connect("192.168.1.50", 60000))
		{
			return true;
		}
		return false;
	}

	bool Follower::OnUserUpdate(float fElapsedTime)
	{
		//Main simulation logic here
		return true;
	}

	void Follower::HandleUserEntry()
	{
		while(m_bWaitingForUserInput)
		{
			std::cout << "Enter max speed for your object: \n";
			float nMaxSpeed;
			std::cin.clear();
			
			if (std::cin >> nMaxSpeed)
			{

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
						m_bWaitingForUserInput = false;
					case FollowerUserEntry::TestPounceMission:
						HandleTestPounceMission();
						m_bWaitingForUserInput = false;
					case FollowerUserEntry::NormalStart:
						HandleNormalStart();
						m_bWaitingForUserInput = false;
					default:
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
	}
	void Follower::HandleTestPounceMission()
	{
	}
	void Follower::HandleNormalStart()
	{
	}
}
