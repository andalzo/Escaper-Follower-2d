#include "common2d.h"
#include <random>
#include <vector>

namespace Simulation2d::Utility
{
	int RandomNumber(const int& nUpperLim, const int& nLowerLim);
	std::vector<olc::vf2d> CreateWayPoints(const int& nUpperLim, const int& nLowerLim);
}