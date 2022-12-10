#ifndef H63DF6225_7F44_4937_B1C5_795D41382D59
#define H63DF6225_7F44_4937_B1C5_795D41382D59

#include <tlp/int/IntType.h>

TLP_NS_BEGIN

template<typename T> struct Inc;

template<int V>
struct Inc<IntType<V>>
{
    using Result = IntType<V + 1>;
};

TLP_NS_END

#define __inc(...)    typename TLP_NS::Inc<__VA_ARGS__>::Result

#endif
