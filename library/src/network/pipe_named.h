#ifndef PIPE_NAMED_H_
#define PIPE_NAMED_H_
#include <iostream>

#include <WinSock2.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "simTools.h"

#define PIPE_DEFAULT_BUF_SIZE 1024
#define PIPE_DEFAULT_TIME_OUT 1000

/**
*@brief     : _PIPE_MODE_E
*@detail    : 与前端定义的 K-V 操作类
*@note      : 
            PIPE_ACCESS_DUPLEX 管道是双向的
            PIPE_ACCESS_INBOUND 数据从客户端流到服务器端
            PIPE_ACCESS_OUTBOUND 数据从服务器端流到客户端
*/
typedef enum _PIPE_MODE_E
{
    E_PIPE_CLIENT,
    E_PIPE_SERVER,

    E_PIPE_BUTTOM
}PIPE_MODE_E;

/**
*@brief     : _PIPE_SRV_SETTING_S
*@detail    : pipe 的设置相关数据
*/
typedef struct _PIPE_SRV_SETTING_S
{
    DWORD                   dwOpenMode;             //数据方向
    DWORD                   dwPipeMode;             //数据传输类型
    DWORD                   nMaxInstances;          //最大连接数
    DWORD                   nOutBufferSize;         //输出buffer的大小
    DWORD                   nInBufferSize;          //输入buffer的大小
    DWORD                   nDefaultTimeOut;        //默认超时时间
    LPSECURITY_ATTRIBUTES   lpSecurityAttributes;   //安全属性
    _PIPE_SRV_SETTING_S():
        dwOpenMode(PIPE_ACCESS_DUPLEX),
        dwPipeMode(PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT),
        nMaxInstances(PIPE_UNLIMITED_INSTANCES),
        nOutBufferSize(PIPE_DEFAULT_BUF_SIZE),
        nInBufferSize(PIPE_DEFAULT_BUF_SIZE),
        nDefaultTimeOut(PIPE_DEFAULT_TIME_OUT),
        lpSecurityAttributes(NULL)
    { }
}PIPE_SRV_SETTING_S, *PPIPE_SRV_SETTING_S;

/**
*@brief     : PipeNamed
*@detail    : 命名管道
*/
class PipeNamed
{
public:
    PipeNamed(PIPE_MODE_E, const char *);
    ~PipeNamed();

    void regRspProc(std::function<void()> funcRspForReq);
    int sendPipeMsg(const char* pcBuff, UINT uiBufLen);
    int sendPipeMsg(const char* pcBuff, UINT uiBufLen, VOID* pvbuff, UINT buffLen);
    int getPipeMsgRsp(VOID* pvbuff, UINT buffLen);

    void readBuffer(char **ppcBuff, UINT *puiBufLen);
    void writeBuffer(const char* pcBuff, UINT uiBufLen);

private:
    VOID __responseForReq();
    int __createServer();
    inline VOID __closePipe();
    static DWORD WINAPI __WorkerThread(LPVOID);

    INT __createClient();
    CHAR                    m_strName[32];  //管道名字
    PIPE_SRV_SETTING_S      m_stSrvSet;     //pipe 设置
    HANDLE                  m_hPipe;

    CHAR                    m_Recvbuffer[PIPE_DEFAULT_BUF_SIZE];
    ULONG                   m_ulRecvBytes;
    CHAR                    m_Sendbuffer[PIPE_DEFAULT_BUF_SIZE];
    ULONG                   m_ulSendBytes;


    std::thread             m_hMaster;//主线程对象
    HANDLE                  m_hWorker;//操作线程句柄

    std::function<void()>   m_funcRspForReq;//消息处理的函数指针

    std::atomic<BOOL>       atmRunState;//用于线程退出的flag
    std::atomic<BOOL>       atmRspArrive;//数据是否到达的标志
};

#endif //PIPE_NAMED_H_