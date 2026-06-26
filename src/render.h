#ifndef RENDER_H
#define RENDER_H

#include "../include/webgpu/wgpu.h"
#include "gui.h"

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include "../include/GLFW/glfw3native.h"

typedef struct {
    WGPUInstance instance;              //The core API/spec entry point (spinning up a new instance).
    WGPUSurface surface;                //The OS Window canvas provider i.e. where to draw whatever (provided and managed by GLFW).
    WGPUAdapter adapter;                //The physical hardware (your GPU).
    WGPUDevice device;                  //The software connection (drivers + some more stuff) to your GPU.
    WGPUQueue queue;                    //The command list that is sent from your CPU to your GPU (adapter) through the drivers (device).
    WGPUTextureFormat swapChainFormat;  //The specific pixel arrangement of your display (kinda).
} RenderContext;

int renderInit(RenderContext *rnctx, GUIContext *ctx);
int renderFrame(RenderContext *rnctx, GUIContext *ctx);
void renderShutDown(RenderContext *rnctx);

#endif
