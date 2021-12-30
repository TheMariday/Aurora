#pragma once
#include "tef/aurora/runnable.h"
#include "functional"

namespace TEF::Aurora {
	class CLI: public Runnable
	{
	public:
		CLI() = default;
		~CLI() = default;

		void RegisterCommandCallback(std::function<void(std::string command)> cb) { m_commandCallback = cb; };
		void RegisterConfirm(std::function<bool()> callback) { m_confirmCallback = callback; };

	private:
		bool MainLoopCallback() override;
		std::function<void(std::string command)> m_commandCallback;
		std::function<bool()> m_confirmCallback;
	};
}