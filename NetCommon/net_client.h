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
#include "net_connection.h"

namespace Simulation2d
{
	namespace Net
	{
		template <typename T>
		class client_interface
		{
		
		public:
			client_interface() : m_socket(m_context)
			{
				//Initialise the socket with the io context, so it can do stuff
			}


			virtual ~client_interface()
			{
				// If the client is destroyed, always try and disconnect from server
				Disconnect();
			}

			//Connect to server with hostname/ip-address and port
			bool Connect(const std::string& host, const uint16_t& port)
			{
				try
				{
					//Resolve hostname/ip-address inti tangiable physical address
					asio::ip::tcp::resolver resolver(m_context);
					asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					//Create connection
					m_connection = std::make_unique<connection<T>>(
						connection<T>::owner::client,
						m_context,
						asio::ip::tcp::socket(m_context),
						m_qMessagesIn
						);

					//Tell the connection object to connect the server
					m_connection->ConnectToServer(endpoints);

					//Start Context Thread
					thrContext = std::thread([this]() {m_context.run(); });

				}
				catch (std::exception& e)
				{
					//TODO: Replace this line with appropriate logging module
					std::cerr << "Client Exception: " << e.what() << "\n";
					return false;
				}

				return true;
			}
		
			void Disconnect()
			{
				if (IsConnected())
				{
					m_connection->Disconnect();
				}

				//Either way, we are also done with the asio context and its thread
				m_context.stop();
				if (thrContext.joinable())
				{
					thrContext.join();
				}

				m_connection.release();
			}
			
			bool IsConnected() const
			{
				if (m_connection)
				{
					return m_connection->IsConnected();
				}
				return false;
			}
			tsqueue<owned_message<T>>& InComing()
			{
				return m_qMessagesIn;
			}

			void Send(const message<T>& msg)
			{
				if (IsConnected())
					m_connection->Send(msg);
			}

		private:
			//asio context handles the data transfer
			asio::io_context m_context;
			// ..but needs a thread of its own to execute its work commands
			std::thread thrContext;
			//This is the hardware socket that is connected to the server
			asio::ip::tcp::socket m_socket;
			//The client has a single instance of a "connection" object, which handles data transfer
			std::unique_ptr<connection<T>> m_connection;
			//Thread safe queue of incoming messages from server
			tsqueue<owned_message<T>> m_qMessagesIn;

		};
	}
}