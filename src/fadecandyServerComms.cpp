#include "tef/aurora/fadecandyServerComms.h"

#include <assert.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include <spdlog/spdlog.h>


TEF::Aurora::FadecandyServerComms::FadecandyServerComms()
{

}

TEF::Aurora::FadecandyServerComms::~FadecandyServerComms()
{
	if (m_pWebsocket)
		m_pWebsocket->close();

	delete m_pWebsocket;
}

bool TEF::Aurora::FadecandyServerComms::Connect(std::string url)
{
	m_pWebsocket = easywsclient::WebSocket::WebSocket::from_url(url);
	if (!m_pWebsocket)
	{
		spdlog::error("Cannot connect to server websocket");
		return false;
	}

	return 0;
}

bool TEF::Aurora::FadecandyServerComms::GetData()
{
	m_pWebsocket->send("{\"type\": \"list_connected_devices\"}");
	while (m_pWebsocket->getReadyState() != easywsclient::WebSocket::WebSocket::CLOSED) {
		m_pWebsocket->poll();
		m_pWebsocket->dispatch([this](const std::string& message) {this->handleMessage(message); });
	}
}

void TEF::Aurora::FadecandyServerComms::handleMessage(const std::string& message)
{
	spdlog::info("New message: {}", message);

	std::string msg = message;

	std::size_t found = 0;
	while (true)
	{
		found = msg.find("serial");
		if (found == std::string::npos) break;
		std::string serial = msg.substr(found + 9, 16);
		msg = msg.substr(found + 9 + 16);
		spdlog::info("Serial: {}", serial);
	}

	/*
	{"type":"list_connected_devices",
	 "devices":[
		{"type":"fadecandy","serial":"FFSCBFHTYCWNHVMZ","timestamp":1630754082792,"version":"1.07","bcd_version":263},
		{"type":"fadecandy","serial":"CSLTUEQOVVMNABUV","timestamp":1630754082805,"version":"1.07","bcd_version":263}
		]
	}
	*/

}