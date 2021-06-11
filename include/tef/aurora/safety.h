#pragma once
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/smartFuse.h"
#include "tef/aurora/effects/debugEffect.h"
#include "tef/aurora/properties.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>

namespace TEF::Aurora {

	class Safety
	{
	public:
		Safety()
		{
			m_smartFuse.Connect();
			m_smartFuse.Run();
		};

		~Safety()
		{
			m_smartFuse.StopAll();
		};

		void BuildCurrentMatrix()
		{
			EffectRunner effectRunner;

			m_debugEffect = std::make_shared<Effects::DebugEffect>();
			effectRunner.AddEffect(m_debugEffect);

			effectRunner.Run();

			m_smartFuse.SetFet(7, true);

			float current;

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
						m_smartFuse.GetCurrent(7, current);
						m_currentMatrix.data[r][g][b] = current - 0.65;
						spdlog::debug("rgb: {} {} {}", r, g, b);
					}
				}
			}

			m_smartFuse.SetFet(7, false);

			effectRunner.Stop();

			Properties::SaveProperty<Properties::CurrentMatrix>(m_currentMatrix, m_currentMatrixFilename);

		};

		void LoadCurrentMatrix()
		{
			Properties::LoadProperty<Properties::CurrentMatrix>(m_currentMatrix, m_currentMatrixFilename);
		}

		void PrintCurrentMatrix()
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

		void FilterCurrentMatrix()
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
								m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r + (axis == 0 ? -1 : 0)][g + (axis == 1 ? -1 : 0)][b + (axis == 2 ? -1 : 0)] / 3.0;
								m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r][g][b] / 3.0;
								m_currentMatrixFiltered.data[r][g][b] += m_currentMatrix.data[r + (axis == 0 ? +1 : 0)][g + (axis == 1 ? +1 : 0)][b + (axis == 2 ? +1 : 0)] / 3.0;
							}
						}

						m_currentMatrixFiltered.data[r][g][b] /= 3.0;

					}
				}
			}

		}

	private:
		std::shared_ptr<Effects::DebugEffect> m_debugEffect;
		TEF::Aurora::SmartFuse m_smartFuse;

		Properties::CurrentMatrix m_currentMatrix;
		Properties::CurrentMatrix m_currentMatrixFiltered;

		std::string m_currentMatrixFilename = "CurrentMatrix.bin";
	};
};
