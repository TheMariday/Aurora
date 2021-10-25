#include "tef/aurora/fadecandyServerComms.h"

#include <spdlog/spdlog.h>

#include <string>


TEF::Aurora::FadecandyServerComms::FadecandyServerComms()
{
	m_stateChangeCallback = [](std::string, bool) {
		spdlog::debug("Fadecandy Server Comms state change callback hit with no registered callback");
	};
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

	m_pWebsocket->send("{\"type\": \"list_connected_devices\"}");

	return true;
}

bool TEF::Aurora::FadecandyServerComms::MainLoopCallback()
{
	if (!m_pWebsocket)
	{
		spdlog::error("Fadecandy Server Comms failed to run mainloop as websocket has not been connected");
		return false;
	}


	if (m_pWebsocket->getReadyState() == easywsclient::WebSocket::WebSocket::CLOSED)
	{
		spdlog::error("Fadecandy Server Comms failed to communicate with server as Websocket has been closed");
		return false;
	}

	m_pWebsocket->poll();
	m_pWebsocket->dispatch([this](const std::string& message) {
		this->handleMessage(message);
		});

	return true;
}

bool TEF::Aurora::FadecandyServerComms::StateChangeCallback(std::function<void(std::string, bool)> callback)
{
	m_stateChangeCallback = callback;
	return true;
}

void TEF::Aurora::FadecandyServerComms::handleMessage(const std::string& message)
{
	std::string msg = message;


	std::vector<std::string> newDevices;

	std::size_t found = 0;
	while (true)
	{
		found = msg.find("serial");
		if (found == std::string::npos)
			break;
		std::string serial = msg.substr(found + 9, 16);
		msg = msg.substr(found + 9 + 16);
		newDevices.push_back(serial);
	}

	for (std::string oldDevice : m_serialDevices)
		if (std::find(newDevices.begin(), newDevices.end(), oldDevice) == newDevices.end()) // old device cannot be found so device is disconnected
			m_stateChangeCallback(oldDevice, false);


	for (std::string newDevice : newDevices)
		if (std::find(m_serialDevices.begin(), m_serialDevices.end(), newDevice) == m_serialDevices.end()) // new device found so device is connected
			m_stateChangeCallback(newDevice, true);

	m_serialDevices = newDevices;
}