#include "arete/tickClock.hpp"

namespace arete {

void TickClock::setStartPoint()
{
    currentTime = timer.now();
}

bool TickClock::tick(float & deltaTime)
{
    lastTime = currentTime;
    currentTime = timer.now();

    if (target != 0)
    {
        deltaSum += std::chrono::duration<float, std::chrono::seconds::period>(
            currentTime - lastTime
        ).count();

        if (deltaSum >= 1/target)
        {
            deltaTime = deltaSum;
            deltaSum = 0;
            return true;
        }

        return false;
    }

    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
        currentTime - lastTime
    ).count();
    return true;
}

}