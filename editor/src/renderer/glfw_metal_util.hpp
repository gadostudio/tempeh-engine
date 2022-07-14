#ifndef TEMPEH_ENGINE_GLFW_METAL_UTIL_HPP
#define TEMPEH_ENGINE_GLFW_METAL_UTIL_HPP

#include <memory>

#include <GLFW/glfw3native.h>
std::unique_ptr<wgpu::SurfaceDescriptorFromMetalLayer> SetupWindowAndGetSurfaceDescriptorCocoa(GLFWwindow* window);

#endif //TEMPEH_ENGINE_GLFW_METAL_UTIL_HPP
