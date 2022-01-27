#include <tempeh/util/result.hpp>
#include <tempeh/gpu/instance.hpp>
#include <string>
#include <iostream>

using namespace Tempeh;

int main()
{
    GPU::Instance::initialize(GPU::BackendType::Vulkan);
    
    return 0;
}