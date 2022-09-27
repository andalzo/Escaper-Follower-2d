#pragma once
#include "common2d.h"

namespace Simulation2d::Flight
{

	class EscapeMission : public Mission
	{
	public:
		EscapeMission() = default;
		EscapeMission(Object2d* d);
	protected:
		virtual void OnExecute();
	private:
		int m_ConstantDirectionFrameCounter = 0;
		olc::vf2d PossibleDirection;
	};
}


