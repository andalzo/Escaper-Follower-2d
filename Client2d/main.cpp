#include "Escaper.h"
#include "Follower.h"

int main()
{
	bool flagOut = false;
	while (!flagOut)
	{
		std::cout << "Which one will be your role ?\nEscaper[e]\nFollower[f]\n";
		std::cin.clear();
		char choice;
		std::cin >> choice;
		if (choice == 'e')
		{
			Simulation2d::Net::Escaper escaper;
			escaper.HandleUserEntry();
			bool isConstructSuccesful = escaper.Construct(Simulation2d::Flight::World_X_Limit, Simulation2d::Flight::World_Y_Limit, 1, 1);
			if (isConstructSuccesful)
			{
				escaper.Start();
			}
			if (!escaper.olc_IsRunning())
			{
				flagOut = true;
			}
		}
		if (choice == 'f')
		{
			Simulation2d::Net::Follower follower;
			follower.HandleUserEntry();
			bool isConstructSuccesful = follower.Construct(Simulation2d::Flight::World_X_Limit, Simulation2d::Flight::World_Y_Limit, 1, 1);
			if (isConstructSuccesful)
			{
				follower.Start();
			}
			if (!follower.olc_IsRunning())
			{
				flagOut = true;
			}
		}
		else
		{
			std::cout << "[WARNING]: Inappropriate Input. Try again.\n";
		}
	}

	return EXIT_SUCCESS;
}