#include "common.h"

#include <cstdio>

namespace internal
{
void internal_assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line)
{
    if (!cond)
    {
        fprintf(stderr, "Assertion (%s) failed in %s line %u", cond_str, file, line);
        if (msg != nullptr)
        {
            fprintf(stderr, ": %s\n", msg);
        }
        else
        {
            fprintf(stderr, "\n");
        }
        __debugbreak();
    }
}
} // namespace internal
