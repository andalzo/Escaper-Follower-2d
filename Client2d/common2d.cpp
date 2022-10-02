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

	
	Object2d Mission::GetObject2d() const
	{
		return *m_ptrObject2d;
	}

	void Mission::Execute()
	{
		if (!m_ptrObject2d)
		{
			return;
		}
		m_ptrObject2d->Velocity = { 0.0f, 0.0f };
		OnExecute();
		m_ptrObject2d->Position += m_ptrObject2d->Velocity;
	}
}