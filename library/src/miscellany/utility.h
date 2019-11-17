#ifndef YSP_UTILITY_H_
#define YSP_UTILITY_H_

#include <functional>
#include <mutex>
#include <condition_variable>
#include <map>
#include <list>
#include <string>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <typeindex>
#include <type_traits>
#include <exception>
#include <iostream>
#include <vector>
#include <unordered_map>

#ifdef __unix__
#elif _WIN32

#include <WinSock2.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#pragma comment(lib,"Ws2_32.lib")
#endif  //Os-arch

#define ARRAY_SIZE(_A) (sizeof(_A)/sizeof(_A[0]))
#define GET_STR_NAME(_S) #_S
#define PENDL() "\r\n"

#define CODE_DESC(_X, _V) (_V)
#define CODE_USED       1
#define CODE_UNUSED     0

#define NULL_BYTE       0xff
#define NULL_WORD       0xffff
#define NULL_DWORD      0xffffffff


#define FREE_PTR_SET_NULL(_P)     \
do {                                \
    if (_P) {                       \
        free(_P);                  \
        _P = nullptr;               \
    }                               \
} while (__LINE__ == -1)

#define DELETE_PTR_SET_NULL(_P)     \
do {                                \
    if (_P) {                       \
        delete _P;                  \
        _P = nullptr;               \
    }                               \
} while (__LINE__ == -1)

#define SZ(_CON)    int((_CON).size())
#define PB          push_back
#define EB          emplace_back
#define ALL(_C)     (_C).begin(),(_C).end()
#define TRANGE(_C, _I) for(decltype((_C).begin()) _I = (_C).begin();    \
                           _I != (_C).end(); ++_I)
#define PRESENT(_C, _X) ((_C).find(_X) != (_C).end())
#define NPRESENT(_C, _X) ((_C).find(_X) == (_C).end())
#define CPRESENT(_C, _X) (std::find(ALL(_C), _X) != (_C).end)

typedef uint32_t valptr;
typedef uint32_t page_t;

typedef std::function<void()> Task;
typedef std::function<void()> Func;
#if CODE_DESC("InitWinSock", CODE_USED)

#if _WIN32
/* message sent and received */
class InitWinSock
{
public:
    InitWinSock(BYTE byMinorVer=2, BYTE byMajorVer=2)
    {
        WSADATA wsaData;
        int iErrMsg = ::WSAStartup(MAKEWORD(byMinorVer, byMajorVer), &wsaData);		//initialize socket
        if (iErrMsg != NO_ERROR) {
            printf("failed with wsaStartup error:%d ", iErrMsg);
        }
    }

    ~InitWinSock()
    {
        ::WSACleanup();
    }
};
#endif  //Os-arch

#endif  //"InitSock"


#if CODE_DESC("NoCopy", CODE_USED)
class NoCopy
{
protected:
    NoCopy()=default;
    ~NoCopy()=default;
private:
    NoCopy(const NoCopy &)=delete;
    NoCopy &operator=(const NoCopy &)=delete;
};

template<typename T>
class NoCopyTemplate
{
public:
    NoCopyTemplate()=default;
    virtual ~NoCopyTemplate()=default;
private:
    NoCopyTemplate(const NoCopyTemplate &)=delete;
    const NoCopyTemplate &operator=(const NoCopyTemplate &)=delete;
};
#endif  //"NoCopy"

#if CODE_DESC("NoMove", CODE_USED)
class NoMove
{
protected:
    NoMove()=default;
    ~NoMove()=default;
private:
    NoMove(NoMove &&)=delete;
    NoMove &&operator=(NoMove &&)=delete;
};
#endif  //"NoMove"

#if CODE_DESC("Singleton", CODE_USED)
/*
Thread Safe:
If control enters the declaration concurrently 
while the variable is being initialized,
the concurrent execution shall wait for completion of the initialization.
--------------HOW TO USE IT
//--------step0:
#include "singleton.h"

class TestSingle : public Singleton<TestSingle>
{
//--------step1: must to be friend
	friend class Singleton<TestSingle>;
public:
	TestSingle(const TestSingle&)=delete;
	TestSingle& operator= (const TestSingle&)=delete;

private:
	TestSingle()=default;
}

//--------step2: Test&Verify
int foo(int arg, char* argv[])
{
	TestSingle& instanceA = TestSingle::getInstance();
	TestSingle& instanceB = TestSingle::getInstance();
}
*/
template <typename T>
class Singleton : public NoCopy
{
public:
    template<typename... Args_T>
	static T &getInstance(Args_T... Args_F)
	{
        using namespace std;
        call_once(sm_ocFlag, [&] () {
            sm_punObj__ = move(unique_ptr<T>(new (nothrow) T(forward<Args_T>(Args_F)...)));
        });

		return (*(sm_punObj__.get()));
	}

