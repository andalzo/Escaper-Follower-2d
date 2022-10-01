#include "ForceFollowMission.h"

namespace Simulation2d::Flight
{

	ForceFollowMission::ForceFollowMission(Object2d* d) : Mission(d)
	{
	}
	void ForceFollowMission::SetTargetPosition(const olc::vf2d& pos)
	{
		if (!m_TargetOldPosition.has_value())
		{
			m_TargetOldPosition = pos;
		}
		else
		{
			olc::vf2d temp = m_TargetNewPosition;
			m_TargetNewPosition = pos;
			m_TargetOldPosition = temp;
		}
	}
	void ForceFollowMission::SetMaxSpeed(const float& spd)
	{
		m_fMaxSpeedPerFrame = spd;
	}
	bool ForceFollowMission::IsForceFollowSucces() const
	{
		return m_bForceFollowSuccess;
	}
	std::optional<olc::vf2d> ForceFollowMission::GetTargetOldPosition() const
	{
		return m_TargetOldPosition;
	}
	void ForceFollowMission::OnExecute()
	{
		if (!m_TargetOldPosition.has_value())
		{
			return;
		}
		//Extracting information of target drone
		olc::vf2d TargetVelocity = m_TargetNewPosition - m_TargetOldPosition.value();
		olc::vf2d TargetDirection = TargetVelocity.norm();
		float fTargetSpeed = TargetVelocity.mag2();
		
		olc::vf2d PossiblePosition = FindPossiblePosition(TargetDirection);
		//Create the vector from our position to target position to obtain our possible velocity
		olc::vf2d PossibleVelocity = (PossiblePosition - m_ptrObject2d->Position);
		//Normalize the our velocity to ensure we headed correct direction in this frame
		olc::vf2d PossibleDirection = PossibleVelocity.norm();
		//We need magnitude of our possible velocity
		float fDistanceToPossiblePosition = PossibleVelocity.mag();
		
		//Examine if we can execute that velocity vector in that frame:
		if (fDistanceToPossiblePosition <= m_ptrObject2d->SpeedPerFrame)
		{
			//..if can, so our possible velocity is our new velocity
			m_ptrObject2d->Velocity = PossibleVelocity;
			//and we are ready to pounce mission here, so we need to set our bool true
			m_bForceFollowSuccess = true;
		}
		else
		{
			//TODO: In realistic world, we need a speed limit here.
			if (m_ptrObject2d->SpeedPerFrame <= fTargetSpeed)
			{
				m_ptrObject2d->SpeedPerFrame = fTargetSpeed + 1.0f;
				if (m_ptrObject2d->SpeedPerFrame >= m_fMaxSpeedPerFrame)
				{
					m_ptrObject2d->SpeedPerFrame = m_fMaxSpeedPerFrame;
				}
			}	
			//..if not, our new velocity should be headed to between our ex position and new
			m_ptrObject2d->Velocity = m_ptrObject2d->SpeedPerFrame * PossibleDirection;
		}
	}

	olc::vf2d ForceFollowMission::FindPossiblePosition(const olc::vf2d& TargetDirection)
	{
		// This is the vector to point our new possible position from our target's position
		olc::vf2d Delta = m_ptrObject2d->fRadius*3*TargetDirection;
		olc::vf2d PossibleNewPosition = m_TargetNewPosition - Delta;
		if (PossibleNewPosition.x >= World_X_Limit)
		{
			PossibleNewPosition.x = World_X_Limit - m_ptrObject2d->SpeedPerFrame;
		}
		if (PossibleNewPosition.x <= 0.0f)
		{
			PossibleNewPosition.x = m_ptrObject2d->SpeedPerFrame;
		}

		if ((PossibleNewPosition.y >= World_Y_Limit))
		{
			PossibleNewPosition.y = World_Y_Limit - m_ptrObject2d->SpeedPerFrame;
		}
		
		if ((PossibleNewPosition.y <= 0.0f))
		{
			PossibleNewPosition.y = m_ptrObject2d->SpeedPerFrame;
		}

		return PossibleNewPosition;
	}
}

