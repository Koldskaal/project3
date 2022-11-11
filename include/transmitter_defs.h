#pragma once
#include <memory>
#include <vector>

typedef enum CallbackResult
{
    Continue = 0,   /**< Signal that the stream should continue invoking the callback and processing audio. */
    Complete = 1,   /**< Signal that the stream should stop invoking the callback and finish once all output samples have played. */
    Abort = 2       /**< Signal that the stream should stop invoking the callback and finish as soon as possible. */
} CallbackResult;

typedef int ReadyToSend(std::shared_ptr<std::vector<int>> newMessage);

struct ToneSlot {
    int x;
    int y;
};
