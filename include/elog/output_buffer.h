#pragma once
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
#include <functional>

#include "any.h"
#include "string_view.h"

namespace elog {
using buffer_t = fmt::memory_buffer;

class buffer_helper
{
public:
   buffer_t* buf{};
   buffer_helper() = default;
   explicit buffer_helper(buffer_t* fmt_buf) : buf(fmt_buf) {}
   auto startWith(const StringView& sv) -> bool
   {
      auto buf_sv = make_view();
      if (sv.size() > buf_sv.size()) { return false; }
      return buf_sv.substr(0, sv.size()) == sv;
   }
   auto endWith(const StringView& sv) -> bool
   {
      auto buf_sv = make_view();
      if (sv.size() > buf_sv.size()) { return false; }
      return buf_sv.substr(buf_sv.size() - sv.size(), sv.size()) == sv;
   }
   auto find(const StringView& sv) -> size_t { return make_view().find(sv); }
   auto rfind(const StringView& sv) -> size_t { return make_view().rfind(sv); }
   auto equal(const StringView& sv) -> bool { return make_view() == sv; }
   void append(const StringView& sv) const
   {
      assert(buf != nullptr);
      if (sv.empty()) { return; }
      buf->append(sv);
   }

   void push_back(char ch) const
   {
      assert(buf != nullptr);
      buf->push_back(ch);
   }

   template <typename... T>
   void formatTo(fmt::format_string<T...> fmt, T&&... args)
   {
      assert(buf != nullptr);
      format_to(std::back_inserter(*buf), fmt, std::forward<T>(args)...);
   }

private:
   [[nodiscard]] auto make_view() const -> StringView
   {
      assert(buf != nullptr);
      return StringView{buf->data(), buf->size()};
   }
};

class OutputBuffer : public buffer_helper
{
private:
   Any_t m_ctx;

public:
   OutputBuffer() = default;
   explicit OutputBuffer(buffer_t* buf) : buffer_helper(buf) {}

   void               setContext(Any_t v) { m_ctx = std::move(v); }
   [[nodiscard]] auto getContext() const -> const Any_t& { return m_ctx; }
   auto               getMutableContext() -> Any_t& { return m_ctx; }
};

using output_buf_t = OutputBuffer;
using callback_t   = std::function<void(output_buf_t&)>;

// RAII for trigger before and after
struct trigger_helper
{
   output_buf_t* buffer{};
   callback_t*   after{};
   trigger_helper(output_buf_t* buffer_, callback_t* before_,
                  callback_t* after_)
     : buffer(buffer_), after(after_)
   {
      assert(buffer_ != nullptr);
      if (before_ && *before_) { before_->operator()(*buffer_); }
   }
   ~trigger_helper()
   {
      assert(buffer != nullptr);
      if (after && *after) { after->operator()(*buffer); }
      buffer->push_back('\n');
   }
};

}   // namespace elog