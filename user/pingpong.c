#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    int ping[2]; // 父到子管道
    int pong[2]; // 子到父管道

    // 创建两个管道
    pipe(ping);
    pipe(pong);

    int pid = fork();

    if (pid < 0) { // fork失败
        fprintf(2, "fork失败\n");
        exit(1);
    } 
		else if (pid == 0) { // 子进程
        close(ping[1]); // 关闭父到子管道的写端
        close(pong[0]); // 关闭子到父管道的读端

        char buf[2];
        // 从父到子管道读取"ping"
        if (read(ping[0], buf, 1) != 1) {
            fprintf(2, "子进程读取失败\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // 向子到父管道写入"pong"
        if (write(pong[1], buf, 1) != 1) {
            fprintf(2, "子进程写入失败\n");
            exit(1);
        }

        close(ping[0]); // 关闭父到子管道的读端
        close(pong[1]); // 关闭子到父管道的写端
        exit(0);
    } 
		else { // 父进程
        close(ping[0]); // 关闭父到子管道的读端
        close(pong[1]); // 关闭子到父管道的写端

        char buf[2] = "a"; // 发送的信号内容
        // 向父到子管道写入"ping"
        if (write(ping[1], buf, 1) != 1) {
            fprintf(2, "父进程写入失败\n");
            exit(1);
        }

        // 等待子进程结束
        wait(0);

        // 从子到父管道读取"pong"
        if (read(pong[0], buf, 1) != 1) {
            fprintf(2, "父进程读取失败\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(ping[1]); // 关闭父到子管道的写端
        close(pong[0]); // 关闭子到父管道的读端
        exit(0);
    }
}
