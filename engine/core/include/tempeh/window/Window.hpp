#pragma once

namespace Tempeh::Core
{
    class Window
    {
    public:
        Window();
        virtual ~Window();

        virtual void process_input() = 0;
    };
}
