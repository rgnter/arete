#pragma once

#include <chrono>

namespace arete {

struct TickClock
{
public:
    TickClock(float Target) : target(Target) {}

    void setStartPoint();

    bool tick(float & deltaTime);

    float target = 0;

private:
    std::chrono::steady_clock timer;
    std::chrono::steady_clock::time_point currentTime;
    std::chrono::steady_clock::time_point lastTime;

    float deltaSum = 0;
};

}