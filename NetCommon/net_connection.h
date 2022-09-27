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
#include "net_tsqueue.h"
#include "net_message.h"


namespace Simulation2d
{
	namespace Net
	{
		//Forwaed declare
		template<typename T>
		class server_interface;

		template <typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:

			enum class owner
			{
				server,
				client
			};



			connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_asioContext(asioContext) , m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;

				if (m_nOwnerType == owner::server)
				{
					m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

					m_nHandshakeCheck = scramble(m_nHandshakeOut);
				}
				else
				{
					m_nHandshakeIn = 0;
					m_nHandshakeOut = 0;
				}
			}

			virtual ~connection()
			{

			}

			uint32_t GetID() const
			{
				return id;
			}

			void ConnectToClient(Simulation2d::Net::server_interface<T>* server, uint32_t uid = 0)
			{
				if (m_nOwnerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;
						WriteValidation();
						ReadValidation(server);
					}
				}
			}
			
			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (m_nOwnerType == owner::client)
				{
					asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								ReadValidation();
							}
						}
					);
				}
			}
			void Disconnect()
			{
				if (IsConnected())
				{
					asio::post(m_asioContext, [this]() {m_socket.close(); });
				}
			}
			bool IsConnected() const
			{
				return m_socket.is_open();
			};

			void Send(const message<T>& msg)
			{
				asio::post(m_asioContext,
					[this, msg]()
					{
						bool bWritingMessage = !m_qMessagesOut.empty();
						m_qMessagesOut.push_back(msg);
						if (!bWritingMessage)
						{
							WriteHeader();
						}
					}
				);
			}

			//Encrypt data
			uint64_t scramble(uint64_t nInput)
			{
				uint64_t out = nInput^0xAABBCCDDE54;
				return out;
			}

			// ASYNC - Used by both client and server to write validation packet
			void WriteValidation()
			{
				asio::async_write(m_socket, asio::buffer(&m_nHandshakeOut, sizeof(uint64_t)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							//Validation data sent, clients should sit and wait for a response (or a closure)
							if (m_nOwnerType == owner::client)
								ReadHeader();
						}
						else
						{
							m_socket.close();
						}
					}
				);
			}

			void ReadValidation(Simulation2d::Net::server_interface<T>* server = nullptr)
			{
				asio::async_read(m_socket, asio::buffer(&m_nHandshakeIn, sizeof(uint64_t)),
					[this, server](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwnerType == owner::server)
							{
								if (m_nHandshakeIn == m_nHandshakeCheck)
								{
									//Client has provided valid solution, so it can be validated
									std::cout << "Client Validated" << std::endl;

									//User can do whatever do with validation
									server->OnClientValidated(this->shared_from_this());

									//Server sit back to reading header
									ReadHeader();
								}
								else
								{
									//Client gave incorrect data, so disconnect
									std::cout << "Client Disconnected (Fail Validation)" << std::endl;
									m_socket.close();
								}
							}
							else
							{
								//Client solves the puzzle 
								m_nHandshakeOut = scramble(m_nHandshakeIn);

								//It can write validation now
								WriteValidation();
							}
						}
						else
						{
							//Some biggerfailure occured
							std::cout << "Client Disconnected (ReadValidation)" << std::endl;
							m_socket.close();
						}
					}
					);
			}


		protected:
			//Each connection has a unique socket to a remote
			asio::ip::tcp::socket m_socket;

			//This context is shared with the whole asio instance
			asio::io_context& m_asioContext;

			//This queue holds all messages to be sent to the remote side of this connection
			tsqueue<message<T>> m_qMessagesOut;

			//This queue holds all messages that have been received from the remote side
			//of this connection. Note it is a reference as the "owner" of this connection
			//is expected to provide a queue
			tsqueue<owned_message<T>>& m_qMessagesIn;
			message<T> m_msgTemporaryIn;

			//The owner describes how some of the connections behaves
			owner m_nOwnerType = owner::server;
			uint32_t id = 0;

			//Handshake Validation
			uint64_t m_nHandshakeOut = 0;
			uint64_t m_nHandshakeIn = 0;
			uint64_t m_nHandshakeCheck = 0;

		private:
			// ASYNC - Prime context ready to read a message header
			void ReadHeader()
			{
				asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_msgTemporaryIn.header.size > 0)
							{
								m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
								ReadBody();
							}
							else
							{
								AddToIncomingMessageQueue();
							}
						}
						else
						{
							// TODO: Change this with spdlog::error()
							std::cout << "[" << id << "] Read Header Fail.\n";
							m_socket.close();
						}
					}
				);
			}

			// ASYNC - Prime context ready to read a message body
			void ReadBody()
			{
				asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							AddToIncomingMessageQueue();
						}
						else
						{
							// TODO: Change this with spdlog::error()
							std::cout << "[" << id << "] Read Body Fail.\n";
							m_socket.close();
						}
					}
				);
			}

			// ASYNC - Prime context to write a message header
			void WriteHeader()
			{
				asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_qMessagesOut.front().body.size() > 0)
							{
								WriteBody();
							}
							else
							{
								m_qMessagesOut.pop_front();

								if (!m_qMessagesOut.empty())
								{
									WriteHeader();
								}
							}
						}
						else
						{
							// TODO: Change this with spdlog::error()
							std::cout << "[" << id << "] Write Header Fail.\n";
							m_socket.close();
						}
					}
					);
			}

			// ASYNC - Prime context to wirte a message body
			void WriteBody()
			{
				asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							m_qMessagesOut.pop_front();

							if (!m_qMessagesOut.empty())
							{
								WriteHeader();
							}		
						}
						else
						{
							// TODO: Change this with spdlog::error()
							std::cout << "[" << id << "] Write Body Fail.\n";
							m_socket.close();
						}
					}
				);
			}

			void AddToIncomingMessageQueue()
			{
				if (m_nOwnerType == owner::server)
				{
					m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });

				}
				else
				{
					m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });
				}

				ReadHeader();
			}

		};
	}
}