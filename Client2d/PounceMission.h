#pragma once
#include "common2d.h"

namespace Simulation2d::Flight
{
	class PounceMission : public Mission
	{
	public:
		PounceMission() = default;
		explicit PounceMission(std::shared_ptr<Object2d> d)
		{

		}
		PounceMission(std::shared_ptr<Object2d> main, std::shared_ptr<Object2d> target)
		{

		}
		void SetTargetDrone(std::shared_ptr<Object2d> target)
		{

		}

	protected:
		void OnExecute() override
		{

		}

	private:
		std::shared_ptr<Object2d> m_target;
		bool m_bIsPounceMissionSucceed = false;
	};
}