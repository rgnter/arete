#pragma once

#include <chrono>

namespace arete {

struct TickClock
{
    void setup();

    bool tick(float & deltaTime);

    float target = 0;

private:
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point currentTime;
    std::chrono::steady_clock::time_point lastTime;

    float deltaSum;
};

}