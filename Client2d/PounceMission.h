#pragma once
#include "Common/common2d.h"

namespace Cevheri::Flight
{
	class PounceMission : public Mission
	{
	public:
		PounceMission() = default;
		explicit PounceMission(std::shared_ptr<Drone> d);
		PounceMission(std::shared_ptr<Drone> main, std::shared_ptr<Drone> target);
		void SetTargetDrone(std::shared_ptr<Drone> target);

	protected:
		void OnExecute() override;

	private:
		std::shared_ptr<Drone> m_target;
		bool m_bIsPounceMissionSucceed = false;
	};
}