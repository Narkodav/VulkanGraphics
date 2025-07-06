#pragma once
#include "../Common.h"

class PlatformContext
{
private:
    PlatformContext() {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
    };

    PlatformContext(const PlatformContext&) = delete;
    PlatformContext& operator=(const PlatformContext&) = delete;

    PlatformContext(PlatformContext&&) = delete;
    PlatformContext& operator=(PlatformContext&&) = delete;

public:

    ~PlatformContext()
    {
        glfwTerminate();
    }

    static PlatformContext& instance()
    {
        static PlatformContext s;
        return s;
    }
};