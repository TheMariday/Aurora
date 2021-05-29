#include <spdlog/spdlog.h>
#include <chrono>
#include <atomic>
#include "tef/aurora/runnable.h"
#include "tef/aurora/testSuite.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

struct LED
{
	void Black()
	{
		r = 0; g = 0; b = 0;
	}

	char r, g, b;
	float x, y, z;
	int index;
	std::string name;
};


class Effect
{
public:
	Effect() {
		m_startedAt = std::chrono::high_resolution_clock::now();
	};
	~Effect() {};

	virtual bool Render(std::vector<LED>& leds) = 0;

protected:
	std::chrono::high_resolution_clock::time_point m_startedAt;
};


class EffectRunner : public TEF::Aurora::Runnable
{
public:
	EffectRunner()
	{
		SetFPS(60);
	}

	~EffectRunner() {};

	bool AddEffect(std::shared_ptr<Effect> effect)
	{
		m_effects.push_back(effect);
		return true;
	}

	bool MainLoopCallback() override
	{
		for (LED& led : m_leds)
			led.Black();

		for (auto& effect : m_effects)
			effect->Render(m_leds);

		// send m_leds buffer to hardware
	}


private:
	std::vector<LED> m_leds;
	std::vector<std::shared_ptr<Effect>> m_effects;
};

class RedEffect : public Effect
{
public:
	RedEffect() {};
	~RedEffect() {};

	bool Render(std::vector<LED>& leds) override
	{
		for (LED& led : leds)
			led.r = 255;
		return true;
	}
};


int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	TEF::Aurora::TestSuite::AutoTest();


	//EffectRunner effectRunner;
	//RedEffect redEffect;

	//std::shared_ptr<Effect> redEffect(new RedEffect);

	//effectRunner.AddEffect(redEffect);
	//effectRunner.Run();


	Sleep(10000);
}