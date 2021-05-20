#include "tef/aurora/smartFuse.h"

#include <spdlog/spdlog.h>
#include <string.h>

#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <vector>

TEF::Aurora::SmartFuse::SmartFuse()
{
}

TEF::Aurora::SmartFuse::~SmartFuse()
{
}

bool TEF::Aurora::SmartFuse::Connect()
{

	return false;
}

bool TEF::Aurora::SmartFuse::SetFet(int channel, bool enabled)
{
	std::string s = std::to_string(channel * 2 + int(enabled));
	
	return Write(s);
}

bool TEF::Aurora::SmartFuse::GetFet(int channel, bool& enabled)
{
	enabled = m_fetStates[channel];
	return true;
}

bool TEF::Aurora::SmartFuse::GetCurrent(int channel, float& current)
{
	current = m_currentReadings[channel];
	return true;
}

bool TEF::Aurora::SmartFuse::StopAll()
{
	for (int channel = 0; channel < m_channels; channel++)
	{
		if (!SetFet(channel, false))
		{
			spdlog::error("Smart Fuse cannot stop channel {}", channel);
		}
		
	}
	return true;
}

bool TEF::Aurora::SmartFuse::MainLoopCallback()
{
	/*
	char buffer[100];
	ssize_t length = read(levelSensor, &buffer, sizeof(buffer));
	if (length == -1)
	{
		cerr << "Error reading from serial port" << endl;
		break;
	}
	else if (length == 0)
	{
		cerr << "No more data" << endl;
		break;
	}
	else
	{
		buffer[length] = '\0';
		cout << buffer; //Read serial data
	}


	msg = self.ard.read_until(self.delimeter);

	if len(msg) != self.packet_size:
	print("Recieved message not long enough!")
		return False


		for board in range(8) :
			lsb, msb, fet = msg[board * 3:board * 3 + 3]

			current = (msb << 8 | lsb)

			#current += self.calibrationOffset
			#current *= self.calibrationMultiplier

			self.boardCurrent[board] = current
			self.boardFET[board] = fet
*/
}