    static T &getInstance()
    {
        using namespace std;
        call_once(sm_ocFlag, [&] () {
            sm_punObj__ = move(unique_ptr<T>(new (nothrow) T()));
        });

        return (*(sm_punObj__.get()));
    }

protected:
	Singleton()=default;
    virtual ~Singleton()=default;

private:
    static std::once_flag sm_ocFlag;
    static std::unique_ptr<T> sm_punObj__;
};

template <typename T>
std::once_flag Singleton<T>::sm_ocFlag;

template <typename T>
std::unique_ptr<T> Singleton<T>::sm_punObj__;

#endif  //"Singleton"

#if CODE_DESC("ScopeGuard", CODE_USED)
/*
------------------HOW TO USE IT
一种是用ON_SCOPE_EXIT宏生成的，不需要dismiss，好处是自动生成唯一对象名。
但是也让这个方法不能从外部调用
Acquire Resource1
ON_SCOPE_EXIT([&] {  Release Resource1 })


另一种是自己生成ScopeGuard对象，这样容易调用dismiss方法；
ScopeGuard onFailureRollback([&] {  rollback  });
... // do something that could fail
onFailureRollback.Dismiss();

*/
#define SCOPEGUARD_LINENAME_CAT(ProductName_F, line) ProductName_F##line
#define SCOPEGUARD_LINENAME(ProductName_F, line) SCOPEGUARD_LINENAME_CAT(ProductName_F, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
#define ON_SCOPE_EXIT_POINTER(_pointer) ON_SCOPE_EXIT([&_pointer]{ delete _pointer;})

class ScopeGuard : public NoCopy
{
public:
    using Func_T = std::function<void()>;
    ScopeGuard(Func_T &&ExitScope_F)
        : m_funcExit__(std::move(ExitScope_F))
        , m_bDissmissed__(false)
    { }

    explicit ScopeGuard(const Func_T &ExitScope_F)
        : m_funcExit__(ExitScope_F)
        , m_bDissmissed__(false)
    { }

    ScopeGuard(ScopeGuard &&Rhs_F)
        : m_funcExit__(std::move(Rhs_F.m_funcExit__))
        , m_bDissmissed__(Rhs_F.m_bDissmissed__)
    {
        Rhs_F.dismiss();
    }

    ~ScopeGuard()
    {
        if (!m_bDissmissed__ && (m_funcExit__ != nullptr)) {
            m_funcExit__();
        }
    }

    void dismiss()
    {
        m_bDissmissed__ = true;
    }

private:
    Func_T      m_funcExit__;
    bool        m_bDissmissed__;
};

//添加默认模板参数，使用时要加尖括号
template<typename F = std::function<void()> >
class ScopeGuardTemplate : public NoCopy
{
public:
    ScopeGuardTemplate(F &&ExitScope_F)
        : m_funcExit__(std::move(ExitScope_F))
        , m_bDissmissed__(false)
    { }

    explicit ScopeGuardTemplate(const F &ExitScope_F)
        : m_funcExit__(ExitScope_F)
        , m_bDissmissed__(false)
    { }

    ScopeGuardTemplate(ScopeGuardTemplate &&Rhs_F)
        : m_funcExit__(std::move(Rhs_F.m_funcExit__))
        , m_bDissmissed__(Rhs_F.m_bDissmissed__)
    {
        Rhs_F.dismiss();
    }

    ~ScopeGuardTemplate()
    {
        if (!m_bDissmissed__ && (m_funcExit__ != nullptr)) {
            m_funcExit__();
        }
    }

    void dismiss()
    {
        m_bDissmissed__ = true;
    }

private:
    F           m_funcExit__;
    bool        m_bDissmissed__;
};

template <typename F>
ScopeGuardTemplate<typename std::decay<F>::type> make_SGuard(F &&Func_F)
{
    return ScopeGuardTemplate<typename std::decay<F>::type>(std::forward<F>(Func_F));
}

#endif  //"ScopeGuard"

#if CODE_DESC("Factory", CODE_USED)

/*
    分享类，可支持二段式构造。doInvoke函数需要支持可重入。
    此对象可以加入对象池，或者享元工厂
*/
class ObjShared
{
public:
    ObjShared()=default;
    virtual ~ObjShared()=default;
    virtual int doInvoke(int ArgsNum_F, va_list Args_F)=0;
};

