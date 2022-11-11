#include "waveTable.h"
#include <math.h>
#include <iostream>

WaveTable::WaveTable()  {}

WaveTable::WaveTable(int frequency, int sampleRate) : _frequency(frequency), tableSize(sampleRate / frequency) {

	for (int i = 0; i < tableSize; i++)
	{
		sine[i] = (float)sin(((double)i / (double)tableSize) * M_PI * 2.);
	}
}

float WaveTable::get(int index) const {
	return sine[index % tableSize];
}