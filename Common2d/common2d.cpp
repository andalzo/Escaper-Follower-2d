#pragma once
#include "common2d.h"

namespace Cevheri::Flight
{
	Mission::Mission(std::shared_ptr<Drone> d) : m_drone(d)
	{

	}

	void Mission::SetDrone(std::shared_ptr<Drone> d)
	{
		m_drone = d;
	}

	void Mission::Execute()
	{
		if (!m_bIsAchievable)
		{
			return;
		}
		OnExecute();
	}
	Drone Mission::GetDrone() const
	{
		return *m_drone;
	}
	void Mission::SetIfAchievable(const bool& b)
	{
		m_bIsAchievable = b;
	}
	
	void Mission::ExecutePerFrame()
	{
		if (!m_bIsAchievable)
		{
			return;
		}	
		m_drone->Velocity = { 0.0f, 0.0f };
		Execute();
		m_drone->Position += m_drone->Velocity;
		std::cout << "Execute per frame completed.\n";
	}
}