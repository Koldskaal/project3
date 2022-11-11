#pragma once
#include "transmitter_defs.h"
#include "portaudio.h"
#include "DTMF.h"
#include <stdio.h>
#include <string>
#include <math.h>
#include <unordered_map>
#include <queue>
#include <vector>

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER_PA  (64)

/*
    @param duration: The duration of the signal in ms
    @param callback: A callback function. Called when the transmitter needs more input.
    If zero nothing is added to the list it will send nothing in the signal but call again after Duration has
    elapsed.
    */
class Transmitter {
public:
    
    Transmitter(int duration, ReadyToSend * callback);

    bool open();
    bool close();
    bool start();
    bool stop();

    int getStream();

    ~Transmitter() {
        stop();
        close();
    };

private:
    int paCallbackMethod(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    static int paCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);

    static void paStreamFinished(void* userdata);
    void paStreamFinishedMethod();

    int HandleNextHex(double currectTime);

    int _duration;
    bool _initialized = false;
    ReadyToSend * _callback;

    int _phase;
    PaStream* _stream;
    DTMF* _tones;
    std::unordered_map<int, ToneSlot> _hexToSlot;

    int _currentHex = -1;
    double _time = 0;
    double _indexOfMessage = 0;
    
    std::queue<int> _messageRemaining;
    std::shared_ptr<std::vector<int>> _nextMessage = std::make_shared<std::vector<int>>();
};