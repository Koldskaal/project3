#include "transmitter.h"

int counter = 0;
int messageCreator(std::shared_ptr<std::vector<int>> message) {
    counter++;
    if (counter > 15) return Complete;

    auto number = rand() % 15;
    message->push_back(number);
    
    return Continue;
}

/*******************************************************************/
int main(void)
{
    Transmitter transmitter(100, &messageCreator);

    printf("PortAudio Test: output two sine waves. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER_PA);
    auto dive = Pa_GetDefaultOutputDevice();
    if (transmitter.open()) {
        if (transmitter.start()) {
            while (transmitter.getStream() != 0) {

            }
        }
    }

    printf("Test finished.\n");
    return 0;
}