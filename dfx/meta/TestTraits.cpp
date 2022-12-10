#include <tlp/test/Test.hpp>
#include <tlp/traits/IsConvertible.h>
#include <tlp/traits/IsBaseOf.h>
#include <tlp/traits/IsBuiltIn.h>
#include <tlp/traits/LambdaTraits.h>

FIXTURE(TestTraits)
{
    TEST("try convert the convertible types")
    {
        ASSERT_TRUE(__is_convertible(char, int));
        ASSERT_FALSE(__is_convertible(char, void*));
        ASSERT_TRUE(__is_convertible(char*, void*));
    };

    TEST("try convert the hierarchy types")
    {
        struct Base{};
        struct Derived : Base {};

        ASSERT_FALSE(__is_convertible(Base, Derived));
        ASSERT_TRUE(__is_convertible(Derived, Base));

        ASSERT_FALSE(__is_convertible(Base*, Derived*));
        ASSERT_TRUE(__is_convertible(Derived*, Base*));
    };

    TEST("try convert the both convertible types")
    {
        ASSERT_TRUE(__is_both_convertible(short, int));
        ASSERT_FALSE(__is_both_convertible(char*, void*));
    };

    TEST("try test inherits relationship of the super sub types")
    {
        struct Base{};
        struct Derived : Base {};

        ASSERT_FALSE(__is_base_of(int, char));
        ASSERT_FALSE(__is_base_of(void*, char*));
        ASSERT_TRUE(__is_base_of(Base, Derived));
        ASSERT_FALSE(__is_base_of(Derived, Base));
    };

    TEST("estimate a type whether a built in type")
    {
        struct Object {};

        ASSERT_TRUE(__is_built_in(char));
        ASSERT_FALSE(__is_built_in(Object));
    };

    TEST("calculate the return type and parameter types of a lambda")
    {
        void run()
        {
            auto f = [](const int* x, char y){ return *x + y; };
            using Lambda = decltype(f);

            ASSERT_EQ(__lambda_return(Lambda), int);
            ASSERT_EQ(__lambda_paras(Lambda), __type_list(const int*, char));
            ASSERT_EQ(__lambda_para(Lambda, 0), const int*);
            ASSERT_EQ(__lambda_para(Lambda, 1), char);
            ASSERT_EQ(__lambda_para(Lambda, 2), __null_type());
        }
    };
};
