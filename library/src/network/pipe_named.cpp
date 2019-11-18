#include "pipe_named.h"

/**
*@brief     : PipeNamed
*@param     : [i]PIPE_MODE_E Mode_F, const char *Name_F
*@note      : 构造函数，需要设置的模式和名字
*@return    :
*/
PipeNamed::PipeNamed(PIPE_MODE_E Mode_F, const char *Name_F):
    m_hPipe(INVALID_HANDLE_VALUE)
{
    atmRunState.store(TRUE);
    strcpy(m_strName, Name_F);
    if (Mode_F == E_PIPE_SERVER) {
        m_hMaster = std::thread(&PipeNamed::__createServer, this);
    }
}

/**
*@brief     : PipeNamed
*@param     : 
*@note      : 析构函数
*@return    :
*/
PipeNamed::~PipeNamed()
{
    atmRunState.store(FALSE);
    __closePipe();

    if (m_hMaster.native_handle()) {
        m_hMaster.detach();
    }
}

/**
*@brief     : PipeNamed::__createServer
*@param     :
*@note      : 创建pipe服务端
*@return    :
*/
int PipeNamed::__createServer()
{
    while (atmRunState.load()) {
        m_hPipe = CreateNamedPipe(m_strName,
            m_stSrvSet.dwOpenMode,
            m_stSrvSet.dwPipeMode,
            m_stSrvSet.nMaxInstances,
            m_stSrvSet.nOutBufferSize,
            m_stSrvSet.nInBufferSize,
            m_stSrvSet.nDefaultTimeOut,
            m_stSrvSet.lpSecurityAttributes);
        if (m_hPipe == INVALID_HANDLE_VALUE) {
            printf("create pipe failed.\r\n");
            return ST_ERR;
        }

        printf("pipe: wait for new connect...  \r\n");
        DWORD  dwThreadId = 0;
        BOOL bConnected = ::ConnectNamedPipe(m_hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (bConnected == FALSE) {
            printf("client connect error\r\n");
            __closePipe();
            return ST_ERR;
        } else {
            m_hWorker = CreateThread(NULL, 0, __WorkerThread, (LPVOID)this, 0, &dwThreadId);
            if (m_hWorker == NULL) {
                printf("create thread fail\r\n");
                return ST_ERR;
            }

            CloseHandle(m_hWorker);//give up the right of control which from WorkerThread 
        }

        printf("connected.\r\n");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return ST_OK;
}

/**
*@brief     : PipeNamed::__closePipe
*@param     :
*@note      : 关闭pipe
*@return    :
*/
VOID PipeNamed::__closePipe()
{
    if (m_hPipe != INVALID_HANDLE_VALUE) {
        printf("close pipe, bye~ \n");
        if (m_hMaster.native_handle()) {
            char wakeup;
            PipeNamed instTmp(E_PIPE_CLIENT, m_strName);
            instTmp.sendPipeMsg(&wakeup, sizeof(wakeup));
        }

        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }
}

/**
*@brief     : PipeNamed::__WorkerThread
*@param     : [i]LPVOID pvThis 
*@note      : 收到数据时的操作线程
*@return    :
*/
DWORD WINAPI PipeNamed::__WorkerThread(LPVOID pvThis)
{
    auto pthis = (PipeNamed *)pvThis;
    BOOL bSuccess = FALSE;
    ULONG ulBytesWritten;

    printf("workThread %d\r\n", ((_Thrd_t*)& std::this_thread::get_id())->_Id);

    while(pthis->atmRunState.load()) {
        bSuccess = ReadFile(pthis->m_hPipe, pthis->m_Recvbuffer, sizeof(pthis->m_Recvbuffer), &pthis->m_ulRecvBytes, NULL);
        if ((!bSuccess) || (pthis->m_ulRecvBytes == 0)) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printf("InstanceThread: client disconnected.\r\n");
            } else {
                printf("InstanceThread ReadFile failed.\r\n");
            }
            break;
        }

        if (pthis->m_funcRspForReq == nullptr) {
            pthis->__responseForReq();
        } else {
            pthis->m_funcRspForReq();
        }

        bSuccess = WriteFile(pthis->m_hPipe, pthis->m_Sendbuffer, pthis->m_ulSendBytes, &ulBytesWritten, NULL);
        if ((!bSuccess) || (pthis->m_ulSendBytes != ulBytesWritten))
        {
            printf("InstanceThread WriteFile failed.\n");
            break;
        }
    }
    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting.
    FlushFileBuffers(pthis->m_hPipe);

    DisconnectNamedPipe(pthis->m_hPipe);
    pthis->__closePipe();

    return 0;
}

/**
*@brief     : PipeNamed::__responseForReq
*@param     :
*@note      : 收到请求时回复消息
*@return    :
*/
VOID PipeNamed::__responseForReq()
{
    printf("test-string %s \r\n", m_Recvbuffer);


    //handle cmd

    strcpy(m_Sendbuffer, "hello-client!!");
    m_ulSendBytes = strlen(m_Sendbuffer) + 1;
}

