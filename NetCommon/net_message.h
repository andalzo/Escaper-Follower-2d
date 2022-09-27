/*
License (OLC-3)
Copyright 2018-2022 OneLoneCoder.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1.Redistributions or derivations of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2.Redistributions or derivative works in binary form must reproduce the above copyright notice. This list of conditions and the following disclaimer
must be reproduced in the documentation and/or other materials provided with the distribution.

3.Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "net_common.h"


namespace Simulation2d
{
	namespace Net
	{
		//Message header is sent at start of all messages. The template allows us
		//to use "enum class" to ensure that the messages are valid at compile time
		template<typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		template<typename T>
		struct message
		{
			message_header<T> header{};
			std::vector<uint8_t> body;

			//returns size of entire message packet in bytes

			size_t size() const
			{
				return body.size();
			}

			//Overriding for std::cout compatibility - produces frinedly description of message
			friend std::ostream& operator<<(std::ostream& os, const message<T>& msg)
			{
				os << "ID:" << int(msg.header.id) << "Size:" << msg.header.size;
				return os;
			}

			//Pushes any POD-like data into the message buffer
			template <typename DataType>
			friend message<T>& operator<<(message<T>& msg, const DataType& data)
			{
				//Check that the type of the data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to serialize in sending");

				//Cache current size of vector, as this will be point we insert the data
				size_t i = msg.body.size();

				//Resize the vector by size of the data being pushed
				msg.body.resize(msg.body.size() + sizeof(DataType));

				//Physically copy the data into newly allocated vector space
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				//Recalculate the message size
				msg.header.size = msg.size();

				//Return target msg, so it can be chained
				return msg;
			}

			friend message<T>& operator<<(message<T>& msg, const std::string& data)
			{
				//Cache current size of vector, as this will be point we insert the data
				size_t i = msg.body.size();

				//Resize the vector by size of the data being pushed
				msg.body.resize(msg.body.size() + data.size());

				//Physically copy the data into newly allocated vector space
				std::memcpy(msg.body.data() + i, data.data(), data.size());

				//Recalculate the message size
				msg.header.size = msg.size();

				//Return target msg, so it can be chained
				return msg;
			}


			//Reading data via msg
			template <typename DataType>
			friend message<T>& operator>>(message<T>& msg, DataType& data)
			{
				//Check the type of data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to serialize in getting");

				//Cache the location towards the end of the vector where the pulled data starts
				size_t i = msg.body.size() - sizeof(DataType);

				// Physically copy the data from the vector into user variable
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				//Shrink the vector to remove read bytes, and reset end position
				msg.body.resize(i);

				//Recalculate the msg size
				msg.header.size = msg.size();

				//Return the target message so it can be chained
				return msg;
			}


			friend void operator>>(message<T>& msg, std::string& data)
			{
				data.assign(msg.body.begin(), msg.body.end());

				//Shrink the vector to remove read bytes, and reset end position
				msg.body.resize(0);

				//Recalculate the msg size
				msg.header.size = msg.size();

			}

		};

		template<typename T>
		class connection;

		template<typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};

	}
}