/*
    享元工厂类，可以保证类的生存期，输出对应对象的shared_ptr，自动析构。
    外界可以输入对应的状态，直接重置对象。
    内部使用hash_table来加速查找，再加上无需申请类空间
    性能比较好。
*/
template <class ObjShared>
class FlyFactory : public Singleton<FlyFactory<ObjShared> >
{
    friend class Singleton<FlyFactory>;
public:
    using FlyPtr_T = std::shared_ptr<ObjShared>;
    /*
        享元工厂，如果存在重复的对象就复用之前已经存在的。
    */
    template<class ObjApp_T, typename... Args_T>
    FlyPtr_T getFlyWeight(const std::string &ObjKey_F, Args_T ...Args_F)
    {
        using namespace std;

        if (NPRESENT(m_umapFlyObjs__, ObjKey_F)) {
            m_umapFlyObjs__[ObjKey_F] = make_shared<ObjApp_T>(forward<Args_T>(Args_F)...);
        }

        return m_umapFlyObjs__[ObjKey_F];
    }

private:
    using FlyObjs_T = std::unordered_map<std::string, std::shared_ptr<ObjShared> >;

    FlyFactory()=default;
    FlyObjs_T m_umapFlyObjs__;
};

// 基类，产品注册模板接口类
// 模板参数 ObjType_T 表示的类是产品抽象类
template <class ObjType_T>
class ItfObjRegistrar : public NoCopy   //禁止外部构造和虚构
{
public:
    using ObjPtr_T = std::shared_ptr<ObjType_T>;
    virtual ~ItfObjRegistrar()=default;
    virtual ObjPtr_T doCreateObj(int, ...) noexcept =0;
protected:
    // 子类的"内部"的其他函数可以调用
    ItfObjRegistrar()=default;
};

// 工厂模板类，用于获取和注册产品对象，每一次获取的对象都是新的
// 模板参数 ObjType_T 表示的类是产品抽象类
template <class ObjType_T>
class ObjFactory : public Singleton<ObjFactory<ObjType_T> >
{
    friend class Singleton<ObjFactory<ObjType_T> >;
public:
    using ObjPtr_T = std::shared_ptr<ObjType_T>;

    virtual ~ObjFactory()
    {
        using namespace std;
        lock_guard<mutex> lg(m_mtxTbl__);
        if (!m_ProductRegistry__.empty()) {
            for (auto &it : m_ProductRegistry__) {
                delete it.second;
            }
        }
    }
    
    // 产品注册
    void regObj(ItfObjRegistrar<ObjType_T> *Registrar_F, std::string ProductName_F)
    {
        using namespace std;
        lock_guard<mutex> lg(m_mtxTbl__);
        m_ProductRegistry__[ProductName_F] = Registrar_F;
    }

    // 产品注销
    void cclObj(std::string ProductName_F)
    {
        using namespace std;
        lock_guard<mutex> lg(m_mtxTbl__);
        m_ProductRegistry__.erase(ProductName_F);
    }

    // 根据名字name，获取对应具体的产品对象
    template<typename... Args_T>
    ObjPtr_T newProduct(std::string &Name_F, Args_T... Args_F)
    {
        using namespace std;
        if (Name_F.empty()) {
            return nullptr;
        }
        int iArgsNum = sizeof...(Args_F);
        // 从map找到已经注册过的产品，并返回产品对象
        lock_guard<mutex> lg(m_mtxTbl__);
        if (PRESENT(m_ProductRegistry__, Name_F)) {
            return m_ProductRegistry__[Name_F]->doCreateObj(
                       iArgsNum, forward<Args_T>(Args_F)...);
        }

        return nullptr;
    }

private:
    ObjFactory()=default;

    // 保存注册过的产品，key:产品名字 , value:产品类型
    std::map<std::string, ItfObjRegistrar<ObjType_T> *> m_ProductRegistry__;
    std::mutex  m_mtxTbl__;
};

// 产品注册模板类，用于创建具体产品和从工厂里注册产品
// 模板参数 ObjType_T 表示的类是产品抽象类（基类），ProductImpl_t 表示的类是具体产品（产品种类的子类）
template<class ObjType_T, class ObjImpl_T>
class ObjRegistrar : public ItfObjRegistrar<ObjType_T>
{
public:
    using ObjPtr_T = std::shared_ptr<ObjType_T>;

    ObjRegistrar(std::string &&Name_F)
    {
        // 通过工厂单例把产品注册到工厂
        ObjFactory<ObjType_T>::getInstance().regObj(this, Name_F);
        m_strName_.swap(Name_F);
    }

    // 构造函数，用于注册产品到工厂，只能显示调用
    explicit ObjRegistrar(const std::string &Name_F)
    {
        // 通过工厂单例把产品注册到工厂
        ObjFactory<ObjType_T>::getInstance().regObj(this, Name_F);
        m_strName_ = Name_F;
    }

    virtual ~ObjRegistrar()
    {
        ObjFactory<ObjType_T>::getInstance().cclObj(m_strName_);
    }

    ObjPtr_T doCreateObj(int ArgsNum_F, ...) noexcept override 
    {
        va_list args;
        va_start(args, ArgsNum_F);

        ObjPtr_T pinst(new (std::nothrow) ObjImpl_T());
        try {
            pinst->doInvoke(ArgsNum_F, args);
        }
        catch (...) {
            va_end(args);
            return nullptr;
        }

        va_end(args);
        return pinst;
    }

protected:
    std::string m_strName_;
};

