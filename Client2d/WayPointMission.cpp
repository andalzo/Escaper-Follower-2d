#include "WayPointMission.h"

namespace Simulation2d::Flight
{
	WayPointMission::WayPointMission(Object2d* d, const std::vector<olc::vf2d> w): Mission(d)
	{
		m_WayPoints = w;
		m_CurrentTargetPoint = m_WayPoints.at(0);
	}

	void WayPointMission::SetWayPoints(std::vector<olc::vf2d>& w)
	{
		m_WayPoints = w;
		m_CurrentTargetPoint = m_WayPoints.at(0);
	}

	void WayPointMission::AddWayPoint(const olc::vf2d& p)
	{
		if (m_WayPoints.empty())
		{
			m_CurrentTargetPoint = p;
		}
		
		m_WayPoints.push_back(p);
	}

	std::vector<olc::vf2d>WayPointMission::GetWayPoints() const
	{
		return m_WayPoints;
	}

	void WayPointMission::OnExecute()
	{
		//Create the vector from our position to target position to obtain our possible velocity
		olc::vf2d PossibleVelocity = (m_CurrentTargetPoint - m_ptrObject2d->Position);
		//Normalize the our velocity to ensure we headed correct direction in this frame
		olc::vf2d PossibleDirection = PossibleVelocity.norm();
		//We need magnitude of our possible velocity
		float fDistanceToWayPoint = PossibleVelocity.mag();
		//Examine if we can execute that velocity vector in that frame:
		if (fDistanceToWayPoint <= m_ptrObject2d->SpeedPerFrame)
		{
			//..if can, so our possible velocity is our new velocity
			m_ptrObject2d->Velocity = PossibleVelocity;
			UpdateTargetWayPoint();
		}
		else
		{
			//..if not, our new velocity should be headed to between our previous waypoint and 
			//next waypoint
			m_ptrObject2d->Velocity = m_ptrObject2d->SpeedPerFrame * PossibleDirection;
		}
	}

	void WayPointMission::UpdateTargetWayPoint()
	{
		UpdateTargetWayPointIndex();
		m_CurrentTargetPoint = m_WayPoints.at(m_CurrentTargetPointIndex);
	}
	
	void WayPointMission::UpdateTargetWayPointIndex()
	{
		m_CurrentTargetPointIndex++;
		//If we hit the last waypoint, we should get back to first one
		if (m_CurrentTargetPointIndex == m_WayPoints.size())
		{
			m_CurrentTargetPointIndex = 0;
		}
	}
}


