#pragma once
#include "micros.h"
LBLOG_NAMESPACE_BEGIN
class noncopyable
{
public:
   noncopyable(const noncopyable&)    = delete;
   void operator=(const noncopyable&) = delete;

protected:
   noncopyable()  = default;
   ~noncopyable() = default;
};
LBLOG_NAMESPACE_END