#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "LinqCpp.hpp"

int main()
{
    std::vector<int> vec { -1, 1, 2, 3, 4 };

    auto maxRet = from(vec).select([](int i){ return i + 2; }).where([](int i){ return i > 2; }).max();
    std::cout << "max: " << maxRet << std::endl;

    auto minRet = from(vec).select([](int i){ return i + 2; }).where([](int i){ return i > 2; }).min();
    std::cout << "min: " << minRet << std::endl;

    auto minmaxRet = from(vec).minmax();
    std::cout << "min: " << *minmaxRet.first << ", max: " << *minmaxRet.second << std::endl;

    auto minmaxRet2 = from(vec).minmax([](int i, int j){ return i > j; });
    std::cout << "min: " << *minmaxRet2.second << ", max: " << *minmaxRet2.first << std::endl;

    auto sumRet = from(vec).sum();
    std::cout << "sum: " << sumRet << std::endl;

    auto countRet = from(vec).count();
    std::cout << "count: " << countRet << std::endl;

    auto aveRet = from(vec).average();
    std::cout << "average: " << aveRet << std::endl;

    std::vector<int> vec2 { 1, 1, 2, 2, 3, 2 };
    std::sort(vec2.begin(), vec2.end());// sort first
    std::cout << "distinct: ";
    from(vec2).distinct().foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "is contains odd: " << from(vec).contains([](int i){ return (i % 2) == 1; }) << std::endl;

    std::cout << "reverse: ";
    from(vec).reverse().foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    auto firstRet = from(vec).first([](int i){ return (i % 2) == 0; });
    std::cout << "first: " << firstRet << std::endl;

    auto lastRet = from(vec).last([](int i){ return (i % 2) == 0; });
    std::cout << "last: " << lastRet << std::endl;

    if (from(vec).any([](int i) { return i < 0; }))
    {
        std::cout << "There are negative elements in the range." << std::endl;
    }

    std::vector<int> vec3 { 2, 4, 6, 8 };
    if (from(vec3).all([](int i){ return (i % 2) == 0; }))
    {
        std::cout << "All the elements are even numbers." << std::endl;
    }

    auto atRet = from(vec).elementAt(1);
    std::cout << "elementAt: " << atRet << std::endl;

    std::map<int, std::string> m;
    m.emplace(1, "Hello");
    m.emplace(2, "world");
    std::cout << "map keys: ";
    from(m).keys().foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;
    std::cout << "map values: ";
    from(m).values().foreach([](const std::string& value){ std::cout << value << " "; });
    std::cout << std::endl;

    std::cout << "take: ";
    from(vec).take(3).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "take: ";
    from(vec).take(0, 3).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "takewhile: ";
    from(vec).takewhile([](int a){ return a > 2; }).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "skip: ";
    from(vec).skip(2).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;
    
    std::cout << "skipwhile: ";
    from(vec).skipwhile([](int i){ return i < 2; }).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "step: ";
    auto stepRet = from(vec).step(2);
    std::for_each(stepRet.begin(), stepRet.end(), [](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::cout << "concat: ";
    from(vec).concat(vec2).foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::vector<int> out;
    from(vec).except(vec3, out);
    std::cout << "except: ";
    std::for_each(out.begin(), out.end(), [](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::vector<int> vec4 { -1, 1 };
    std::cout << "is includes: " << from(vec).includes(vec4) << std::endl;
    std::cout << "is includes: " << from(vec).includes(vec4, [](int i, int j){ return i > j; }) << std::endl;

    std::multimap<int, int> mm = from(vec).groupby([](int i){ return i; });
    for (auto& iter : mm)
    {
        std::cout << "key: " << iter.first << ", value: " << iter.second << std::endl;
    }

    std::vector<double> vec5 { 1.1, 2.3, 3.3 };
    std::cout << "double to int: ";
    from(vec5).cast<int>().foreach([](int i){ std::cout << i << " "; });
    std::cout << std::endl;

    std::vector<int> vec6 (vec.begin(), vec.end());
    std::cout << "equals: " << from(vec6).equals(from(vec)) << std::endl;
    std::cout << "equals: " << (from(vec6) == from(vec)) << std::endl;
    std::cout << "equals: " << (from(vec6) != from(vec)) << std::endl;

    return 0;
}
