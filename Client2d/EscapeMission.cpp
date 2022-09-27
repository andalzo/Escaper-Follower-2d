#include "EscapeMission.h"
#include "WaypointCreator.h"

namespace Simulation2d::Flight
{
	EscapeMission::EscapeMission(Object2d* d) : Mission(d)
	{
	}

	void EscapeMission::OnExecute()
	{
		if (m_ConstantDirectionFrameCounter == 50)
		{
			PossibleDirection.x = Utility::RandomNumber(-1, 1);
			PossibleDirection.y = Utility::RandomNumber(-1, 1);
			m_ptrObject2d->SpeedPerFrame = Utility::RandomNumber(1, 2);
			m_ConstantDirectionFrameCounter = 0;
			if (PossibleDirection == olc::vf2d{0, 0})
			{
				int chOne = Utility::RandomNumber(0, 1);
				if (chOne == 0)
				{
					PossibleDirection.x = 1.0f;
				}
				else
				{
					PossibleDirection.y = 1.0f;
				}
			}
		}

		olc::vf2d PossibleVelocity = m_ptrObject2d->SpeedPerFrame * PossibleDirection;
		olc::vf2d PossiblePosition = m_ptrObject2d->Position + PossibleVelocity;
		m_ConstantDirectionFrameCounter++;
		
		bool bIsCloseBorder = false;
		if (PossiblePosition.x >= World_X_Limit)
		{
			PossiblePosition.x = World_X_Limit - m_ptrObject2d->SpeedPerFrame;
			bIsCloseBorder = true;
		}
		if (PossiblePosition.x <= 0.0f)
		{
			PossiblePosition.x = m_ptrObject2d->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if ((PossiblePosition.y >= World_Y_Limit))
		{
			PossiblePosition.y = World_Y_Limit - m_ptrObject2d->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if ((PossiblePosition.y <= 0.0f))
		{
			PossiblePosition.y = m_ptrObject2d->SpeedPerFrame;
			bIsCloseBorder = true;
		}

		if (bIsCloseBorder)
		{
			m_ptrObject2d->Velocity = PossiblePosition - m_ptrObject2d->Position;
			bIsCloseBorder = true;
			return;
		}

		m_ptrObject2d->Velocity = PossibleVelocity;
	}


}



