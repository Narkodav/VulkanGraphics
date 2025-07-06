#include "FrameRateCalculator.h"

void FrameRateCalculator::setFrameTimeBuffer(int frameTimeBufferSize)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    frameTimes.rename("FrameTimes");
    frameTimes.resize(frameTimeBufferSize);
}

void FrameRateCalculator::addFrameTime(float frameTime)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    frameTimes.addSample(frameTime);
}

float FrameRateCalculator::updateFrameRate()
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    return frameRate = 1 / frameTimes.getAverage();
}

float FrameRateCalculator::getFrameRate() const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return frameRate;
}

void FrameRateCalculator::displayFrameRateToConsole() const {
    // Clear line and move cursor to start
    std::cout << "\r\033[K"
        << "FPS: " << getColorCode(frameRate)
        << std::fixed << std::setprecision(1) << frameRate
        << "\033[0m" << " (" << getLabel(frameRate) << ")"
        << std::flush;
}

std::string FrameRateCalculator::getColorCode(float frameRate) {
    if (frameRate >= 120.0f) return "\033[1;32m";   // Bold Green
    if (frameRate >= 60.0f)  return "\033[32m";     // Green
    if (frameRate >= 30.0f)  return "\033[33m";     // Yellow
    return "\033[31m";                              // Red
}

std::string FrameRateCalculator::getLabel(float frameRate) {
    if (frameRate >= 120.0f) return "Excellent";
    if (frameRate >= 60.0f)  return "Good";
    if (frameRate >= 30.0f)  return "Fair";
    return "Poor";
}
