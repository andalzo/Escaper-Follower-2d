#pragma once
#include <cstdint>
#include "olcPGEX_TransformedView.h"

namespace Simulation2d::Net
{
	enum class SimMsg: uint32_t
	{
		Server_GetStatus,
		Server_GetPing,

		Client_Accepted,
		Client_Rejected,
		Client_AssignID,
		Client_RegisterWithServer,
		
		Client_PounceStart,
		Client_PounceSuccesful,
		Client_PounceCancel,


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
		bool bPounced = false; //If drone is in the state of pouncing or escaping mission?
		bool bPounceSucces = false; //If pounce is succeed by follower, this is valid just for escaper

		olc::vf2d Position;
		olc::vf2d Velocity;
		olc::Pixel Color;

		float SpeedPerFrame = 1.0f;
	};

	class Mission
	{
	public:
		Mission() = default;

		explicit Mission(Object2d* d);
		void SetObject2d(Object2d* d);
		Object2d GetObject2d() const;
		void Execute();
	
	protected:
		virtual void OnExecute()
		{

		}
		Object2d* m_ptrObject2d = nullptr;
	};

}




