#pragma once
#include <cstdint>
#include "olcPGEX_TransformedView.h"

namespace Simulation2d::Net
{
	enum class MsgTypes : uint32_t
	{
		Server_GetStatus,
		Server_GetPing,

		Client_Accepted,
		Client_AssignID,
		Client_RegisterWithServer,
		Client_UnregisterWithServer,

		Simulation_AddObject,
		Simulation_RemoveObject,
		Simulation_UpdateObject
	};
}

namespace Simulation2d::Flight
{

	constexpr int World_X_Limit = 600;
	constexpr int World_Y_Limit = 800;

	struct Object2d
	{
		uint32_t nUniqueID = 0;

		enum class Type : uint8_t
		{
			Follower,
			Escaper
		};

		Type type;

		float fRadius = 8.0f; //Radius of Drone (In 2d we represent drone as circle
		bool bPounced = false; //If drone is in the state of pouncing or escaping mission ?

		olc::vf2d Position;
		olc::vf2d Velocity;

		float SpeedPerFrame;
	};

	class Mission
	{
	public:
		Mission() = default;

		explicit Mission(Object2d* d);
		void SetObject2d(Object2d* d);
		Object2d GetObject2d() const;
		
		void SetIfAchievable(const bool& b);
		void Execute();
		void ExecutePerFrame();
	protected:
		virtual void OnExecute() = 0;
		Object2d* m_ptrObject2d = nullptr;
	private:
		bool m_bIsAchievable = false;
	};

}




