#include "EscapeMission.h"
#include "WaypointCreator.h"

namespace Cevheri::Flight
{
	EscapeMission::EscapeMission(std::shared_ptr<Drone> d) : Mission(d)
	{
	}

	void EscapeMission::OnExecute()
	{
		olc::vf2d PossibleDirection;
		PossibleDirection.x = Utility::RandomNumber(-1, 1);
		PossibleDirection.y = Utility::RandomNumber(-1, 1);

		olc::vf2d PossibleVelocity = m_drone->SpeedPerFrame * PossibleDirection;
		olc::vf2d PossiblePosition = m_drone->Position + PossibleVelocity;
		
		bool bIsCloseBorder = false;
		if (PossiblePosition.x >= World_X_Limit)
		{
			PossiblePosition.x = World_X_Limit - m_drone->SpeedPerFrame;
			bIsCloseBorder = true;
		}
		if (PossiblePosition.x <= 0.0f)
		{
			PossiblePosition.x = m_drone->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if ((PossiblePosition.y >= World_Y_Limit))
		{
			PossiblePosition.y = World_Y_Limit - m_drone->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if ((PossiblePosition.y <= 0.0f))
		{
			PossiblePosition.y = m_drone->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if (bIsCloseBorder)
		{
			m_drone->Velocity = PossiblePosition - m_drone->Position;
			bIsCloseBorder = true;
		}

		m_drone->Velocity = PossibleVelocity;
	}


}



