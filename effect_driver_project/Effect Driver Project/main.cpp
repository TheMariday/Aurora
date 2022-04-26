#include "LedHarnessCV.h"
#include "thread"
#include "Effect.h"

#include "FightSong.h"

int main()
{
	LedBuffer buffer;
	HarnessCV pose = HarnessCV(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");


	std::thread t(system, "\"\"D:\\Program Files\\VLC\\vlc.exe\" \"C:\\Users\\marti\\Desktop\\fight song_mixdown.mp3\"\"");
	t.detach();

	std::vector<std::shared_ptr<Effect>> effects;

	EffectRunner effectRunner;
	pose.m_wait = true;
	pose.Render(); // this will wait for user to press a key
	pose.m_wait = false;

	timestamp song_start = std::chrono::system_clock::now();

	FightSong(&effectRunner, &pose, song_start);

	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();

		effectRunner.Update(frame_time);

		if (!pose.Render())
			break;

		for (LED& led : buffer.leds)
		{
			led.hsv.v *= 0.9f;
			led.hsv.s *= 0.9f;
		}
	}

	return 0;
}