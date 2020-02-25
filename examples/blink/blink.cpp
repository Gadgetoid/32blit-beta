#include "32blit.hpp"

using namespace blit;

extern "C" {
    void init();
    void update(uint32_t time_ms);
    void render(uint32_t time_ms);
}

int main() {
    api.LED.g = 255;
    return 0;
}

void init(void) {
    //set_screen_mode(hires);
    api.LED.b = 200;
}

void update(uint32_t time_ms) {
    api.LED.g = api.joystick.y * 255;
}

void render(uint32_t time_ms) {
    api.LED.r++;// 255;

    screen.pen = Pen(255, 0, 255);
    screen.clear();
}