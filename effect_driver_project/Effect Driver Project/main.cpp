#include "LedHarness.h"
#include "thread"
#include "Effect.h"

#include "FightSong.h"

int main()
{
	LedBuffer buffer;
	Harness pose = Harness(&buffer, "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\led_positions.csv");


	std::thread t(system, "\"\"D:\\Program Files\\VLC\\vlc.exe\" \"C:\\Users\\marti\\Desktop\\fight song_mixdown.mp3\"\"");
	t.detach();

	std::vector<std::shared_ptr<Effect>> effects;

	EffectRunner effectRunner;

	pose.RenderToScreen(); // this will wait for user to press a key

	timestamp song_start = std::chrono::system_clock::now();

	FightSong(&effectRunner, &pose, song_start);

	while (true)
	{
		timestamp frame_time = std::chrono::system_clock::now();

		effectRunner.Update(frame_time);

		if (pose.RenderToScreen(false, 0) == 27)
			break;

		for (LED& led : buffer.leds)
		{
			led.hsv.v *= 0.9f;
			led.hsv.s *= 0.9f;
		}
	}

	return 0;
}