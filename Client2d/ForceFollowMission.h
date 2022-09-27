#pragma once
#include "Common/common2d.h"
#include <optional>

namespace Cevheri::Flight
{
	// TODO: Need to test this mission
	class ForceFollowMission : public Mission
	{
	public:
		ForceFollowMission() = default;
		explicit ForceFollowMission(std::shared_ptr<Drone> d);

		void SetTargetPosition(const olc::vf2d& pos);

		bool IsForceFollowSucces() const;
		std::optional<olc::vf2d> GetTargetOldPosition() const;

	protected:
		void OnExecute() override;

	private:
		olc::vf2d FindPossiblePosition(const olc::vf2d& TargetDirection);

		olc::vf2d m_TargetNewPosition;
		std::optional<olc::vf2d> m_TargetOldPosition;
		bool m_bForceFollowSuccess = false;
	};

	

}

