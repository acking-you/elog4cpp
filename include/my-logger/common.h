#pragma once
#include "fmt/format.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
#include <string>
#include <ctime>
#include <cstdint>

#define LBLOG_NAMESPACE lblog
#define LBLOG_NAMESPACE_BEGIN namespace LBLOG_NAMESPACE {
#define LBLOG_NAMESPACE_END  }
#define USING_LBLOG using namespace LBLOG_NAMESPACE;
#define USING_LBLOG_DETAIL  using namespace LBLOG_NAMESPACE::detail;

LBLOG_NAMESPACE_BEGIN
	class noncopyable
	{
	 public:
		noncopyable(const noncopyable&) = delete;
		void operator=(const noncopyable&) = delete;

	 protected:
		noncopyable() = default;
		~noncopyable() = default;
	};
LBLOG_NAMESPACE_END
