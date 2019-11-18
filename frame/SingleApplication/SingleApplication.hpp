#ifndef _SINGLEAPPLICATION_H
#define _SINGLEAPPLICATION_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>   
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifndef PATH_MAX
#define PATH_MAX 1024   // 默认最大路径长度
#endif

class SingleApplication
{
public:
    static bool isRunning()
    {
        // 获取当前可执行文件名
        std::string processName = currentExeName();
        if (processName.empty())
        {
            exit(1);
        }

        // 打开或创建一个文件
        std::string filePath = std::string("/var/run/") + processName + ".pid";
        int fd = open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd < 0)
        {
            printf("Open file failed, error : %s", strerror(errno));
            exit(1);
        }

        // 将该文件锁定
        // 锁定后的文件将不能够再次锁定
        struct flock fl;
        fl.l_type = F_WRLCK; // 写文件锁定
        fl.l_start = 0;
        fl.l_whence = SEEK_SET;
        fl.l_len = 0;
        int ret = fcntl(fd, F_SETLK, &fl);
        if (ret < 0)
        {
            if (errno == EACCES || errno == EAGAIN)
            {
                printf("%s already locked, error: %s\n", filePath.c_str(), strerror(errno));
                close(fd);
                return true;
            }
        }

        // 锁定文件后，将该进程的pid写入文件
        char buf[16] = {'\0'};
        sprintf(buf, "%d", getpid());
        ftruncate(fd, 0);
        ret = write(fd, buf, strlen(buf));
        if (ret < 0)
        {
            printf("Write file failed, file: %s, error: %s\n", filePath.c_str(), strerror(errno));
            close(fd);
            exit(1);
        }

        // 函数返回时不需要调用close(fd)
        // 不然文件锁将失效
        // 程序退出后kernel会自动close
        return false;
    }

private:
    static std::string currentExeName()
    {
        char buf[PATH_MAX] = {'\0'};

        int ret = readlink("/proc/self/exe", buf, PATH_MAX);
        if (ret < 0 || ret >= PATH_MAX)
        {
            return "";
        }

        std::string path(buf);
        int pos = path.find_last_of("/");
        if (pos == -1)
        {
            return "";
        }

        path = path.substr(pos + 1, path.size() - 1);
        return path;
    }
};

#endif