//享元工厂类的注册机，想要被共享的类以此自动完成注册注销。
template<class ObjType_T, class ObjImpl_T>
class FlyRegistrar : public ObjRegistrar<ObjType_T, ObjImpl_T>
{
public:
    FlyRegistrar(std::string &&Name_F)
        : ObjRegistrar(std::forward<std::string>(Name_F))
    { }
    
    explicit FlyRegistrar(const std::string &Name_F)
        : ObjRegistrar(Name_F)
    { }

    virtual ~FlyRegistrar()=default;

    ObjPtr_T doCreateObj(int ArgsNum_F, ...) noexcept override
    {
        va_list args;
        va_start(args, ArgsNum_F);
        
        auto &&ptrFly = FlyFactory<ObjType_T>::getInstance().getFlyWeight<ObjImpl_T>(m_strName_);
        try {
            ptrFly->doInvoke(ArgsNum_F, args);
        }
        catch (...) {
            va_end(args);
            return nullptr;
        }
        
        va_end(args);
        return std::move(ptrFly);
    }
};

#endif  //"Factory"

#if CODE_DESC("ProAndCon", CODE_USED)

//生产者消费者队列，如果队列已满，新的请求会同步阻塞
template<class T>
class ProAndCon
{
public:
    ProAndCon(int maxSize)
    {
        m_iMaxSize__ = maxSize;
    }
    ProAndCon()
        : m_iMaxSize__(16)
    { }
    virtual ~ProAndCon()=default;
    void product(const T &v)
    {
        using namespace std;
        lock_guard<mutex> locker(m_mtxList__);
        while (isFull()) {
            m_cvNotFull__.wait(m_mtxList__);
        } //往队列里面生产一个元素,同时通知不为空这个信号量
        m_quElements__.push_back(v);
        m_cvNotEmpty__.notify_one();
    }

    void consumption(T &v)
    {
        using namespace std;
        lock_guard<mutex> locker(m_mtxList__);
        while (isEmpty()) {
            m_cvNotEmpty__.wait(m_mtxList__);
        } //在队列里面消费一个元素,同时通知队列不满这个信号量
        v = m_quElements__.front();
        m_quElements__.pop_front();
        m_cvNotFull__.notify_one();
    }

private:
    bool isEmpty() const //队列为空
    {
        return m_quElements__.empty();
    }
    bool isFull() const //队列已满
    {
        return SZ(m_quElements__) == m_iMaxSize__;
    }

    int                          m_iMaxSize__;//队列最大容量
    std::list<T>                 m_quElements__;
    std::mutex                   m_mtxList__;//全局互斥锁
    std::condition_variable_any  m_cvNotEmpty__;//全局条件变量（不为空）
    std::condition_variable_any  m_cvNotFull__;//全局条件变量（不为满）
};
#endif  //"ProAndCon"

void printMemory(const char *buf, int len);

#if CODE_DESC("mutable data type", CODE_USED)

//any 类型，是一个特殊的只能容纳一个元素的容器,他可以擦除类型,给任何类型赋值。
typedef struct _ANY_S
{
    _ANY_S(void)
        : m_tpIndex__(std::type_index(typeid(void)))
    { }

    explicit _ANY_S(const _ANY_S &that)
        : m_ptr__(that.__clone())
        , m_tpIndex__(that.m_tpIndex__)
    { }

    _ANY_S(_ANY_S &&that)
        : m_ptr__(std::move(that.m_ptr__))
        , m_tpIndex__(that.m_tpIndex__)
    { }

    //创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
    //std::enable_if 是限定第二个模板参数必须是ANY_S
    template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, _ANY_S>::value, U>::type> _ANY_S(U &&Value_F)
        : m_ptr__(new Derived < typename std::decay<U>::type>(std::forward<U>(Value_F)))
        , m_tpIndex__(std::type_index(typeid(typename std::decay<U>::type)))
    { }

    bool isNull() const
    {
        return !bool(m_ptr__);
    }

    template<class U>
    bool is() const
    {
        return m_tpIndex__ == std::type_index(typeid(U));
    }

    //将Any转换为实际的类型
    template<class U>
    U &anyCast()
    {
        if (!is<U>()) {
            std::cout << "can not cast " << typeid(U).name()
                      << " to " << m_tpIndex__.name() << std::endl;
            throw std::logic_error{"bad cast"};
        }

        auto derived = dynamic_cast<Derived<U> *>(m_ptr__.get());

        return derived->instValue;
    }

    _ANY_S &operator=(const _ANY_S &a)
    {
        if (m_ptr__ == a.m_ptr__) {
            return *this;
        }

        m_ptr__ = a.__clone();
        m_tpIndex__ = a.m_tpIndex__;

        return *this;
    }

    _ANY_S &operator=(_ANY_S &&a)
    {
        if (m_ptr__ == a.m_ptr__) {
            return *this;
        }

        m_ptr__ = std::move(a.m_ptr__);
        m_tpIndex__ = a.m_tpIndex__;

        return *this;
    }

private:
    struct Base;
    using BasePtr = std::unique_ptr<Base>;

    struct Base//接口类
    {
        virtual ~Base()=default;
        virtual BasePtr doClone() const = 0;
    };

    template<typename T>
    struct Derived : public Base//派生类，使用智能指针管理
    {
        template<typename U>
        Derived(U &&value)
            : instValue(std::forward<U>(value))
        { }

        BasePtr doClone() const override
        {
            return BasePtr(new Derived<T>(instValue));
        }

        T instValue;
    };

    BasePtr __clone() const//对象转移时，本质是交接智能指针。
    {
        if (m_ptr__ != nullptr) {
            return m_ptr__->doClone();
        }

        return nullptr;
    }

    BasePtr         m_ptr__;
    std::type_index m_tpIndex__;
}ANY_S, *PANY_S;

