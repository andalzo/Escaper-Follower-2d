#include "Follower.h"

namespace Simulation2d::Net
{
	Follower::Follower()
	{
		sAppName = "Follower2d";
		sObject2dDesc.type = Flight::Object2d::Type::Follower;
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
}