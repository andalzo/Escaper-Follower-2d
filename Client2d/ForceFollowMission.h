#pragma once
#include "common2d.h"
#include <optional>

namespace Simulation2d::Flight
{
	class ForceFollowMission : public Mission
	{
	public:
		ForceFollowMission() = default;
		explicit ForceFollowMission(Object2d* d);

		void SetTargetPosition(const olc::vf2d& pos);
		void SetMaxSpeed(const float& spd);

		bool IsForceFollowSucces() const;
		std::optional<olc::vf2d> GetTargetOldPosition() const;

	protected:
		void OnExecute() override;

	private:
		olc::vf2d FindPossiblePosition(const olc::vf2d& TargetDirection);

		olc::vf2d m_TargetNewPosition;
		std::optional<olc::vf2d> m_TargetOldPosition;
		bool m_bForceFollowSuccess = false;
		float m_fMaxSpeedPerFrame;
	};

	

}

