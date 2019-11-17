#ifndef _UNIT_TEST_HPP_
#define _UNIT_TEST_HPP_

#include <vector>
#include <typeinfo>
#include <cstring>
#include <cstdio>
#include <cmath>

#include "rsc\common\utility.h"

#define UT_OPER_STR_BUF_MAX_LEN 5

#define TEST_NAME(_TEST_NAME) _TEST_NAME##_TEST

#define TEST(_TEST_NAME) \
class TEST_NAME(_TEST_NAME) : public TestCase \
{ \
public: \
	TEST_NAME(_TEST_NAME)(const char *Name_F):TestCase(Name_F) { } \
	virtual void run() override; \
private: \
	static TestCase* const sm_pinstTestCase__; \
}; \
TestCase* const TEST_NAME(_TEST_NAME)::sm_pinstTestCase__ = UnitTest::getInstance().registerTestCase( \
new TEST_NAME(_TEST_NAME)(#_TEST_NAME)); \
void TEST_NAME(_TEST_NAME)::run()

#define RUN_ALL_TESTS(_STR) UnitTest::getInstance().run(_STR);

#define FORMAT_BLACK(...)       "\033[0;30m"##__VA_ARGS__"\033[0m"
#define FORMAT_RED(...)         "\033[0;31m"##__VA_ARGS__"\033[0m"
#define FORMAT_GREEN(...)       "\033[0;32m"##__VA_ARGS__"\033[0m"
#define FORMAT_YELLOW(...)      "\033[0;33m"##__VA_ARGS__"\033[0m"
#define FORMAT_BULE(...)        "\033[0;34m"##__VA_ARGS__"\033[0m"
#define FORMAT_PURPLE(...)      "\033[0;35m"##__VA_ARGS__"\033[0m"
#define FORMAT_DEEP_GREEN(...)  "\033[0;36m"##__VA_ARGS__"\033[0m"
#define FORMAT_WHITE(...)       "\033[0;37m"##__VA_ARGS__"\033[0m"

typedef enum _OPERATOR_TYPE_E
{
    E_OPERATOR_TYPE_EQ,//equal            ==
    E_OPERATOR_TYPE_NE,//not equal        !=
    E_OPERATOR_TYPE_GT,//great than       >
    E_OPERATOR_TYPE_LT,//less than        <
    E_OPERATOR_TYPE_GE,//great or equal   >=
    E_OPERATOR_TYPE_LE,//less or equal    <=

    E_OPERATOR_TYPE_BT//butt
} OPERATOR_TYPE_E;

class TestCase
{
public:
	TestCase(const char *CaseName_F) :
        cm_pcCaseName__(CaseName_F),
        m_bTestResult__(false)
    { };
	virtual ~TestCase()
    { }
    virtual void run() = 0;
    const char* getCaseName() const
    {
        return cm_pcCaseName__;
    }

    bool getTestResult()
    {
        return m_bTestResult__;
    }

    void setTestResult(bool &&result_F)
    {
        m_bTestResult__= result_F;
    }

private:
    const char* cm_pcCaseName__;
    bool        m_bTestResult__;
};

class UnitTest : public Singleton<UnitTest>
{
    friend class Singleton<UnitTest>;
public:
    ~UnitTest()
    {
        for(const auto &it : m_vecTestCases__) {
            delete it;
        }
    }

	TestCase *registerTestCase(TestCase *TestCase_F)
    {
		m_vecTestCases__.emplace_back(TestCase_F);
		return TestCase_F;
	}

	bool run(const char *szInput_F)
    {
		m_bTestResult__ = true;
		printf(FORMAT_YELLOW("            ## Unit Tests ##")"\n");

		m_iAll__ = 0;
		for (const auto &it : m_vecTestCases__) {
            if (szInput_F && !strstr(it->getCaseName(), szInput_F)) {
                continue ;
            }
            printf("***************************************\r\n");
            printf(FORMAT_PURPLE("###[|Start>.....] ")"%s\n", it->getCaseName());
			++m_iAll__;
			m_pinstCurrentTestCase__ = it;
			m_pinstCurrentTestCase__->setTestResult(true);

			printf(FORMAT_BULE("###[...|Run->...] ")"%s\n", it->getCaseName());

			it->run();

            if (it->getTestResult()) {
                ++m_iPassedNum__;
                printf(FORMAT_GREEN("###[......|Pass-] ")"%s\n", it->getCaseName());
            }
            else {
                ++m_iFailedNum__;
                m_bTestResult__ = false;
                printf(FORMAT_RED("###[......|Fail-] ")"%s\n", it->getCaseName());
            }
		}

		printf("\n\033[33m###[ ALL  ] :\033[33;1m%d\033[0m\n", m_iAll__);
		printf("\033[32m###[ PASS ] :\033[32;1m%d\033[0m\n", m_iPassedNum__);
		printf("\033[31m###[ FAIL ] :\033[31;1m%d\033[0m\n", m_iFailedNum__);
        system("pause");
		return !m_bTestResult__;
	}
    TestCase &getCrtTestCase()
    {
        return (*m_pinstCurrentTestCase__);
    }

private:
    UnitTest():
        m_pinstCurrentTestCase__(nullptr),
        m_bTestResult__(false),
        m_iAll__(0),
        m_iPassedNum__(0),
        m_iFailedNum__(0)
    { }
    TestCase               *m_pinstCurrentTestCase__;
    bool                    m_bTestResult__;
    int                     m_iAll__;
    int                     m_iPassedNum__;
    int                     m_iFailedNum__;
    std::vector<TestCase *> m_vecTestCases__;
};

template <class ElemType>
bool checkNumericalData(ElemType Lhs_F, ElemType Rhs_F,
	                    const char *StrLhs_F, const char *strRhs_F,
	                    const char *FileName_F, const unsigned long LineNum_F,
                        OPERATOR_TYPE_E OperatorType_F)
{
    using namespace std;
	bool bIsTure = false;
	char strOperator[UT_OPER_STR_BUF_MAX_LEN] = {0};

	switch (OperatorType_F) {
		case E_OPERATOR_TYPE_EQ:
            if (typeid(ElemType) == typeid(double)) {
                bIsTure = (fabs(Lhs_F - Rhs_F) < 1e-8);
            }
            else if (typeid(ElemType) == typeid(float)) {
                bIsTure = (fabs(Lhs_F - Rhs_F) < 1e-6);
            }
            else {
                bIsTure = (Lhs_F == Rhs_F);
            }
            strcpy_s(strOperator, sizeof(strOperator), " == ");
			break;

		case E_OPERATOR_TYPE_NE:
            if (typeid(ElemType) == typeid(double)) {
                bIsTure = (std::fabs(Lhs_F - Rhs_F) > 1e-8);
            }
            else if (typeid(ElemType) == typeid(float)) {
                bIsTure = (std::fabs(Lhs_F - Rhs_F) > 1e-6);
            }
            else {
                bIsTure = (Lhs_F != Rhs_F);
            }
            strcpy_s(strOperator, sizeof(strOperator), " != ");
			break;

		case E_OPERATOR_TYPE_GT:
			bIsTure = (Lhs_F > Rhs_F);
            strcpy_s(strOperator, sizeof(strOperator), " > ");
			break;

		case E_OPERATOR_TYPE_LT:
			bIsTure = (Lhs_F < Rhs_F);
            strcpy_s(strOperator, sizeof(strOperator), " < ");
			break;

		case E_OPERATOR_TYPE_LE:
			bIsTure = (Lhs_F <= Rhs_F);
			strcpy_s(strOperator, sizeof(strOperator), " <= ");
			break;

		case E_OPERATOR_TYPE_GE:
			bIsTure = (Lhs_F >= Rhs_F);
			strcpy_s(strOperator, sizeof(strOperator), " >= ");
			break;

        default:
            strcpy_s(strOperator, sizeof(strOperator), " ?? ");
            break;
	}

	if (bIsTure == false) {
        UnitTest::getInstance().getCrtTestCase().setTestResult(false);
		printf("\033[36;1m%s\033[0m: \033[31;1m%lu\033[0m: ", FileName_F, LineNum_F);
		printf("\033[33;1mExpect: \033[0m");
        if (typeid(ElemType) == typeid(double)) {
            printf("%s(%f)%s%s(%f)\n",
                   StrLhs_F, (double)Lhs_F, strOperator, strRhs_F, (double)Rhs_F);
        }
        else if (typeid(ElemType) == typeid(float)) {
            printf("%s(%f)%s%s(%f)\n",
                StrLhs_F, (float)Lhs_F, strOperator, strRhs_F, (float)Rhs_F);
        }
        else if ((typeid(ElemType) == typeid(unsigned long long))
                 ||(typeid(ElemType) == typeid(long long))) {
            printf("%s(%lld)%s%s(%lld)\n",
                StrLhs_F, (__int64)Lhs_F, strOperator, strRhs_F, (__int64)Rhs_F);
        }
        else{
            printf("%s(%d)%s%s(%d)\n",
                StrLhs_F, (int)Lhs_F, strOperator, strRhs_F, (int)Rhs_F);
        }
	}
	return bIsTure;
}

bool checkStrData(const char *Lhs_F, const char *Rhs_F,
                  const char* StrLhs_F, const char* StrRhs_F,
                  const char* FileName_F, const unsigned long LineNum_F,
                  OPERATOR_TYPE_E OperatorType_F)
{
	bool bIsTure = false;
	char strOperator[UT_OPER_STR_BUF_MAX_LEN];
    memset(strOperator, 0, sizeof(strOperator));

	if (OperatorType_F == E_OPERATOR_TYPE_EQ) {
		bIsTure = ((strcmp(Lhs_F, Rhs_F) == 0));
		strcpy_s(strOperator, sizeof(strOperator), " == ");
	}
	else if (OperatorType_F == E_OPERATOR_TYPE_NE) {
		bIsTure = ((strcmp(Lhs_F, Rhs_F) != 0));
		strcpy_s(strOperator, sizeof(strOperator), " != ");
	}
    else {
        strcpy_s(strOperator, sizeof(strOperator), " ?? ");
    }

	if (bIsTure == false) {
		UnitTest::getInstance().getCrtTestCase().setTestResult(false);
		printf("\033[36;1m%s\033[0m: \033[31;1m%lu\033[0m: ", FileName_F, LineNum_F);
		printf("\033[33;1mExpect: \033[0m%s%s%s\n", StrLhs_F, strOperator, StrRhs_F);
	}

	return bIsTure;
}

#define CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, _OPER_TYPE) \
checkNumericalData(_LEFT, static_cast<decltype(_LEFT)>(_RIGHT), #_LEFT, #_RIGHT, __FILE__, __LINE__, _OPER_TYPE)
#define CHECK_STR_DATA(_LEFT, _RIGHT, _OPER_TYPE) \
checkStrData(_LEFT, _RIGHT, #_LEFT, #_RIGHT, __FILE__, __LINE__, _OPER_TYPE)

/* expect 只判断是否正确，并且输出, assert 判断条件的结果后返回 */
#define EXPECT_EQ(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_EQ)
#define EXPECT_NE(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_NE)
#define EXPECT_GT(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_GT)
#define EXPECT_LT(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_LT)
#define EXPECT_GE(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_GE)
#define EXPECT_LE(_LEFT, _RIGHT) CHECK_NUMERICAL_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_LE)

#define EXPECT_TRUE(_CONDITION) CHECK_NUMERICAL_DATA(static_cast<bool>(_CONDITION), true, E_OPERATOR_TYPE_EQ)
#define EXPECT_FALSE(_CONDITION) CHECK_NUMERICAL_DATA(static_cast<bool>(_CONDITION), false, E_OPERATOR_TYPE_EQ)
#define EXPECT_STREQ(_LEFT, _RIGHT) CHECK_STR_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_EQ)
#define EXPECT_STRNE(_LEFT, _RIGHT) CHECK_STR_DATA(_LEFT, _RIGHT, E_OPERATOR_TYPE_NE)

#define ASSERT_EQ(_LEFT, _RIGHT) if (!EXPECT_EQ(_LEFT, _RIGHT)) return ;
#define ASSERT_NE(_LEFT, _RIGHT) if (!EXPECT_NE(_LEFT, _RIGHT)) return ;
#define ASSERT_GT(_LEFT, _RIGHT) if (!EXPECT_GT(_LEFT, _RIGHT)) return ;
#define ASSERT_LT(_LEFT, _RIGHT) if (!EXPECT_LT(_LEFT, _RIGHT)) return ;
#define ASSERT_GE(_LEFT, _RIGHT) if (!EXPECT_GE(_LEFT, _RIGHT)) return ;
#define ASSERT_LE(_LEFT, _RIGHT) if (!EXPECT_LE(_LEFT, _RIGHT)) return ;

#define ASSERT_TRUE(_CONDITION) if (!EXPECT_TRUE(_CONDITION)) return ;//是真的话通过，不是就返回
#define ASSERT_FALSE(_CONDITION) if (!EXPECT_FALSE(_CONDITION)) return ;//是假就通过，不是就返回
#define ASSERT_STREQ(_LEFT, _RIGHT) if (!EXPECT_STREQ(_LEFT, _RIGHT)) return ;
#define ASSERT_STRNE(_LEFT, _RIGHT) if (!EXPECT_STRNE(_LEFT, _RIGHT)) return ;

#define ASSERT_PTREQ(_LEFT, _RIGHT) if (!EXPECT_EQ((uint32_t)_LEFT, (uint32_t)_RIGHT)) return ;
#define ASSERT_PTRNE(_LEFT, _RIGHT) if (!EXPECT_NE((uint32_t)_LEFT, (uint32_t)_RIGHT)) return ;

#endif /* _UNIT_TEST_HPP_ */