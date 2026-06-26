#include "gui.h"

int guiInit(GUIContext *ctx, int width, int height, const char *title){

    ctx -> width = width;
    ctx -> height = height;

    if(!glfwInit()){
        return -1; // Initialization Failed
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    ctx -> window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!ctx -> window){
        glfwTerminate();
        return -2; //Window Creation Failed
    }

    return 0;
}

void guiPollEvents(void){
    glfwPollEvents();
}

int guiShouldClose(GUIContext *ctx){
    return glfwWindowShouldClose(ctx -> window);
}

void guiTerminate(GUIContext *ctx){
    if(ctx -> window){
        glfwDestroyWindow(ctx -> window);
    }
    glfwTerminate();
}
