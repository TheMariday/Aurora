#pragma once
#include <string>
#include <vector>
#include <functional>
#include "easywsclient/easywsclient.hpp"
#include "tef/aurora/runnable.h"


namespace TEF::Aurora {

	class FadecandyServerComms : public Runnable
	{
	public:
		FadecandyServerComms();
		~FadecandyServerComms();
		bool Connect(std::string url = "ws://192.168.1.109:7890");

		bool MainLoopCallback() override;

		bool getConnected(std::vector<std::string>& connectedSerialIDs) { connectedSerialIDs = m_serialDevices; return true; };

		bool StateChangeCallback(std::function<void(std::string, bool)> callback);

	private:
		void handleMessage(const std::string& message);
		easywsclient::WebSocket::WebSocket::pointer m_pWebsocket;

		std::vector<std::string> m_serialDevices;

		std::function<void(std::string, bool)> m_stateChangeCallback;

	};
}