// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/param.h"
// #include "kernel/fs.h"

// #define MAXARGS 10
// #define MSGSIZE 16

// int main(int argc, char *argv[]){
//     sleep(10);

//     //获取前一个命令的标准化输出
//     char buf[MSGSIZE];
//     read(0,buf,MSGSIZE);

//     char *xargv[MAXARGS];
//     int xargc = 0;
//     for(int i = 1;i < argc; ++i){
//         xargv[xargc] = argv[i];
//         xargc++;
//     }

//     char *p = buf;
//     for(int i = 0;i < MSGSIZE; ++i){
//         if(buf[i] == '\n'){
//             if(fork() == 0){
//                 buf[i] = 0;
//                 xargv[xargc] = p;
//                 xargc++;
//                 xargv[xargc] = 0;
//                 xargc++;
//                 exec(xargv[0],xargv);
//                 exit(0);
//             }
//             else{
//                 p = &buf[i + 1];
//                 wait(0);    
//             }
//         }
//     }
//     exit(0);
// }

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h"

#define MAXARGS 32
#define MSGSIZE 16

int main(int argc, char *argv[]) {
    // 睡眠10秒，等待前一个指令执行结束
    sleep(10);

    // 获取前一个命令的标准输出
    char buf[MSGSIZE];
    if (read(0, buf, MSGSIZE) <= 0) {
        fprintf(2, "Failed to read from stdin\n");
        exit(1);
    }

    // 设置初始参数
    char *xargv[MAXARGS];
    int xargc = 0;
    for (int i = 1; i < argc && xargc < MAXARGS - 1; ++i) {
        xargv[xargc++] = argv[i];
    }

    char *p = buf;
    for (int i = 0; i < MSGSIZE; ++i) {
        if (buf[i] == '\n' || buf[i] == 0) {
            buf[i] = 0;
            if (fork() == 0) {
                xargv[xargc] = p;
                xargv[xargc + 1] = 0;  // 末尾标记
                exec(xargv[0], xargv);
                fprintf(2, "exec %s failed\n", xargv[0]);//exec调用失败
                exit(1);
            } else {
                p = &buf[i + 1];
                wait(0);
            }
        }
    }
    exit(0);
}
