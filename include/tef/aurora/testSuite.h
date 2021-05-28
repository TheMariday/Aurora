#pragma once
#include <functional>

namespace TEF::Aurora::TestSuite {

	void Test(std::string testName, std::function<bool()> func);

	bool ButtonTest();

	bool SoundTest();

	bool UserControlTest();

	bool MasterControllerTest();

	bool SmartFuseTest();

	bool AutoTest();

};
