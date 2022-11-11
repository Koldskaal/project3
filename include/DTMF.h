#pragma once
#include "waveTable.h"
#include <memory>

class DTMF {
public:
	DTMF(int* lowTones, int* highTones, int amount, int samplerate);

	float getTone(int low, int high, int index);
private:
	int _length;
	std::unique_ptr<WaveTable[]> _lowTones;
	std::unique_ptr<WaveTable[]> _highTones;
	int _sampleRate;
};