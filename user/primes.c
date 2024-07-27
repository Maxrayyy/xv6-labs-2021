#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ_END 0
#define WRITE_END 1

void sieve(int left_pipe[2]) {
    int prime, num;
    int right_pipe[2];

    // 从左侧管道读取第一个数字，它是当前进程的素数
    if (read(left_pipe[READ_END], &prime, sizeof(prime)) <= 0) {
        // 没有更多数据可读，关闭管道并退出
        close(left_pipe[READ_END]);
        exit(0);
    }

    printf("prime %d\n", prime);

    // 创建右侧管道
    if (pipe(right_pipe) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        // 子进程继续筛选
        close(right_pipe[WRITE_END]);
        sieve(right_pipe);
    } else {
        // 父进程从左侧管道读取数字，筛选并写入右侧管道
        close(right_pipe[READ_END]);

        while (read(left_pipe[READ_END], &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(right_pipe[WRITE_END], &num, sizeof(num));
            }
        }

        close(left_pipe[READ_END]);
        close(right_pipe[WRITE_END]);
        wait(0); // 等待子进程结束
    }
}

int main(int argc, char *argv[]) {
    int initial_pipe[2];
    int i;

    // 创建初始管道
    if (pipe(initial_pipe) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if (fork() == 0) {
        // 子进程开始筛选
        close(initial_pipe[WRITE_END]);
        sieve(initial_pipe);
    } else {
        // 父进程向初始管道写入数字
        close(initial_pipe[READ_END]);

        for (i = 2; i <= 35; i++) {
            write(initial_pipe[WRITE_END], &i, sizeof(i));
        }

        close(initial_pipe[WRITE_END]);
        wait(0); // 等待子进程结束
    }

    exit(0);
}
