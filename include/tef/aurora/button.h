#pragma once
#include <wiringPi.h>
#include <chrono>
#include <functional>
#include <vector>

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"

namespace TEF::Aurora {

	class Button : public Runnable
	{
	public:
		Button() = default;

		~Button() = default;

		bool Connect(int pin);

		bool IsConnected();

		bool RegisterCallbackDown(std::function<bool()> callback = {}) { m_downCallback = callback; };
		bool RegisterCallbackUp(std::function<bool()> callback = {}) { m_upCallback = callback; };

	private:

		bool MainLoopCallback() override;

		int m_pin = -1;
		bool m_state = 0;

		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;

		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;
	};

	class DacButton : public Runnable
	{
	public:
		DacButton() = default;
		~DacButton() = default;

		bool Connect(DacMCP3008* dac, int pin, int debounceTime = 100, int refreshRate = 100);

		bool IsConnected();

		bool RegisterCallbackDown(std::function<bool()> callback = {}) { m_downCallback = callback; };
		bool RegisterCallbackUp(std::function<bool()> callback = {}) { m_upCallback = callback; };
		bool RegisterCallbackDisconnect(std::function<bool()> callback = {}) { m_disconnectCallback = callback; };
		bool RegisterCallbackReconnect(std::function<bool()> callback = {}) { m_reconnectCallback = callback; };

	private:

		bool MainLoopCallback() override;

		int VoltageToState(voltage volts);

		int m_pin;
		int m_state = 0;

		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;
		std::function<bool()> m_disconnectCallback;
		std::function<bool()> m_reconnectCallback;

		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;

		DacMCP3008* m_pDac;
	};
}