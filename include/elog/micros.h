#pragma once

#define LBLOG_NAMESPACE       elog
#define LBLOG_NAMESPACE_BEGIN namespace LBLOG_NAMESPACE {
#define LBLOG_NAMESPACE_END   }
#define USING_LBLOG           using namespace LBLOG_NAMESPACE;
#define USING_LBLOG_DETAIL    using namespace LBLOG_NAMESPACE::detail;

// #define F_EXPAND(x)                                            x
// #define F_GET_MICRO(v1, v2, v3, v4, v5, v6, v7, v8, NAME, ...) NAME
// #def ine F_PASTE(...) \
//    F_EXPAND(F_GET_MICRO(__VA_ARGS__, F_SET7, F_SET6, F_SET5, F_SET4, F_SET3,
//     \
//                         F_SET2, F_SET1, F_SET0)(__VA_ARGS__))
//
// #define F_SET1(func, v1)             func(v1)
// #define F_SET2(func, v1, v2)         F_SET1(func, v1) F_SET1(func, v2)
// #define F_SET3(func, v1, v2, v3)     F_SET1(func, v1) F_SET2(func, v2, v3)
// #define F_SET4(func, v1, v2, v3, v4) F_SET1(func, v1) F_SET3(func, v2, v3,
// v4)
// #def ine F_SET5(func, v1, v2, v3, v4, v5) \
//    F_SET1(func, v1) F_SET4(func, v2, v3, v4, v5)
// #def ine F_SET6(func, v1, v2, v3, v4, v5, v6) \
//    F_SET1(func, v1) F_SET5(func, v2, v3, v4, v5, v6)
// #def ine F_SET7(func, v1, v2, v3, v4, v5, v6, v7) \
//    F_SET1(func, v1) F_SET6(func, v2, v3, v4, v5, v6, v7)
// #define CAST_INT(x)  static_cast<int>(x)
// #define CAST_FLAG(x) static_cast<LBLOG_NAMESPACE::Flags>(x)
//
// #define FLAG_ADD(x) | CAST_INT(x)
// #def ine FLAGS(v1, ...) \
//    CAST_FLAG(CAST_INT(v1) F_EXPAND(F_PASTE(FLAG_ADD, ##__VA_ARGS__)))