/**
*@brief     : PipeNamed::__createClient
*@param     :
*@note      : 创建pipe客户端
*@return    :
*/
INT PipeNamed::__createClient()
{
    while(atmRunState.load()) {
        m_hPipe = CreateFile(m_strName,
                             GENERIC_READ | GENERIC_WRITE,
                             0,//no share
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);//no tmp file
        if (m_hPipe != INVALID_HANDLE_VALUE) {
            Sleep(5);
            break;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            printf("can't open pipe \r\n");
            return ST_ERR;
        }

        if (!WaitNamedPipe(m_strName, 1000)) {
        
            printf("1s wait timed out\r\n");
            return ST_ERR;
        }
    }

    // The pipe connected; change to message-read mode. 

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(m_hPipe, &dwMode, NULL, NULL))
    {
        printf(" SetNamedPipeHandleState fail\r\n");
        return ST_ERR;
    }

    DWORD dwWritten;
    if (!WriteFile(m_hPipe, m_Sendbuffer, m_ulSendBytes, &dwWritten, NULL))
    {
        printf(" WriteFile fail\r\n");
        return ST_ERR;
    }

    DWORD cbRead=0;
    BOOL bSuccess=FALSE;

    do {
        bSuccess = ReadFile(m_hPipe, m_Recvbuffer, sizeof(m_Recvbuffer), &cbRead, NULL);
        if (!bSuccess && GetLastError() != ERROR_MORE_DATA) {
            break;
        }
    } while (!bSuccess);  // repeat loop if ERROR_MORE_DATA 

    if (!bSuccess) {
        printf(" read response File fail\r\n");
        return ST_ERR;
    }
    atmRspArrive.store(TRUE);

    return 0;
}

/**
*@brief     : PipeNamed::regRspProc
*@param     : std::function<void()> funcRspForReq
*@note      : 注册消息处理函数
*@return    :
*/
void PipeNamed::regRspProc(std::function<void()> funcRspForReq)
{
    m_funcRspForReq = std::move(funcRspForReq);

}

/**
*@brief     : PipeNamed::sendPipeMsg
*@param     : [i]const char *pcBuff, UINT uiBufLen
*@note      : 发送pipe消息
*@return    :
*/
int PipeNamed::sendPipeMsg(const char *pcBuff, UINT uiBufLen)
{
    memcpy_s(m_Sendbuffer, sizeof(m_Sendbuffer), pcBuff, uiBufLen);
    m_ulSendBytes = uiBufLen;
    atmRspArrive.store(FALSE);
    if (__createClient() != ST_OK) {
        return ST_ERR;
    }

    return ST_OK;
}

/**
*@brief     : PipeNamed::sendPipeMsg
*@param     : [i]const char* pcBuff, UINT uiBufLen, VOID* pvbuff, UINT buffLen
*@note      : 发送pipe消息并回填返回值
*@return    :
*/
int PipeNamed::sendPipeMsg(const char* pcBuff, UINT uiBufLen, VOID* pvbuff, UINT buffLen)
{
    if (sendPipeMsg(pcBuff, uiBufLen)) {
        return ST_ERR;
    }
    memcpy_s(pvbuff, buffLen, m_Recvbuffer, sizeof(m_Recvbuffer));
    __closePipe();
    return ST_OK;
}

/**
*@brief     : PipeNamed::getPipeMsgRsp
*@param     : [i]UINT buffLen [o]VOID *pvbuff
*@note      : 获取pipe的返回值
*@return    :
*/
int PipeNamed::getPipeMsgRsp(VOID *pvbuff, UINT buffLen)
{
    for (int i=0; i < 10; ++i) {
        if (atmRspArrive.load()) {
            memcpy_s(pvbuff, buffLen, m_Recvbuffer, sizeof(m_Recvbuffer));
            return ST_OK;
        }
    }
    __closePipe();
    printf("recv rsp time out \r\n");
    return ST_ERR;
}

/**
*@brief     : PipeNamed::readBuffer
*@param     : [i]UINT buffLen [o]VOID *pvbuff
*@note      : 读pipe消息
*@return    :
*/
void PipeNamed::readBuffer(char** ppcBuff, UINT* puiBufLen)
{
    *ppcBuff = m_Recvbuffer;
    *puiBufLen = m_ulRecvBytes;
}

/**
*@brief     : PipeNamed::writeBuffer
*@param     : [i]UINT buffLen [o]VOID *pvbuff
*@note      : 写pipe消息
*@return    :
*/
void PipeNamed::writeBuffer(const char* pcBuff, UINT uiBufLen)
{
    memcpy_s(m_Sendbuffer, sizeof(m_Sendbuffer), pcBuff, uiBufLen);
}