/** 获取最大的整数 */
//声明
template <size_t arg, size_t... rest>
struct IntegerMax;

//继承萃取机制，说明此变量是一个编译期常量
template <size_t arg>
struct IntegerMax<arg> : std::integral_constant<size_t, arg>
{

};

//继承萃取机制，进行比较，取最大值，说明此变量是一个编译期常量
template <size_t arg1, size_t arg2, size_t... rest>
struct IntegerMax<arg1, arg2, rest...>
    : std::integral_constant<size_t, arg1 >= arg2
                                        ? IntegerMax<arg1, rest...>::value
                                        : IntegerMax<arg2, rest...>::value>
{

};

/** 获取最大的align，字节对齐数 */
template<typename... Args>
struct MaxAlign
    : std::integral_constant<int
        , IntegerMax<std::alignment_of<Args>::value...>::value>
{

};

/** 是否包含某个类型 */
template <typename T, typename... List>
struct Contains;

//std::conditional，它在编译期根据一个判断式选择两个类型中的一个，和条件表达式的语义类似，类似于一个三元表达式
template <typename T, typename Head, typename... Rest>
struct Contains<T, Head, Rest...>
    : std::conditional<std::is_same<T, Head>::value
                       , std::true_type
                       , Contains<T, Rest... > >::type
{

};

template <typename T>
struct Contains<T> : std::false_type
{

};

//为类型设置索引，从0开始，往后+1
template <typename T, typename... List>
struct IndexOf;

template <typename T, typename Head, typename... Rest>
struct IndexOf<T, Head, Rest...>
{
    enum
    {
        value = IndexOf<T, Rest...>::value + 1
    };
};

template <typename T, typename... Rest>
struct IndexOf<T, T, Rest...>
{
    enum
    {
        value = 0
    };
};

template <typename T>
struct IndexOf<T>
{
    enum
    {
        value = -1
    };

};


//查找索引时的type.
template<int index, typename... Types>
struct At;

template<int index, typename First, typename... Types>
struct At<index, First, Types...>
{
    using type = typename At<index - 1, Types...>::type;
};

template<typename T, typename... Types>
struct At<0, T, Types...>
{
    using type = T;

};


//variant来作为union的类型安全替代品。它可以在任意时间保存模板参数列表中某一类型的值或者空值。
template<typename... Types>
class Variant
{
    enum
    {
        data_size = IntegerMax<sizeof(Types)...>::value,
        align_size = MaxAlign<Types...>::value
    };
    //内部变量的存储采用字节对齐，保证读写速度。
    using Data_T = typename std::aligned_storage<data_size, align_size>::type;

public:

    template<int index>
    using IndexType = typename At<index, Types...>::type;

    Variant(void)
        : m_tpIndex__(typeid(void))
    { }

    Variant(Variant<Types...> &&old)
        : m_tpIndex__(old.m_tpIndex__)
    {
        this->__move(old.m_tpIndex__, &old.m_data__, &m_data__);
    }

    Variant(const Variant<Types...> &old)
        : m_tpIndex__(old.m_tpIndex__)
    {
        this->__copy(old.m_tpIndex__, &old.m_data__, &m_data__);
    }

    Variant &operator=(Variant &&old)
    {
        this->__move(old.m_tpIndex__, &old.m_data__, &m_data__);
        m_tpIndex__ = old.m_tpIndex__;

        return *this;
    }

    Variant &operator=(const Variant &old)
    {
        this->__copy(old.m_tpIndex__, &old.m_data__, &m_data__);
        m_tpIndex__ = old.m_tpIndex__;

        return *this;
    }

