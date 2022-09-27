#pragma once
#include "Common/common2d.h"

namespace Cevheri::Flight
{

	class WayPointMission : public Mission
	{
	public:
		WayPointMission() = default;
		WayPointMission(std::shared_ptr<Drone> d, const std::vector<olc::vf2d> w);
		void SetWayPoints(std::vector<olc::vf2d>& m_WayPoints);
		void AddWayPoint(const olc::vf2d& p);
		std::vector<olc::vf2d> GetWayPoints() const;

	protected:
		void OnExecute() override;
	private:
		void UpdateTargetWayPoint();
		void UpdateTargetWayPointIndex();
		
		std::vector<olc::vf2d> m_WayPoints;
		olc::vf2d m_CurrentTargetPoint;
		uint32_t m_CurrentTargetPointIndex = 0;
	};
}



