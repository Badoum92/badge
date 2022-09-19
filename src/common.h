#pragma once

#include <cstdint>

#define ASSERT(COND)          internal::internal_assert((COND), #COND, nullptr, __FILE__, __LINE__)
#define ASSERT_MSG(COND, MSG) internal::internal_assert((COND), #COND, (MSG), __FILE__, __LINE__)

#define BIT(VAR, B) (((VAR) & (1 << (B))) != 0)
#define SET_BIT(VAR, B, X)

namespace internal
{
void internal_assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line);
}

template <typename T>
inline bool bit(const T& var, size_t n)
{
    return var & (1 << n);
}

template <typename T>
inline void set_bit(T& var, size_t n, bool b)
{
    var = b ? var | (1 << n) : var & ~(1 << n);
}
