#ifndef _TEMPEH_GPU_DETAIL_WRAPPER_H
#define _TEMPEH_GPU_DETAIL_WRAPPER_H

#include <memory>

namespace Tempeh::GPU::Detail
{
    template<typename T>
    class Wrapper
    {
    public:
        Wrapper(std::shared_ptr<T>&& impl) :
            impl_(std::move(impl))
        {
        }

        Wrapper(const std::shared_ptr<T>& impl) :
            impl_(impl)
        {
        }

        Wrapper(const Wrapper<T>& other) :
            impl_(other.impl_)
        {
        }

        Wrapper(Wrapper<T>&& other) :
            impl_(std::move(other.impl_))
        {
        }

        Wrapper& operator=(const Wrapper<T>& other)
        {
            impl_ = other.impl_;
            return *this;
        }

        Wrapper& operator=(Wrapper<T>&& other)
        {
            impl_ = std::move(other.impl_);
            return *this;
        }

    protected:
        std::shared_ptr<T> impl_;
    };
}

#endif