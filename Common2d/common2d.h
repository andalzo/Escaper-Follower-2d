#pragma once
#include <cstdint>
#include "olcPGEX_TransformedView.h"

namespace Cevheri::Net
{
	enum class MsgTypes : uint32_t
	{
		Server_GetStatus,
		Server_GetPing,

		Drone_Accepted,
		Drone_AssignID,
		Drone_RegisterWithServer,
		Drone_UnregisterWithServer,

		GS_AddDrone,
		GS_RemoveDrone,
		GS_UpdateDrone
	};
}

namespace Cevheri::Flight
{

	constexpr int World_X_Limit = 600;
	constexpr int World_Y_Limit = 800;

	struct Drone
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

		explicit Mission(std::shared_ptr<Drone> d);
		void SetDrone(std::shared_ptr<Drone> d);
		Drone GetDrone() const;
		
		void SetIfAchievable(const bool& b);
		void Execute();
		void ExecutePerFrame();
	protected:
		virtual void OnExecute()
		{

		}
		std::shared_ptr<Drone> m_drone;
	private:
		bool m_bIsAchievable;
	};

}


namespace Cevheri::Math
{

	template <typename T>
	class LangrangeInterpolator
	{
	private:
		std::vector<olc::v2d_generic<T> > data{};
	public:
		LangrangeInterpolator() = default;

		explicit LangrangeInterpolator(const std::vector< olc::v2d_generic<T> >& d) : data(d)
		{

		}

		LangrangeInterpolator(std::vector<T> inputs, std::vector<T> outputs)
		{
			if (inputs.size() != outputs.size())
			{
				// TODO: Use fast logging library here, instead of std::cerr; 
				std::cerr << "[LangrangeInterpolater] Vector size do not match! Program is shutting down\n";
				std::exit(EXIT_FAILURE);
			}

			if (inputs.empty())
			{
				data.push_back(olc::v2d_generic<T>{0, 0});
				return;
			}


			for (uint32_t i = 0; i < inputs.size(); i++)
			{
				olc::v2d_generic<T> new_data{ inputs.at(i), outputs.at(i) };
				data.push_back(new_data);
			}
		}

		void Set(const std::vector< olc::v2d_generic<T> >& d)
		{
			data = d;
		}

		void AddData(const olc::v2d_generic<T>& new_data)
		{
			data.push_back(new_data);
		}

		void Reset()
		{
			data.clear();
		}

		void ShowData() const
		{
			for (const auto& d : data)
			{
				//Should be used spdlog here
				std::cout << "(" << d.input << "," << d.output << ")\n";
			}
		}

		std::vector<olc::v2d_generic<T>>& get_data() const
		{
			return data;
		}



		// function to interpolate the given data points using Lagrange's formula with memoization
		T operator()(const T& value)
		{
			T result = 0; // Initialize result
			for (const olc::v2d_generic<T>& updata : this->data)
			{
				if (value == updata.x)
				{
					//If we know the value, we do not have to calculete it
					return updata.y;
				}

				T term = updata.y; // Compute individual terms of above formula
				for (const olc::v2d_generic<T>& subdata : this->data)
				{
					if (updata.input != subdata.input)
						term = term * (value - subdata.x) / T(updata.x - subdata.y);
				}

				// Add current term to result
				result += term;
			}

			olc::v2d_generic<T> new_data{ value, result };
			add_data(new_data);

			return result;
		}

	};

}

namespace Cevheri::Flight
{
	template<typename T>
	struct Route
	{
		Math::LangrangeInterpolator<T> X;
		Math::LangrangeInterpolator<T> Y;
	};
}

