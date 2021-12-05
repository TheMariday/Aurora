// This entire file needs an overhaul

#include "tef/aurora/safety.h"
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/properties.h"

#include <spdlog/spdlog.h>
#include <iostream>

TEF::Aurora::Safety::Safety()
{
}

bool TEF::Aurora::Safety::Connect()
{
	return m_smartFuse.Connect(Properties::GetProperty<std::string>("fuse", "device").value_or("/dev/ttyUSB0"));
}

void TEF::Aurora::Safety::BuildCurrentMatrix()
{
	EffectRunner effectRunner;

	m_debugEffect = std::make_shared<Effects::DebugEffect>();
	effectRunner.AddEffect(m_debugEffect);

	effectRunner.SetFPS(60);
	effectRunner.Run();

	m_smartFuse.SetFet(7, true);

	int current;

	for (int r = 0; r < 10; r++)
	{
		for (int g = 0; g < 10; g++)
		{
			for (int b = 0; b < 10; b++)
			{
				m_debugEffect->r = r;
				m_debugEffect->g = g;
				m_debugEffect->b = b;

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				m_smartFuse.GetCurrentRaw(7, current);
				m_currentMatrix.data[r][g][b] = (float)current;
				spdlog::debug("rgb: {} {} {}", r, g, b);
			}
		}
	}

	m_smartFuse.SetFet(7, false);

	Properties::SaveProperty<Properties::CurrentMatrix>(m_currentMatrix, m_currentMatrixFilename);

}

void TEF::Aurora::Safety::LoadCurrentMatrix()
{
	Properties::LoadProperty<Properties::CurrentMatrix>(m_currentMatrix, m_currentMatrixFilename);
}

void TEF::Aurora::Safety::PrintCurrentMatrix()
{
	for (int r = 0; r < 10; r++) {
		for (int g = 0; g < 10; g++) {
			for (int b = 0; b < 10; b++) {
				std::cout << m_currentMatrixFiltered.data[r][g][b] << ",";
			}
			std::cout << std::endl;
		}
	}
}
void TEF::Aurora::Safety::FilterCurrentMatrix()
{

	for (int r = 0; r < 10; r++) {
		for (int g = 0; g < 10; g++) {
			for (int b = 0; b < 10; b++) {

				int rgb[] = { r, g, b };

				for (int axis = 0; axis < 3; axis++)
				{
					int axisIndex = rgb[axis];
					switch (axisIndex) {
					case 0:
					case 9:
						m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r][g][b];
						break;
					default:
						m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r + (axis == 0 ? -1 : 0)][g + (axis == 1 ? -1 : 0)][b + (axis == 2 ? -1 : 0)] / 3.0f;
						m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r][g][b] / 3.0f;
						m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r + (axis == 0 ? +1 : 0)][g + (axis == 1 ? +1 : 0)][b + (axis == 2 ? +1 : 0)] / 3.0f;
					}
				}

				m_currentMatrixFiltered.data[r][g][b] /= 3.0f;

			}
		}
	}

}

TEF::Aurora::Safety::~Safety()
{
	m_smartFuse.StopAll();
}