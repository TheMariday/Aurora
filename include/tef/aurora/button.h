#pragma once
#include <wiringPi.h>
#include <chrono>
#include <functional>

#include "tef/aurora/runnable.h"
#include "tef/aurora/dacMCP3008.h"

namespace TEF::Aurora {

	class DacButton : public Runnable
	{
	public:
		DacButton() = default;
		~DacButton() = default;

		bool Connect(DacMCP3008* dac, int pin, std::string name = "unknown", int debounceTime = 100);

		void RegisterCallbackDown(std::function<bool()> callback) { m_downCallback = callback; };
		void RegisterCallbackUp(std::function<bool()> callback) { m_upCallback = callback; };

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