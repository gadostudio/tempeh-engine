#include <tempeh/gpu/command_list.hpp>

namespace Tempeh::GPU
{
    CommandList::CommandList()
    {
    }

    CommandList::~CommandList()
    {

    }

    CommandList& CommandList::begin()
    {
        return *this;
    }

    CommandList& CommandList::begin_render_pass()
    {
        return *this;
    }

    CommandList& CommandList::end_render_pass()
    {
        return *this;
    }

    void end()
    {

    }
}
