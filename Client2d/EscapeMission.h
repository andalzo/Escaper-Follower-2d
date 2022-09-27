#pragma once
#include "Common/common2d.h"

namespace Cevheri::Flight
{



	class EscapeMission : public Mission
	{
	public:
		EscapeMission() = default;
		EscapeMission(std::shared_ptr<Drone> d);
	protected:
		void OnExecute() override;
	private:
	};
}


