#pragma once

#include <chrono>

namespace arete {

class TickClock
{
  struct Tick {
    bool shouldTick;
    float deltaTime;
  };

public:
  using Clock = std::chrono::steady_clock;

public:
  //! Constructs tick clock.
  //! @param tickPeriod Tick period [T].
  explicit TickClock(float tickPeriod);

  //! Performs tick.
  //! @returns Tick information.
  Tick tick() noexcept;

private:
  Clock::time_point curentTickTime;
  Clock::time_point lastTickTime;

  float tickTimeDelta = 0;
  float tickPeriod = 0;
};

}