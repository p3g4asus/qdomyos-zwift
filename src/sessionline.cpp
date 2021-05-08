#include "sessionline.h"

SessionLine::SessionLine(double speed, int8_t inclination, double distance, uint16_t watt, int8_t resistance, int8_t peloton_resistance, uint8_t heart, double pace, uint8_t cadence, double calories, double elevationGain, uint32_t elapsed, bool lap, QDateTime time)
{
    this->speed = speed;
    this->inclination = inclination;
    this->distance = distance;
    this->watt = watt;
    this->resistance = resistance;
    this->peloton_resistance = peloton_resistance;
    this->heart = heart;
    this->pace = pace;
    this->time = time;
    this->cadence = cadence;
    this->calories = calories;
    this->elevationGain = elevationGain;
    this->elapsedTime = elapsed;
    this->lapTrigger = lap;
}

SessionLine::SessionLine() {
    elapsedTime = SESSIONLINE_INVALID_ELAPSED;
}

SessionLine::SessionLine(const SessionLine& cp) {
    this->operator=(cp);
}

SessionLine& SessionLine::operator=(SessionLine const& rhs)& {
    this->speed = rhs.speed;
    this->inclination = rhs.inclination;
    this->distance = rhs.distance;
    this->watt = rhs.watt;
    this->resistance = rhs.resistance;
    this->peloton_resistance = rhs.peloton_resistance;
    this->heart = rhs.heart;
    this->pace = rhs.pace;
    this->time = rhs.time;
    this->cadence = rhs.cadence;
    this->calories = rhs.calories;
    this->elevationGain = rhs.elevationGain;
    this->elapsedTime = rhs.elapsedTime;
    this->lapTrigger = rhs.lapTrigger;
    this->nsum = rhs.nsum;
    return *this;
}
SessionLine& SessionLine::operator+=(SessionLine const& rhs)& {
    if (elapsedTime!=rhs.elapsedTime) {
        this->operator=(rhs);
    }
    else {
        this->speed += rhs.speed;
        this->inclination = rhs.inclination;
        this->distance = rhs.distance;
        this->watt += rhs.watt;
        this->resistance = rhs.resistance;
        this->peloton_resistance = rhs.peloton_resistance;
        this->heart = rhs.heart;
        this->pace = rhs.pace;
        this->time = rhs.time;
        this->cadence += rhs.cadence;
        this->calories = rhs.calories;
        this->elevationGain = rhs.elevationGain;
        this->lapTrigger = rhs.lapTrigger;
        this->nsum ++;
    }
    return *this;
}
