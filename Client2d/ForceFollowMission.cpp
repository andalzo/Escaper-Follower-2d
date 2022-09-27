#include "ForceFollowMission.h"

namespace Cevheri::Flight
{

	ForceFollowMission::ForceFollowMission(std::shared_ptr<Drone> d) : Mission(d)
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
		olc::vf2d PossibleVelocity = (PossiblePosition - m_drone->Position);
		//Normalize the our velocity to ensure we headed correct direction in this frame
		olc::vf2d PossibleDirection = PossibleVelocity.norm();
		//We need magnitude of our possible velocity
		float fDistanceToPossiblePosition = PossibleVelocity.mag();
		
		//Examine if we can execute that velocity vector in that frame:
		if (fDistanceToPossiblePosition <= m_drone->SpeedPerFrame)
		{
			//..if can, so our possible velocity is our new velocity
			m_drone->Velocity = PossibleVelocity;
			//and we are ready to pounce mission here, so we need to set our bool true
			m_bForceFollowSuccess = true;
		}
		else
		{
			//TODO: In realistic world, we need a speed limit here.
			if (m_drone->SpeedPerFrame <= fTargetSpeed)
			{
				m_drone->SpeedPerFrame = fTargetSpeed + 1.0f;
			}	
			//..if not, our new velocity should be headed to between our ex position and new
			m_drone->Velocity = m_drone->SpeedPerFrame * PossibleDirection;
		}
	}

	olc::vf2d ForceFollowMission::FindPossiblePosition(const olc::vf2d& TargetDirection)
	{
		// This is the vector to point our new possible position from our target's position
		olc::vf2d Delta = m_drone->fRadius*3*TargetDirection;
		olc::vf2d PossibleNewPosition = m_TargetNewPosition - Delta;
		if (PossibleNewPosition.x >= World_X_Limit)
		{
			PossibleNewPosition.x = World_X_Limit - m_drone->SpeedPerFrame;
		}
		if (PossibleNewPosition.x <= 0.0f)
		{
			PossibleNewPosition.x = m_drone->SpeedPerFrame;
		}

		if ((PossibleNewPosition.y >= World_Y_Limit))
		{
			PossibleNewPosition.y = World_Y_Limit - m_drone->SpeedPerFrame;
		}
		
		if ((PossibleNewPosition.y <= 0.0f))
		{
			PossibleNewPosition.y = m_drone->SpeedPerFrame;
		}

		return PossibleNewPosition;
	}
}

