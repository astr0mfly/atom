#include "error_number.h"

#define MODULE_BITS  15 //模块ID所占位数，该域表示错误发生的模块 
#define ERROR_BITS   16 //每个模块错误码位数，该域表示发生何种错误 
#define ERROR_BIT    (1 << (MODULE_BITS + ERROR_BITS)) //符号位，1表示错误


#if CODE_DESC("class-errorno", CODE_USED)
ErrCode::ErrCode():
    m_bInitialized__(false)
{}

short ErrCode::RegSubModule(AbstractErrCode* subModule_F)
{
    short nModuleIndex = m_vecModules__.size();
    m_vecModules__.emplace_back(module_T(subModule_F->doGetName(), subModule_F));

    ERR_STR_S stTmp;
    stTmp.bAvailable = true;
    m_vecErrstrArray__.emplace_back(stTmp);

    subModule_F->doSetErrStr(&m_vecErrstrArray__[nModuleIndex]);
    return nModuleIndex;
}

inline ERR_T ErrCode::gen_ERRNO(const short& moduleId_F, const short& moduleError_F)
{
    return (__beginOfModule_ERRNO(moduleId_F) | moduleError_F | ERROR_BIT);
}

//最终错误码的绝对值
inline ERR_T ErrCode::genAbs_ERRNO(const short& moduleId_F, const short& moduleError_F)
{
    return (__beginOfModule_ERRNO(moduleId_F) | moduleError_F);
}

const char* ErrCode::getErrorStr_ERRNO(ERR_T errno_F) //转换函数 
{
    unsigned short       module_id = __getModuleId_ERRNO(errno_F);
    unsigned short       error_id = __getModuleError_ERRNO(errno_F);
    if (0 == m_bInitialized__) {
        __init();
    }
    if (0 == errno_F)
        return "SUCCESS";
    if (errno_F > 0)
        return "ERR_ERRSTR_NOT_NEGATIVE";
    if (module_id > m_vecErrstrArray__.size())
        return "ERR_ERRSTR_INVALID_MODULE_ID";
    if (true != m_vecErrstrArray__[module_id].bAvailable)
        return "ERR_ERRSTR_NOT_AVAILABLE";
    if (error_id > m_vecErrstrArray__[module_id].errorLast)
        return "ERR_ERRSTR_OUT_OF_LAST";
    if (0 == m_vecErrstrArray__[module_id].errorArray[error_id])
        return "ERR_ERRSTR_NOT_DEFINED";
    return m_vecErrstrArray__[module_id].errorArray[error_id];
}

void ErrCode::__init()
{
    for (const auto &it : m_vecModules__) {
        it.second->doInitErrorCode();
    }

    m_bInitialized__ = true;
}

inline ERR_T ErrCode::__beginOfModule_ERRNO(const short& moduleId_F)
{
    return (moduleId_F << ERROR_BITS);
}

inline short ErrCode::__getModuleId_ERRNO(const ERR_T& errCode_F)//仅取moduleID
{
    return ((errCode_F & ~ERROR_BIT) >> ERROR_BITS);
}

inline short ErrCode::__getModuleError_ERRNO(const ERR_T& errCode_F)
{
    return (errCode_F & ((1 << ERROR_BITS) - 1));//对errcode的模块部清零，仅取errno
}
#endif  //"class-errno"

#if CODE_DESC("class-ModuleError", CODE_USED)
ModuleError::ModuleError() :
    m_pstErrStr_(nullptr),
    m_nModuleIndex__(0),
    m_pinstErrCode__(nullptr)
{
    m_pinstErrCode__ = ErrCode::getInstance();
    m_pinstErrCode__->RegSubModule(this);
}

ModuleError::ModuleError(ErrCode* ErrnoObj_F) :
    m_pstErrStr_(nullptr),
    m_nModuleIndex__(0),
    m_pinstErrCode__(ErrnoObj_F)
{
    m_pinstErrCode__->RegSubModule(this);
}

int ModuleError::keygen(const short& moduleError_F)
{
    return m_pinstErrCode__->gen_ERRNO(m_nModuleIndex__, moduleError_F);
}

std::string ModuleError::doGetName()
{
    return ERROR_CODE_GET_CLASS_NAME(ModuleError);
}

void ModuleError::doSetErrStr(PERR_STR_S ErrStr_F)
{
    m_pstErrStr_ = ErrStr_F;
}
#endif