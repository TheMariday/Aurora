#pragma once
#include <string>
#include "easywsclient/easywsclient.hpp"

namespace TEF::Aurora {

	class FadecandyServerComms
	{
	public:
		FadecandyServerComms();
		~FadecandyServerComms();
		bool Connect(std::string url = "ws://192.168.1.109:7890");
		bool GetData();
		
	private:
		void handleMessage(const std::string& message);
		easywsclient::WebSocket::WebSocket::pointer m_pWebsocket;
	};
}