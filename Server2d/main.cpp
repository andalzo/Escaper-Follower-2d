#include <iostream>
#include "GroundStationServer.h"


int main()
{
	Cevheri::Net::GroundStationServer GroundStation(60000);
	GroundStation.Start();
	while (1)
	{
		GroundStation.Update(-1,true);
	}

}


