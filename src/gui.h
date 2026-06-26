#ifndef GUI_H
#define GUI_H

#define GLFW_INCLUDE_NONE
#include "../include/GLFW/glfw3.h"

typedef struct{
    GLFWwindow* window;
    int width;
    int height;
} GUIContext;

int guiInit(GUIContext* ctx, int width, int height, const char* title);
void guiPollEvents(void);
int guiShouldClose(GUIContext* ctx);
void guiTerminate(GUIContext* ctx);

#endif
