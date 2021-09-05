#pragma once
#include "tef/aurora/effects/debugEffect.h"
#include "tef/aurora/properties.h"
#include "tef/aurora/smartFuse.h"

#include <memory>

// this is stil very much a work in progress

namespace TEF::Aurora {

	class Safety
	{
	public:
		Safety();

		~Safety();

		bool Connect();

		void BuildCurrentMatrix();

		void LoadCurrentMatrix();

		void PrintCurrentMatrix();

		void FilterCurrentMatrix();

	private:
		std::shared_ptr<Effects::DebugEffect> m_debugEffect;
		TEF::Aurora::SmartFuse m_smartFuse;

		Properties::CurrentMatrix m_currentMatrix;
		Properties::CurrentMatrix m_currentMatrixFiltered;

		std::string m_currentMatrixFilename = "CurrentMatrix.bin";
	};
};
