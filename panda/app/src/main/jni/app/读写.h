#ifndef syscall读写_H
#define syscall读写_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <malloc.h>
#include <math.h>
#include <thread>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <locale>
#include <string>
#include <codecvt>
#include <dlfcn.h>
#include <vector>
using namespace std;

typedef char UTF8;
typedef unsigned short UTF16;
class 读写{
public:
int process_vm_readv_syscall,process_vm_writev_syscall,pid;
void Initializeread(int pid1);
int getPID(const char *packageName);
ssize_t process_v(pid_t __pid, const struct iovec *__local_iov, unsigned long __local_iov_count,const struct iovec *__remote_iov, unsigned long __remote_iov_count,unsigned long __flags, bool iswrite);
bool pvm(void *address, void *buffer, size_t size, bool iswrite);
bool writev(unsigned long address, void *buffer, size_t size);
bool readv(unsigned long address, void *buffer, size_t size);
float getFloat(unsigned long addr);
int getDword(unsigned long addr);
unsigned long getPtr64(unsigned long addr);
unsigned long getPtr32(unsigned long addr);
void writefloat(unsigned long addr, float data);
void writedword(unsigned long addr,int data);
unsigned long get_module_base(const char *module_name);
unsigned long getbss(const char *module_name);
//void getUTF8(char * buf, unsigned long namepy);
void getUTF8(UTF8 * buf, unsigned long namepy);
};


#endif