    template <class T
        , class = typename std::enable_if<Contains<typename std::decay<T>::type
                                                   , Types...>::value>::type>
    Variant(T &&Value_F)
        : m_tpIndex__(typeid(void))
    {
        typedef typename std::decay<T>::type U;

        __destroy(m_tpIndex__, &m_data__);

        new(&m_data__) U(std::forward<T>(Value_F));

        m_tpIndex__ = std::type_index(typeid(U));
    }

    bool operator==(const Variant &Rhs_F) const
    {
        return m_tpIndex__ == Rhs_F.m_tpIndex__;

    }

    bool operator<(const Variant &Rhs_F) const
    {
        return m_tpIndex__ < Rhs_F.m_tpIndex__;
    }

    ~Variant()
    {
        __destroy(m_tpIndex__, &m_data__);
    }

    //对比参数类型
    template<typename T>
    bool is() const
    {
        return (m_tpIndex__ == std::type_index(typeid(T)));
    }

    //参数包是否为空
    bool empty() const
    {
        return m_tpIndex__ == std::type_index(typeid(void));
    }

    std::type_index type() const
    {
        return m_tpIndex__;//调用拷贝构造，实际上是指针传递。
    }

    template<typename T>
    typename std::decay<T>::type &get()
    {
        using U = typename std::decay<T>::type;

        if (!is<U>()) {

            std::cout << typeid(U).name() << " is not defined. "
                      << "current type is " << m_tpIndex__.name() << std::endl;

            throw std::bad_cast{};
        }

        return *(U *)(&m_data__);//强转并返回引用
    }

    template<typename T>
    int indexOf() const//通过比对类型包查找索引
    {
        return IndexOf<T, Types...>::value;
    }

private:
    void __destroy(const std::type_index &index, void *buf)
    {   //下面是lamba表达式递归迭代，依次析构，
        [this](Types &&...) { }((__destroy0<Types>(index, buf), 0)...);
    }

    template<typename T>
    void __destroy0(const std::type_index &id, void *data)
    {
        if (id == std::type_index(typeid(T))) {
            reinterpret_cast<T *>(data)->~T();
        }
    }

    void __move(const std::type_index &old_t, void *old_v, void *new_v)
    {
        [this](Types &&...) { }((__move0<Types>(old_t, old_v, new_v), 0)...);
    }

    template<typename T>
    void __move0(const std::type_index &old_t, void *old_v, void *new_v)
    {
        if (old_t == std::type_index(typeid(T))) {
            new (new_v)T(std::move(*reinterpret_cast<T *>(old_v)));
        }
    }

    void __copy(const std::type_index &old_t, const void *old_v, void *new_v)
    {
        [this](Types &&...) { }((__copy0<Types>(old_t, old_v, new_v), 0)...);
    }

    template<typename T>
    void __copy0(const std::type_index &old_t, const void *old_v, void *new_v)
    {

        if (old_t == std::type_index(typeid(T))) {
            new (new_v)T(*reinterpret_cast<const T *>(old_v));
        }
    }

private:
    Data_T          m_data__;
    std::type_index m_tpIndex__;
};

//有时我们会用一个值来表示一种“没有什么意义”的状态，
template<typename T>
class Optional
{
    using data_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;
public:
    Optional()
        : m_bHasInit__(false)
    { }

    Optional(const T &Value_F)
    {
        __create(Value_F);
    }

    Optional(T &&Value_F)
        : m_bHasInit__(false)
    {
        __create(std::move(Value_F));
    }

    Optional(Optional &&other)
        : m_bHasInit__(false)
    {
        if (other.isInit()) {
            __assign(std::move(other));
            other.__destroy();
        }
    }
    
    Optional(const Optional &other)
        : m_bHasInit__(false)
    {
        if (other.isInit()) {
            __assign(other);
        }
    }

    Optional &operator =(Optional &&other)
    {
        __assign(std::move(other));
        return *this;
    }

    Optional &operator =(const Optional &other)
    {
        __assign(other);
        return *this;
    }

    explicit operator bool() const //逻辑的隐式转换
    {
        return isInit();
    }

    T &operator *()
    {
        return *((T *)(&m_data__));
    }

    T const &operator *() const
    {
        if (isInit()) {
            return *((T *)(&m_data__));
        }

        throw std::exception("");
    }

    bool operator == (const Optional<T> &rhs) const
    {
        return (!bool(*this)) != (!rhs) ? false : (!bool(*this) ? true : (*(*this)) == (*rhs));
    }

    bool operator < (const Optional<T> &rhs) const
    {
        return !rhs ? false : (!bool(*this) ? true : (*(*this) < (*rhs)));
    }

    bool operator != (const Optional<T> &rhs)
    {
        return !(*this == (rhs));
    }

    ~Optional()
    {
        __destroy();
    }

