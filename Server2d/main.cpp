#include <iostream>
#include "Server2d.h"


int main()
{
	Simulation2d::Net::Server2d server(60000);
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	return 0;
}


