#ifndef YSP_COMM_ERROR_NUMBER_H_
#define YSP_COMM_ERROR_NUMBER_H_

#include <string>
#include <vector>
#include <utility>
#include "utility.h"

#define ERROR_CODE_GET_CLASS_NAME(_OBJ) #_OBJ

#define CODE_OK     0
#define CODE_ERR    ~0

#define CHICK_ERROR_STATUS_RETURN(_X)   \
do {                                    \
    if (_X != CODE_OK) return _X;       \
} while (0)                             

typedef int ERR_T; //使用32位带符号整型数表示完整错误代号

typedef struct _ERR_STR_S
{
    bool            bAvailable;
    char            pcRsv[3];
    ERR_T           errorLast;
    const char    **errorArray; //存放各个错误码对应的字符串 
}ERR_STR_S, * PERR_STR_S;

class AbstractErrCode {
public:
    virtual void doInitErrorCode() = 0;
    virtual std::string doGetName() = 0;
    virtual void doSetErrStr(PERR_STR_S ErrStr_F) = 0;
};
/*
 -<<1bit<<-<<15bit<<------<<16bit<<-----
    +----------------+----------------+
    |-|----MODULE----+-----ERRNO------+
    +----------------+----------------+
     ↑
   ErrorBit
*/
class ErrCode : public Singleton<ErrCode>
{
public:
    typedef std::pair<std::string, AbstractErrCode*> module_T;

    ErrCode();
    virtual ~ErrCode() {}

    short RegSubModule(AbstractErrCode* subModule_F);

    //得到最终的错误码
    inline ERR_T gen_ERRNO(const short& moduleId_F, const short& moduleError_F);

    //最终错误码的绝对值
    inline ERR_T genAbs_ERRNO(const short& moduleId_F, const short& moduleError_F);

    const char* getErrorStr_ERRNO(ERR_T errno_F);

    friend class Singleton<ErrCode>;
private:
    void __init();

    inline ERR_T __beginOfModule_ERRNO(const short& moduleId_F);

    inline short __getModuleId_ERRNO(const ERR_T& errCode_F);

    inline short __getModuleError_ERRNO(const ERR_T& errCode_F);

    bool m_bInitialized__;
    std::vector<ERR_STR_S>  m_vecErrstrArray__;
    std::vector<module_T>   m_vecModules__;
};

class ModuleError :public AbstractErrCode
{
public:
    ModuleError();
    explicit ModuleError(ErrCode *ErrnoObj_F);
    virtual ~ModuleError(){}

    int keygen(const short& moduleError_F);

    virtual void doInitErrorCode() override
    { }

    virtual std::string doGetName() override;

    void doSetErrStr(PERR_STR_S ErrStr_F) override;
protected:
    PERR_STR_S m_pstErrStr_;

private:
    short m_nModuleIndex__;
    ErrCode* m_pinstErrCode__;
};

//example 错误码从1开始，0用来表示成功
class DispatcherError :public ModuleError, public Singleton<DispatcherError>
{
public:
    typedef enum _ERRNO_CFG_E
    {
        E_CFG_OK =0,
        E_LOST_MSG=1,
        E_SRV_NOT_FOUND,

        E_CFG_BTM
    }ERRNO_CFG_E;
    DispatcherError()
    {
        memset(m_astrErrorCfg__, 0, sizeof(m_astrErrorCfg__));
    }
    explicit DispatcherError(ErrCode* ErrnoObj_F) :
        ModuleError(ErrnoObj_F)
    {
        DispatcherError::DispatcherError();
    }
    virtual ~DispatcherError(){}

    std::string doGetName() override
    {
        return ERROR_CODE_GET_CLASS_NAME(DispatcherError);
    }

    void addErrStr(short iErrType, const char* szErrDesc)
    {
        m_astrErrorCfg__[iErrType] = szErrDesc;
        m_pstErrStr_->errorLast = iErrType;
    }

    void doInitErrorCode() override
    {
        addErrStr(E_CFG_OK, ERROR_CODE_GET_CLASS_NAME(DispatcherError)":successfully");
        addErrStr(E_LOST_MSG, ERROR_CODE_GET_CLASS_NAME(DispatcherError)":lost-massage");

        m_pstErrStr_->errorArray = m_astrErrorCfg__;
    }

    friend class Singleton<DispatcherError>;
private:
    const char *m_astrErrorCfg__[E_CFG_BTM];
};

#endif //YSP_COMM_ERROR_NUMBER_H_