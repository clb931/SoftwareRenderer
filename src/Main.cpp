#include "Window.h"

int main() {
    clb::Window window("SoftwareRenderer");
    window.Create();

    while (window.HandleMessages() == 0) {
        window.Flip();
    }

    return 0;
}