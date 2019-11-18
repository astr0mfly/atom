#include "unit_test.hpp"
#include "..\..\rsc\common\utility.h"

/*
�����ʲô��Ԫ���Ծ�ֱ�������� include
��ģ����ļ��������ʱ���Զ�ע���Ӧ�ĵ�Ԫ�����ࡣ
*/
#define S3_UT           1

#if S3_UT
#include "dfx\s3_interface\s3_test.hpp"
#endif  //S3_UT

static void printSetting(int Argc_F, char* Argv_F[], char* Envp_F[])
{
    printf("\r\n############## UNIT TEST ################\r\n");
    printf("| No.  |          Arguments(Total: [%d])\r\n", Argc_F);
    for (int iIndex = 0; iIndex < Argc_F; iIndex++) {
        printf("+------+-------------------------------\r\n");
        printf("| %-4d : %s\n", iIndex, Argv_F[iIndex]);
    }
    printf("+======+===============================\r\n");
    printf("| No.  |          Environment\r\n");
    for (int iIndex = 0; Envp_F[iIndex] != NULL; iIndex++) {
        printf("+------+-------------------------------\r\n");
        printf("| %-4d : %s\n", iIndex, Envp_F[iIndex]);
    }
    printf("+======+===============================\r\n");
    system("pause");
}

int main(int argc, char *argv[], char *envp[])
{
    printSetting(argc, argv, envp);

    InitWinSock instWinsock2;

    return RUN_ALL_TESTS(argc == 2 ? argv[1] : '\0');
}