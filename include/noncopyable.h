//
// Created by Alone on 2022-9-21.
//

#ifndef MY_LOGGER_NONCOPYABLE_H
#define MY_LOGGER_NONCOPYABLE_H

namespace lblog{
    class noncopyable
    {
    public:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };
}
#endif //MY_LOGGER_NONCOPYABLE_H