    template<class... Args>
    void emplace(Args &&... args)
    {
        __destroy();
        __create(std::forward<Args>(args)...);
    }

    bool isInit() const
    {
        return m_bHasInit__;
    }

private:
    template<class... Args>
    void __create(Args &&... args)
    {
        new (&m_data__) T(std::forward<Args>(args)...);
        m_bHasInit__ = true;
    }

    void __destroy()
    {
        if (m_bHasInit__) {
            m_bHasInit__ = false;
            ((T *)(&m_data__))->~T();
        }
    }

    void __assign(const Optional &other)
    {
        if (other.isInit()) {
            __copy(other.m_data__);
            m_bHasInit__ = true;
        }
        else {
            __destroy();
        }
    }

    void __assign(Optional &&other)
    {
        if (other.isInit()) {
            __move(std::move(other.m_data__));
            m_bHasInit__ = true;
            other.__destroy();
        }
        else {
            __destroy();
        }
    }

    void __move(data_t &&val)
    {
        __destroy();
        new (&m_data__) T(std::move(*((T *)
            (&val))));
    }

    void __copy(const data_t &val)
    {
        __destroy();
        new (&m_data__) T(*((T *)(&val)));
    }

    bool        m_bHasInit__;
    data_t      m_data__;
};

#endif //mutable data type


#if CODE_DESC("ioc aop", CODE_USED)
//控制反转-依赖注入
class IocContainer : public NoCopy
{
public:
    IocContainer(void)
    { }
    ~IocContainer(void)
    { }

    template<class T, typename Depend, typename... Args>
    void regType(const std::string &strKey)
    {
        std::function<T *(Args...)> funcProc = [](Args... args) {
            return new T(new Depend(args...));
        };//通过闭包擦除了参数类型

        regType(strKey, funcProc);
    }

    template<class T, typename... Args>
    T *resolve(const std::string &strKey, Args... args)
    {
        if (m_umapCreator__.find(strKey) == m_umapCreator__.end()) {
            return nullptr;
        }

        ANY_S resolver = m_umapCreator__[strKey];

        std::function<T *(Args...)> funcProc = resolver.anyCast<std::function<T * (Args...)> >();

        return funcProc(args...);
    }

    template<class T, typename... Args>
    std::shared_ptr<T> resolveShared(const std::string &strKey, Args... args)
    {
        T *t = resolve<T>(strKey, args...);
        return std::shared_ptr<T>(t);
    }

private:
    void regType(const std::string &strKey, ANY_S constructor)
    {
        if (m_umapCreator__.find(strKey) != m_umapCreator__.end()) {
            throw std::invalid_argument("this key has already exist!");
        }

        //通过Any擦除了不同类型的构造器
        m_umapCreator__.emplace(strKey, constructor);
    }

private:
    std::unordered_map<std::string, ANY_S> m_umapCreator__;
};

/*

    IocContainer ioc;
    ioc.regType<A, DerivedC>(“C”);      //配置依赖关系
    auto c = ioc.resolveShared<A>(“C”);

    ioc.RegType<A, DerivedB, int, double>(“C”);   //注册时要注意DerivedB的参数int和double
    auto b = ioc.ResolveShared<A>(“C”, 1, 2.0); //还要传入参数
    b->Func();
*/

//面向切片编程，后续添加时间计数对象和log对象。可能还有错误码对象。
struct Aspect : public NoCopy
{
    template<typename Func>
    explicit Aspect(const Func &f)
        : m_func(f)
    { }

    template<typename T>
    void invoke(T &&value)
    {
        value.before();
        m_func();
        value.after();
    }

    template<typename Head, typename... Tail>
    void invoke(Head &&head, Tail &&... tail)
    {
        head.before();
        invoke(std::forward<Tail>(tail)...);
        head.after();
    }

private:
    std::function<void()> m_func;
};

template<typename... AP>
void invoke(const std::function<void()> &f)
{
    Aspect msp(f);
    msp.invoke(AP()...);
}

/*
struct AA
{
    void before()
    {
        cout<<"before from AA"<<endl;
    }

    void after()
    {
        cout<<"after from AA"<<endl;
    }
};

struct BB
{
    void before()
    {
        cout<<"before from BB"<<endl;
    }

    void after()
    {
        cout<<"after from BB"<<endl;
    }
};

struct CC
{
    void before()
    {
        cout<<"before from CC"<<endl;
    }

    void after()
    {
        cout<<"after from CC"<<endl;
    }
};

struct TT
{
    void g()
    {
        cout<<"real g function"<<endl;
    }

    void h(int a)
    {
        cout<<"real h function: "<<a<<endl;
    }
};

struct DD
{
    void before()
    {

    }

    void after()
    {

    }
};

void GT()
{
    cout<<"real GT function"<<endl;
}

void HT(int a)
{
    cout<<"real HT function: "<<a<<endl;
}

void TestAOP()
{
    TT tt;
    std::function<void()> ff = std::bind(&TT::g, &tt);
        //组合了两个切面AA BB
    Invoke<AA,BB>([&ff](){ff();}); //织入成员函数
    Invoke<AA,BB>([&tt](){tt.g();}); //织入对象

    int aa = 3;
    Invoke<AA,BB>(&GT); //织入方法
    Invoke<AA,BB>([aa](){HT(aa);});//织入带参的方法

    //织入带参数的成员函数和对象
    std::function<void(int)> ff1 = std::bind(&TT::h, &tt, std::placeholders::_1);
    Invoke<AA,BB,CC,DD>([&ff1,aa](){ff1(aa);}); //组合了四个切面
    Invoke<AA,BB>([&tt,aa](){tt.h(aa);});
}

*/

