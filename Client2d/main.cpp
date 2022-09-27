#include "Escaper.h"




int main()
{
	bool flagOut = false;
	while (!flagOut)
	{
		std::cout << "Which one will be your role ? [e for Escaper|f for Follower\n";
		std::cin.clear();
		char choice;
		std::cin >> choice;
		if (choice == 'e')
		{
			Cevheri::Net::Escaper escaper;
			escaper.HandleUserEntry();
			bool isConstructionSuccesful = escaper.Construct(Cevheri::Flight::World_X_Limit, Cevheri::Flight::World_Y_Limit, 1, 1);
			if (isConstructionSuccesful)
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

		}
		else
		{
			std::cout << "[WARNÝNG]: Inappropriate Input. Try again.\n";
		}
	}

	return EXIT_SUCCESS;
}