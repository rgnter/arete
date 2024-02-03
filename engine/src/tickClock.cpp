#include "arete/tickClock.hpp"

namespace arete {

TickClock::TickClock(float tickPeriod)
    : tickPeriod(tickPeriod)
{}

TickClock::Tick TickClock::tick() noexcept
{
  using FloatingPointDuration = std::chrono::duration<
    float,
    std::chrono::seconds::period>;

  lastTickTime = curentTickTime;
  curentTickTime = Clock::now();

  tickTimeDelta = FloatingPointDuration(curentTickTime - lastTickTime).count();

  if (tickPeriod == 0.0f)
  {
    return {true, tickTimeDelta};
  }

  if (tickTimeDelta >= 1/tickPeriod)
  {
    tickTimeDelta = 0;
    return {true, tickTimeDelta};
  }

  return {false, tickTimeDelta};
}

} // namespace arete