#pragma once
#include <wiringPi.h>
#include <chrono>
#include <functional>

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"

namespace TEF::Aurora {

	class Button : public Runnable
	{
	public:
		Button() = default;

		~Button() = default;

		bool Connect(int pin, int debounce);

		bool IsConnected();

		void RegisterCallbackDown(std::function<bool()> callback = {}) { m_downCallback = callback; };
		void RegisterCallbackUp(std::function<bool()> callback = {}) { m_upCallback = callback; };

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
		explicit DacButton();
		~DacButton() = default;

		bool Connect(DacMCP3008* dac, int pin, int debounceTime = 100);

		bool IsConnected();

		void RegisterCallbackDown(std::function<bool()> callback) { m_downCallback = callback; };
		void RegisterCallbackUp(std::function<bool()> callback) { m_upCallback = callback; };

		void SetName(std::string name) { m_name = name; };

	private:

		bool MainLoopCallback() override;

		int VoltageToState(float volts);

		enum {
			DISCONNECTED = -1,
			UP = 0,
			DOWN = 1,
		};

		int m_pin;
		int m_state = UP;

		std::string m_name = "unknown";

		std::function<bool()> m_downCallback;
		std::function<bool()> m_upCallback;

		std::chrono::high_resolution_clock::time_point  m_lastCallback;
		std::chrono::microseconds m_debounce;

		DacMCP3008* m_pDac;
	};
}