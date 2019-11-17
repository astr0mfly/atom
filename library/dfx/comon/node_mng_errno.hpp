#ifndef _NODE_MANAGER_ERROR_CODE_H_
#define _NODE_MANAGER_ERROR_CODE_H_
#include "error_number.h"
/*
step1: add enum ERRNO_CFG_E
step2: addErrStr in doInitErrorCode

*/

#define NODE_MNG_RECORD_ERROR(_ENUM) NodeMngError::getInstance()->keygen(NodeMngError::_ENUM)
#define NODE_MNG_ADD_ERR_STR(_CODE, _STR) addErrStr(_CODE, ERROR_CODE_GET_CLASS_NAME(NodeMngError)": "##_STR)
class NodeMngError :public ModuleError, public Singleton<NodeMngError>
{
public:
    typedef enum _ERRNO_CFG_E
    {
        E_CFG_OK=0,
        E_ERROR_CMD=1,
        E_LOST_MSG=2,
        E_TIME_OUT,
        E_MEMORY_ERR,
        E_NOT_FOUND,
        E_OUT_OF_RANGE,
        E_ROOT_REPEAT,
        E_NODE_UNEXPECT_STATE,
        E_SHM_CANT_USED,
        E_NOT_LEADER,

        E_CFG_BTM
    }ERRNO_CFG_E;
    NodeMngError()
    {
        memset(m_astrErrorCfg__, 0, sizeof(m_astrErrorCfg__));
    }
    explicit NodeMngError(ErrCode* ErrnoObj_F) :
        ModuleError(ErrnoObj_F)        
    {
        NodeMngError::NodeMngError();
    }
    virtual ~NodeMngError() {}

    std::string doGetName() override
    {
        return ERROR_CODE_GET_CLASS_NAME(NodeMngError);
    }

    void addErrStr(short iErrType, const char* szErrDesc)
    {
        m_astrErrorCfg__[iErrType] = szErrDesc;
        m_pstErrStr_->errorLast = iErrType;
    }

    void doInitErrorCode() override
    {
        NODE_MNG_ADD_ERR_STR(E_CFG_OK, "successfully");
        NODE_MNG_ADD_ERR_STR(E_LOST_MSG, "lost-massage");
        NODE_MNG_ADD_ERR_STR(E_ROOT_REPEAT, "root was already exist");
        NODE_MNG_ADD_ERR_STR(E_NODE_UNEXPECT_STATE, "The node is in an unpredictable state");
        NODE_MNG_ADD_ERR_STR(E_SHM_CANT_USED, "shm_name was wrong or shm was't exist");
        NODE_MNG_ADD_ERR_STR(E_OUT_OF_RANGE, "illegal argument");
        NODE_MNG_ADD_ERR_STR(E_ERROR_CMD, "invalid value");
        NODE_MNG_ADD_ERR_STR(E_NOT_LEADER, "reciver is not in a leader state");

        
        m_pstErrStr_->errorArray = m_astrErrorCfg__;
    }

    friend class Singleton<NodeMngError>;
private:
    const char* m_astrErrorCfg__[E_CFG_BTM];
};

#endif  //_NODE_MANAGER_ERROR_CODE_H_