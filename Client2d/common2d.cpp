#pragma once
#include "common2d.h"

namespace Simulation2d::Flight
{
	Mission::Mission(Object2d* ptr) : m_ptrObject2d(ptr)
	{

	}

	void Mission::SetObject2d(Object2d* d)
	{
		m_ptrObject2d = d;
	}

	void Mission::Execute()
	{
		if (!m_bIsAchievable)
		{
			return;
		}
		OnExecute();
	}
	
	Object2d Mission::GetObject2d() const
	{
		return *m_ptrObject2d;
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
		m_ptrObject2d->Velocity = { 0.0f, 0.0f };
		Execute();
		m_ptrObject2d->Position += m_ptrObject2d->Velocity;
	}
}