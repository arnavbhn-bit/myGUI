#include "gui.h"
#include "render.h"
#include <stdio.h>

int main() {
    GUIContext gui;
    RenderContext renderer;

    int gui_result = guiInit(&gui, 800, 600, "WebGPU Native Window");
    if (gui_result != 0) {
        printf("Failed to init GUI! Error Code: %d\n", gui_result);
        return -1;
    }

    int render_res = renderInit(&renderer, &gui);
    if (render_res != 0) {
        printf("Render Init Failed with code: %d\n", render_res);
        guiTerminate(&gui);
        return -1;
    }

    // Main Loop
    while (!guiShouldClose(&gui)) {
        guiPollEvents();                // Inputs/Events check karo
        renderFrame(&renderer, &gui);   // Screen par render karo
    }

    renderShutDown(&renderer);
    guiTerminate(&gui);

    return 0;
}
