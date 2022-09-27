#include "WaypointCreator.h"


namespace Simulation2d::Utility
{
	int RandomNumber(const int& nLowerLim,const int& nUpperLim)
	{
		std::random_device randomness;
		std::uniform_int_distribution<int> distribution(nLowerLim, nUpperLim);
		return distribution(randomness);
	}

	std::vector<olc::vf2d> CreateWayPoints(const int& nLowerLim, const int& nUpperLim)
	{
		int nOfWayPoints = RandomNumber(nLowerLim, nUpperLim);
		std::vector<olc::vf2d> result;
		for (int i = 0; i < nOfWayPoints; i++)
		{
			int x = RandomNumber(0, Flight::World_X_Limit);
			int y = RandomNumber(0, Flight::World_Y_Limit);
			result.emplace_back(x, y);
		}
		return result;
	}

}
