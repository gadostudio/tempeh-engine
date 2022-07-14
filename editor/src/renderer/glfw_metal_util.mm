#include <GLFW/glfw3.h>
#include <dawn/webgpu_cpp.h>
#import <QuartzCore/CAMetalLayer.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <memory>

std::unique_ptr<wgpu::SurfaceDescriptorFromMetalLayer> SetupWindowAndGetSurfaceDescriptorCocoa(GLFWwindow* window) {
    if (@available(macOS 10.11, *)) {
        NSWindow* nsWindow = glfwGetCocoaWindow(window);
        NSView* view = [nsWindow contentView];

        // Create a CAMetalLayer that covers the whole window that will be passed to
        // CreateSurface.
        [view setWantsLayer:YES];
        [view setLayer:[CAMetalLayer layer]];

        // Use retina if the window was created with retina support.
        [[view layer] setContentsScale:[nsWindow backingScaleFactor]];

        std::unique_ptr<wgpu::SurfaceDescriptorFromMetalLayer> desc =
                std::make_unique<wgpu::SurfaceDescriptorFromMetalLayer>();
        desc->layer = [view layer];
        return desc;
    }

    return nullptr;
}