#endif //ioc aop

#if CODE_DESC("lexical_cast", CODE_USED)

namespace lexical
{
    extern const char *cg_strTrue;
    extern const char *cg_strFalse;

    template <typename To, typename From>
    struct Converter
    {
    };

    //to bool, 把from的非整数类型不转换
    template <typename From>
    struct Converter<bool, From>
    {
        static typename std::enable_if<std::is_integral<From>::value, bool>::type convert(From From_F)
        {
            return !!From_F;
        }
    };

#define CONVERTER_MACRO(_TYPE, _FUNC)                   \
    template <typename From>                            \
    struct Converter<_TYPE, From>                       \
    {                                                   \
        static _TYPE convert(std::string &&From_F)      \
        {                                               \
            return std::_FUNC(From_F.c_str());          \
        }                                               \
                                                        \
        static _TYPE convert(const std::string &From_F) \
        {                                               \
            return std::_FUNC(From_F.c_str());          \
        }                                               \
                                                        \
        static _TYPE convert(const char *From_F)        \
        {                                               \
            return std::_FUNC(From_F);                  \
        }                                               \
    }

    //to numeric, 偏特化
    CONVERTER_MACRO(int, atoi);
    CONVERTER_MACRO(long, atol);
    CONVERTER_MACRO(long long, atoll);
    CONVERTER_MACRO(float, atof);
    CONVERTER_MACRO(double, atof);
    CONVERTER_MACRO(unsigned long, stoul);
    CONVERTER_MACRO(unsigned long long, stoull);

#undef CONVERTER_MACRO

    inline bool checkbool(const char *From_F, const size_t len, const char *s)
    {
        for (size_t i = 0; i < len; i++) {
            if (From_F[i] != s[i]) {
                return false;
            }
        }
        return true;
    }

    static bool convert(const char *From_F)
    {
        const unsigned int len = strlen(From_F);
        if (len != 4 && len != 5) {
            throw std::invalid_argument("argument is invalid");
        }

        if ((len == 4) && checkbool(From_F, len, cg_strTrue)) {
            return true;
        }
        else {
            if (checkbool(From_F, len, cg_strFalse)) {
                return false;
            }
        }

        throw std::invalid_argument("argument is invalid");
    }

    template <>
    struct Converter<bool, std::string>
    {
        static bool convert(std::string &&From_F)
        {
            return lexical::convert(From_F.c_str());
        }

        static bool convert(const std::string &From_F)
        {
            return lexical::convert(From_F.c_str());
        }
    };

    template <>
    struct Converter<bool, const char *>
    {
        static bool convert(const char *From_F)
        {
            return lexical::convert(From_F);
        }
    };

    template <>
    struct Converter<bool, char *>
    {
        static bool convert(char *From_F)
        {
            return lexical::convert(From_F);
        }
    };

    template <unsigned N>
    struct Converter<bool, const char[N]>
    {
        static bool convert(const char(&From_F)[N])
        {
            return lexical::convert(From_F);
        }
    };

    template <unsigned N>
    struct Converter<bool, char[N]>
    {
        static bool convert(const char(&From_F)[N])
        {
            return lexical::convert(From_F);
        }
    };

    //to string
    template <typename From>
    struct Converter<std::string, From>
    {
        static std::string convert(const From &From_F)
        {
            return std::move(std::to_string(From_F));
        }
    };

}   //end lexical

//限制，To和From不一样就进行转换
template <typename To, typename From = std::string>
typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(From &&From_F)
{
    return lexical::Converter<To, From>::convert(std::forward<From>(From_F));
}

template <typename To, typename From = std::string>
typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(const From &From_F)
{
    return lexical::Converter<To, From>::convert(From_F);
}

//如果数据类型一致，则不转换。
template <typename To, typename From = std::string>
typename std::enable_if<std::is_same<To, From>::value, To>::type lexical_cast(const From &From_F)
{
    return From_F;
}

#endif  //"lexical_cast"

#endif  //YSP_UTILITY_H_