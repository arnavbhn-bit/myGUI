#include "render.h"

static void onAdapterRequest(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2) {
    if (status == WGPURequestAdapterStatus_Success) {
        *(WGPUAdapter*)userdata1 = adapter;
    }
}

static void onDeviceRequest(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2) {
    if (status == WGPURequestDeviceStatus_Success) {
        *(WGPUDevice*)userdata1 = device;
    }
}

int renderInit(RenderContext *rnctx, GUIContext *ctx) {
    // 1. Create WebGPU Instance
    WGPUInstanceDescriptor instance = WGPU_INSTANCE_DESCRIPTOR_INIT;
    rnctx->instance = wgpuCreateInstance(&instance);
    if (!rnctx->instance) return -3; // Instance creation failed!

    // 2. Set Up Pure Wayland Surface
    WGPUSurfaceSourceWaylandSurface wlSource = {
        .chain = { .next = NULL, .sType = WGPUSType_SurfaceSourceWaylandSurface },
        .display = glfwGetWaylandDisplay(),
        .surface = glfwGetWaylandWindow(ctx->window)
    };

    // if (!wlSource.display || !wlSource.surface) {
    //     printf("CRITICAL ERROR: Native Wayland handles are NULL! Is GLFW initialized via Wayland?\n");
    //     return -98;
    // }

    WGPUSurfaceDescriptor surfaceDesc = {
        .label = "Native Wayland Surface",
        .nextInChain = (WGPUChainedStruct*)&wlSource
    };

    rnctx->surface = wgpuInstanceCreateSurface(rnctx->instance, &surfaceDesc);
    if (!rnctx->surface) return -4;

    // 3. Request Physical GPU Adapter
    rnctx->adapter = NULL;
    WGPURequestAdapterOptions options = {
        .nextInChain = NULL,
        .compatibleSurface = rnctx->surface,
        .powerPreference = WGPUPowerPreference_HighPerformance
    };

    WGPURequestAdapterCallbackInfo callBackADPTR = {
        .nextInChain = NULL,
        .callback = onAdapterRequest,
        .userdata1 = &rnctx->adapter
    };

    wgpuInstanceRequestAdapter(rnctx->instance, &options, callBackADPTR);
    wgpuInstanceProcessEvents(rnctx->instance);

    if (!rnctx->adapter) return -5;

    // 4. Request Logical Device
    rnctx->device = NULL;
    WGPUDeviceDescriptor deviceDesc = {
        .nextInChain = NULL,
        .label = "My Logical Device",
        .requiredFeatureCount = 0,
        .requiredFeatures = NULL,
        .requiredLimits = NULL
    };

    WGPURequestDeviceCallbackInfo callBackDEVICE = {
        .nextInChain = NULL,
        .callback = onDeviceRequest,
        .userdata1 = &rnctx->device
    };

    wgpuAdapterRequestDevice(rnctx->adapter, &deviceDesc, callBackDEVICE);
    wgpuInstanceProcessEvents(rnctx->instance);

    if (!rnctx->device) return -6;

    // 5. Extract Command Queue
    rnctx->queue = wgpuDeviceGetQueue(rnctx->device);
    if (!rnctx->queue) return -7;

    // 6. Query Capabilities & Build Surface Configuration
    WGPUSurfaceCapabilities capabilities = {0};
    wgpuSurfaceGetCapabilities(rnctx->surface, rnctx->adapter, &capabilities);

    rnctx->swapChainFormat = (capabilities.formatCount > 0)
        ? capabilities.formats[0]
        : WGPUTextureFormat_RGBA8Unorm;

    WGPUCompositeAlphaMode optimalAlphaMode = (capabilities.alphaModeCount > 0)
        ? capabilities.alphaModes[0]
        : WGPUCompositeAlphaMode_Opaque;

    WGPUSurfaceConfiguration surfaceConfig = {
        .device = rnctx->device,
        .format = rnctx->swapChainFormat,
        .usage = WGPUTextureUsage_RenderAttachment,
        .alphaMode = optimalAlphaMode,
        .presentMode = WGPUPresentMode_Fifo, // Solid Wayland VSync
        .width = ctx->width,
        .height = ctx->height
    };

    wgpuSurfaceConfigure(rnctx->surface, &surfaceConfig);
    wgpuSurfaceCapabilitiesFreeMembers(capabilities);

    return 0;
}

// ============================================================================
// Per-Frame Render Execution
// ============================================================================

int renderFrame(RenderContext *rnctx, GUIContext *ctx) {
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(rnctx->surface, &surfaceTexture);

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal) {
        return -8;
    }

    WGPUTextureViewDescriptor viewDesc = {
        .format = rnctx->swapChainFormat,
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
        .aspect = WGPUTextureAspect_All
    };
    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDesc);

    WGPUCommandEncoderDescriptor encDesc = { .label = "Frame Encoder" };
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(rnctx->device, &encDesc);

    WGPURenderPassColorAttachment colorAttachment = {0};

    // Ab un fields ko bharo jo hame chahiye
    colorAttachment.view = targetView;
    colorAttachment.resolveTarget = NULL;
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;
    colorAttachment.clearValue = (WGPUColor){ 0.11, 0.12, 0.15, 1.0 }; // Sleek Gray
    // WebGPU modern specs me depthSlice explicitly default 0 ya WGPU_DEPTH_SLICE_UNDEFINED hona chahiye
    colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    WGPURenderPassDescriptor renderPassDesc = {0};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;
    renderPassDesc.depthStencilAttachment = NULL;

    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
    wgpuRenderPassEncoderEnd(renderPass);

    WGPUCommandBufferDescriptor cmdDesc = { .label = "Command Buffer" };
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, &cmdDesc);

    wgpuQueueSubmit(rnctx->queue, 1, &commandBuffer);
    wgpuSurfacePresent(rnctx->surface);

    // Frame Scope Releases
    wgpuTextureViewRelease(targetView);
    wgpuTextureRelease(surfaceTexture.texture);
    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(commandBuffer);

    return 0;
}

// ============================================================================
// Resource Destruction
// ============================================================================

void renderShutDown(RenderContext *rnctx) {
    if (rnctx->queue) wgpuQueueRelease(rnctx->queue);
    if (rnctx->device) wgpuDeviceRelease(rnctx->device);
    if (rnctx->adapter) wgpuAdapterRelease(rnctx->adapter);
    if (rnctx->surface) wgpuSurfaceRelease(rnctx->surface);
    if (rnctx->instance) wgpuInstanceRelease(rnctx->instance);
}
