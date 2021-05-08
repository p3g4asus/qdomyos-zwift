#ifndef SESSIONLINE_H
#define SESSIONLINE_H
#include <QTimer>
#include <QDateTime>
#define SESSIONLINE_INVALID_ELAPSED 0xFFFFFFFF
class SessionLine
{
public:
    int8_t inclination;
    double distance;
    int8_t resistance;
    int8_t peloton_resistance;
    uint8_t heart;
    double pace;
    QDateTime time;
    double calories;
    double elevationGain;
    uint32_t elapsedTime;
    bool lapTrigger = false;

    SessionLine();
    SessionLine(const SessionLine& cp);
    SessionLine(double speed, int8_t inclination, double distance, uint16_t watt, int8_t resistance, int8_t peloton_resistance, uint8_t heart, double pace, uint8_t cadence, double calories, double elevationGain, uint32_t elapsed, bool lap, QDateTime time = QDateTime::currentDateTime());
    SessionLine& operator+=(SessionLine const& rhs)&;
    SessionLine& operator=(SessionLine const& rhs)&;
    uint16_t getWatt() const {return (uint16_t)(nsum==1?watt:(double)watt / nsum + 0.5);}
    uint8_t getCadence() const {return (uint8_t)(nsum==1?cadence:(double)cadence / nsum + 0.5);}
    double getSpeed() const {return nsum==1?speed:speed / nsum;}
private:
    int nsum = 1;
    double speed;
    uint32_t watt;
    uint32_t cadence;
};

#endif // SESSIONLINE_H
