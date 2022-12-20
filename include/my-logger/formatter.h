//
// Created by Alone on 2022-12-14.
//

#pragma once
#include"common.h"

#include <functional>
#include<string>

LBLOG_NAMESPACE_BEGIN
	struct context
	{ // 用于传递每次输出内容的上下文
		int level;
		int line;
		const char* short_filename{};
		const char* long_filename{};
		std::string text;
	};
	struct Config;

	using formatter_t = std::function<void(Config*, context const&, fmt::memory_buffer&)>;

	namespace formatter
	{
		void defaultFormatter(Config* config, context const& ctx, fmt::memory_buffer& buf);
		void jsonFormatter(Config* config, context const& ctx, fmt::memory_buffer& buf);
	}
LBLOG_NAMESPACE_END
