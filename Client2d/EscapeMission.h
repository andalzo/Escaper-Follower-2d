#pragma once
#include "common2d.h"

namespace Simulation2d::Flight
{

	class EscapeMission : public Mission
	{
	public:
		EscapeMission() = default;
		EscapeMission(Object2d* d);
		bool IsConstantDirection() const;
	protected:
		void OnExecute() override;	
	private:
		int m_ConstantDirectionFrameCounter = 0;
		bool m_bIsConstantDirection = false; 
		//This bool is determined if it is in constant direction for testing purposes
		olc::vf2d PossibleDirection;
	};
}


