#include "Engine.h"

int main() {
    Engine engine;

    engine.init("Engine test", "Test", Extent());
    engine.run();
    engine.cleanup();
    return 0;
}