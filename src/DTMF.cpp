#include "DTMF.h"

DTMF::DTMF(int* lowTones, int* highTones, int length, int sampleRate) : _length(length), _sampleRate(sampleRate) {
	_lowTones = std::make_unique<WaveTable[]>(_length);
	for (size_t i = 0; i < length; i++)
	{
		_lowTones[i] = * new WaveTable(lowTones[i], sampleRate);
	}

	_highTones = std::make_unique<WaveTable[]>(_length);
	for (size_t i = 0; i < length; i++)
	{
		_highTones[i] = * new WaveTable(highTones[i], sampleRate);
	}
}

float DTMF::getTone(int low, int high, int index) {
	if (low >= _length  || high >= _length || low < 0 || high < 0) {
		// Error tone out of bounds
		return 0;
	}

	auto lowTone = _lowTones[low].get(index);
	auto highTone = _highTones[high].get(index);

	return (lowTone + highTone) * 0.5f;
}