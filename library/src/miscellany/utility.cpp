#include "utility.h"
#include "stdio.h"

namespace lexical
{
const char *cg_strTrue = "true";
const char *cg_strFalse = "false";
}   // end of namespace lexical


/**
*@brief     : printMemory
*@param     :
*@note      : ´òÓ¡ÄÚ´æ
*@return    :
*/
void printMemory(const char *buf, int len)
{
    int i, j, k;
    char binstr[80];
    int addr = (int)buf;
    for (i = 0; i < len; i++) {
        if (0 == (i % 16)) {
            sprintf_s(binstr, sizeof(binstr), "%08x -", i + addr);
            sprintf_s(binstr, sizeof(binstr), "%s %02x", binstr, (unsigned char)buf[i]);
        }
        else if (15 == (i % 16)) {
            sprintf_s(binstr, sizeof(binstr), "%s %02x", binstr, (unsigned char)buf[i]);
            sprintf_s(binstr, sizeof(binstr), "%s  ", binstr);
            for (j = i - 15; j <= i; j++) {
                sprintf_s(binstr, sizeof(binstr), "%s%c", binstr, ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
            }
            printf("%s\n", binstr);
        }
        else {
            sprintf_s(binstr, sizeof(binstr), "%s %02x", binstr, (unsigned char)buf[i]);
        }
    }
    if (0 != (i % 16)) {
        k = 16 - (i % 16);
        for (j = 0; j < k; j++) {
            sprintf_s(binstr, sizeof(binstr), "%s   ", binstr);
        }
        sprintf_s(binstr, sizeof(binstr), "%s  ", binstr);
        k = 16 - k;
        for (j = i - k; j < i; j++) {
            sprintf_s(binstr, sizeof(binstr), "%s%c", binstr, ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
        }
        printf("%s\n", binstr);
    }
}