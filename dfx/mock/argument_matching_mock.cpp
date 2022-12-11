#define CATCH_CONFIG_MAIN
#include "FakeIt/fakeit.hpp"

using namespace fakeit;

namespace testutils
{
	template<typename VecType>
	std::vector<VecType>& multi_emplace(std::vector<VecType>& vec)
	{
		return vec;
	}

	template<typename VecType>
	std::vector<VecType>&& multi_emplace(std::vector<VecType>&& vec)
	{
		return std::move(vec);
	}

	template<typename VecType, typename Head, typename... Tail>
	std::vector<VecType>& multi_emplace(std::vector<VecType>& vec, Head&& head, Tail&&... tail)
	{
	    vec.emplace_back(std::move(head));
	    return multi_emplace(vec, tail...);
	}

	template<typename VecType, typename Head, typename... Tail>
	std::vector<VecType>&& multi_emplace(std::vector<VecType>&& vec, Head&& head, Tail&&... tail)
	{
	    vec.emplace_back(std::move(head));
	    return std::move(multi_emplace(vec, tail...));
	}
}

    std::string formatLineNumner(std::string file, int num){
#ifndef __GNUG__
        return file + std::string("(") + std::to_string(num) + std::string(")");
#else
        return file + std::string(":") + std::to_string(num);
#endif
    }

struct Base {
    virtual ~Base() = default;
    virtual int value() const { return 1; }
};

struct Derivated : public Base {
    int value() const override { return 2; }
};

bool operator==(const Base& lhs, const Base& rhs) {
    return lhs.value() == rhs.value();
}

struct WeirdType
{
    template <typename T>
    WeirdType(T&&){}
};

struct MoveOnlyType {
    const int i_;
    MoveOnlyType(int i) : i_{i} {}
    MoveOnlyType(const MoveOnlyType&) = delete;
    MoveOnlyType(MoveOnlyType&& o) : i_{o.i_} {};
    bool operator==(const MoveOnlyType& o) const { return i_ == o.i_; }
};

struct SomeInterface {
    virtual int func(int) = 0;
    virtual int func2(int, std::string) = 0;
    virtual int func3(const int&) = 0;
    virtual int strfunc(const char*) = 0;
    virtual int funcMoveOnly(MoveOnlyType) = 0;
    virtual int funcVectorOfMoveOnly(std::vector<MoveOnlyType>) = 0;
    virtual int funcSlicing(const Base&) = 0;
    virtual int funcDouble(double) = 0;
    virtual int funcWeirdType(int, WeirdType) = 0;
};

TEST_CASE( "mixed_matchers", "[single-file]" ) {

    Mock<SomeInterface> mock;

    When(Method(mock, func2).Using(_, _)).Return(0);
    When(Method(mock, func2).Using(1, _)).Return(1);
    When(Method(mock, func2).Using(2, _)).Return(2);
    When(Method(mock, func2).Using(_, "3")).Return(3);

    SomeInterface &i = mock.get();

    REQUIRE(0 == i.func2(10, "2"));
    REQUIRE(1 == i.func2(1, "2"));
    REQUIRE(2 == i.func2(2, "2"));
    REQUIRE(3 == i.func2(1, "3"));

    When(Method(mock, func2).Using(Eq(4), "4")).Return(4);
    When(Method(mock, func2).Using(Eq(5), Eq(std::string("5")))).Return(5);
    When(Method(mock, func2).Using(Eq(6), Eq<std::string>("6"))).Return(6);

    REQUIRE(4 == i.func2(4, "4"));
    REQUIRE(5 == i.func2(5, "5"));
    REQUIRE(6 == i.func2(6, "6"));
}

TEST_CASE( "test_eq_matcher", "[single-file]" ) {

    Mock<SomeInterface> mock;

    When(Method(mock, func).Using(Eq(1))).Return(1);
    When(Method(mock, func).Using(Eq(2))).Return(2);

    SomeInterface &i = mock.get();
    REQUIRE(1 == i.func(1));
    REQUIRE(2 == i.func(2));

    Verify(Method(mock, func).Using(Eq(1))).Once();
    Verify(Method(mock, func).Using(Eq(2))).Once();
}

TEST_CASE( "test_gt_matcher", "[single-file]" ) {
    Mock<SomeInterface> mock;

    When(Method(mock, func).Using(Gt(1))).AlwaysReturn(1);
    When(Method(mock, func).Using(Gt(2))).AlwaysReturn(2);

    SomeInterface &i = mock.get();
    REQUIRE(1 == i.func(2));
    REQUIRE(2 == i.func(3));

    Verify(Method(mock, func).Using(Gt(2))).Once();
    Verify(Method(mock, func).Using(Gt(3))).Never();
}

TEST_CASE( "test_vector_of_move_only_type", "[single-file]" ) {
    Mock<SomeInterface> mock;

    When(Method(mock, funcVectorOfMoveOnly).Using(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{10}, MoveOnlyType{15}))).Return(1);
    When(Method(mock, funcVectorOfMoveOnly).Using(Eq(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{20}, MoveOnlyType{25})))).Return(2);

    SomeInterface& i = mock.get();
    REQUIRE(1 == i.funcVectorOfMoveOnly(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{10}, MoveOnlyType{15})));
    REQUIRE(2 == i.funcVectorOfMoveOnly(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{20}, MoveOnlyType{25})));

    Verify(Method(mock, funcVectorOfMoveOnly).Using(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{10}, MoveOnlyType{15}))).Once();
    Verify(Method(mock, funcVectorOfMoveOnly).Using(Eq(testutils::multi_emplace(std::vector<MoveOnlyType>{}, MoveOnlyType{20}, MoveOnlyType{25})))).Once();
}
