//
// Created by admin on 2021/3/11.
//

#ifndef TPL_SRC_COMM_ENUM_BEAUTIFIER_H
#define TPL_SRC_COMM_ENUM_BEAUTIFIER_H

#include <string_view>

#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

#define EMPTY()

#define EVAL(...) EVAL16(__VA_ARGS__)
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()


#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define CAT(a, b) a ## b

#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

#define MAP(m, first, ...)           \
  m (first)                             \
  IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
    DEFER2(_MAP)()(m, __VA_ARGS__)   \
  )(                                 \
    /* Do nothing, just terminate */ \
  )
#define _MAP() MAP

#define STRINGIFY(x) case std::remove_cvref_t<decltype(v)>::x: return #x;
#define ELEMENT(x) x,
#define ENUM(name, ...)\
EVAL(enum class name{MAP(ELEMENT, __VA_ARGS__)});\
EVAL(std::string_view stringify(const name &v) { switch (v) { MAP(STRINGIFY, __VA_ARGS__) }})\
template<typename OS> OS &operator<<(OS &os, const name &v) { return os << stringify(v); }\
std::string_view stringify(const name &v)

#endif //TPL_SRC_COMM_ENUM_BEAUTIFIER_H
