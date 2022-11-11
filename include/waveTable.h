#pragma once

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)
#define SAMPLE_RATE   (44100)

class WaveTable {
public:
	WaveTable();
	WaveTable(int frequency, int sampleRate);
	float get(int index) const;
private:
	int _frequency;
	float sine[TABLE_SIZE];
	int tableSize;
};