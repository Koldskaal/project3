#include "transmitter.h"

Transmitter::Transmitter(int duration, ReadyToSend * callback) :
	_duration(duration), _callback(callback), _phase(0), _stream(0), _tones(0)
{
	auto lows = new int[] {697, 770, 852, 941};
	auto highs = new int[] {1209, 1336, 1477, 1633};
	_tones = new DTMF(lows, highs, 4, SAMPLE_RATE);
	auto total = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			ToneSlot slot = { i, j };
			_hexToSlot[total] = slot;
			total++;
		}
	}

	_fadeMax = 44100 / (1000.0f / duration) * 0.20f;
	printf("FAAAAAAAAAAAAAAAAAAADE: %d",_fadeMax);

	auto err = Pa_Initialize();
	if (err != paNoError) goto error;

	_initialized = true;
	return;

error:
	fprintf(stderr, "An error occurred while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return;
}

bool Transmitter::open() {
	if (!_initialized) {
		return false;
	}

	int deviceC = Pa_GetDeviceCount();
	printf("Number of devices = %d\n", deviceC);

	auto index = Pa_GetDefaultOutputDevice();

	PaStreamParameters outputParameters;

	outputParameters.device = index;
	if (outputParameters.device == paNoDevice) {
		printf("NO DEVICE!!");
		return false;
	}

	const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
	if (pInfo != 0)
	{
		printf("Output device name: '%s'\n", pInfo->name);
	}

	outputParameters.channelCount = 1;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
		&_stream,
		NULL, /* no input */
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER_PA,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		&Transmitter::paCallback,
		this            /* Using 'this' for userData so we can cast to Transmitter* in paCallback method */
	);

	if (err != paNoError)
	{
		/* Failed to open stream to device !!! */
		return false;
	}

	err = Pa_SetStreamFinishedCallback(_stream, &Transmitter::paStreamFinished);

	if (err != paNoError)
	{
		Pa_CloseStream(_stream);
		_stream = 0;

		return false;
	}

	return true;

}

int Transmitter::paCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData) {

	return ((Transmitter*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
		framesPerBuffer,
		timeInfo,
		statusFlags);
}

int Transmitter::paCallbackMethod(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags) {

	float* out = (float*)outputBuffer;
	unsigned long i;

	if (_time == 0) { //initialize start time
		_time = static_cast<double>(timeInfo->currentTime);
	}

	auto currentTime = static_cast<double>(timeInfo->currentTime);
	// handle current assigned HEX in list and decide what to add to output buffer
	auto err = HandleNextHex(currentTime);
	if (err != paContinue) {
		return err;
	}

	(void)statusFlags;
	(void)inputBuffer;

	for (i = 0; i < framesPerBuffer; i++)
	{
		if (_currentHex < 0 || _currentHex > 15) {
			*out++ = 0;
			continue;
		}

		auto slot = _hexToSlot[_currentHex];
		auto prevSlot = _hexToSlot[_prevHex];


		auto oldTone = _tones->getTone(prevSlot.x, prevSlot.y, _prevPhase);
		auto newTone = _tones->getTone(slot.x, slot.y, _phase);
		*out++ = oldTone * ((double)_fade / _fadeMax) +
			newTone * ((double)(_fadeMax - _fade)/_fadeMax);
		_phase++;
		_prevPhase++;
		if (_fade > 0)
			_fade--;
	}

	return paContinue;
}

void Transmitter::paStreamFinished(void* userData)
{
	((Transmitter*)userData)->paStreamFinishedMethod();
} 

void Transmitter::paStreamFinishedMethod() {
	_stream = 0;
	printf("Stream closed \n");
}

bool Transmitter::close()
{
	if (_stream == 0)
		return false;

	PaError err = Pa_CloseStream(_stream);
	_stream = 0;

	return (err == paNoError);
}


bool Transmitter::start()
{
	if (_stream == 0)
		return false;

	PaError err = Pa_StartStream(_stream);

	return (err == paNoError);
}

bool Transmitter::stop()
{
	if (_stream == 0)
		return false;

	PaError err = Pa_StopStream(_stream);

	return (err == paNoError);
}

int Transmitter::getStream() {
	return (int)_stream;
}

int Transmitter::HandleNextHex(double currentTime) {
	if (currentTime - _time >= (double)_duration * 0.001f) {
		if (_messageRemaining.empty()) {
			_nextMessage->clear();
			auto result = _callback(_nextMessage);
			if (result != 0) 
				return result;
									   
			if (_nextMessage->size() > 0) {
				for (auto var : *_nextMessage)
				{
					_messageRemaining.push(var);
				}
				_nextMessage->clear();
			}
			else {
				_messageRemaining.push(-1);
			}
		}

		_fade = _fadeMax;
		_prevHex = _currentHex;
		_currentHex = _messageRemaining.front();
		_time = currentTime;
		_prevPhase = _phase;
		_phase = 0;
		_messageRemaining.pop();
	}
	return Continue;
}