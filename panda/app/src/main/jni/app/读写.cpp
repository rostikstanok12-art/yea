#include "读写.h"

void 读写::Initializeread(int pid1){
	this->pid = pid1;
	process_vm_readv_syscall = 270;
	process_vm_writev_syscall = 271;
}

int 读写::getPID(const char*packageName)
{
	int id = -1;
	DIR*dir;
	FILE*fp;
	char filename[64];
	char cmdline[64];
	struct dirent*entry;
	dir = opendir("/proc");
	while((entry = readdir(dir))!=NULL)
	{
		id = atoi(entry->d_name);
		if(id!=0)
		{
			sprintf(filename,"/proc/%d/cmdline",id);
			fp = fopen(filename,"r");
			if(fp)
			{
				fgets(cmdline,sizeof(cmdline),fp);
				fclose(fp);
				if(strcmp(packageName,cmdline)==0)
				{
					return id;
				}
			}
		}
	}
	closedir(dir);
	return-1;
}

ssize_t 读写::process_v(pid_t __pid,const struct iovec*__local_iov,unsigned long __local_iov_count,
	const struct iovec*__remote_iov,unsigned long __remote_iov_count,
	unsigned long __flags,bool iswrite)
{
	return syscall((iswrite?process_vm_writev_syscall:process_vm_readv_syscall),__pid,
		__local_iov,__local_iov_count,__remote_iov,__remote_iov_count,__flags);
}

bool 读写::pvm(void*address,void*buffer,size_t size,bool iswrite)
{
	struct iovec local[1];
	struct iovec remote[1];
	local[0].iov_base = buffer;
	local[0].iov_len = size;
	remote[0].iov_base = address;
	remote[0].iov_len = size;
	if(pid<0)
	{
		return false;
	}
	ssize_t bytes = process_v(pid,local,1,remote,1,0,iswrite);
	return bytes==size;
}

// 读取内存
bool 读写::readv(unsigned long address,void*buffer,size_t size)
{
	return pvm(reinterpret_cast<void*>(address),buffer,size,false);
}

// 写入内存
bool 读写::writev(unsigned long address,void*buffer,size_t size)
{
	return pvm(reinterpret_cast<void*>(address),buffer,size,true);
}

float 读写::getFloat(unsigned long addr)
{
	float var = 0;
	readv(addr,&var,4);
	return(var);
}


// 获取D类内存
int 读写::getDword(unsigned long addr)
{
	int var = 0;
	readv(addr,&var,4);
	return(var);
}

// 获取指针(32位游戏)
unsigned long 读写::getPtr32(unsigned long addr)
{
	unsigned int var = 0;
	readv(addr&0xFFFFFFFFFF,&var,4);
	return(var&0xFFFFFFFFFF);
}

// 获取指针(64位游戏)
unsigned long 读写::getPtr64(unsigned long addr)
{
	unsigned long var = 0;
	readv(addr&0xFFFFFFFFFF,&var,8);
	return(var&0xFFFFFFFFFF);
}

// 写入F类内存
void 读写::writefloat(unsigned long addr,float data)
{
	writev(addr,&data,4);
}

void 读写::writedword(unsigned long addr,int data)
{
	writev(addr,&data,4);
}




// 获取基址
unsigned long 读写::get_module_base(const char*module_name)
{
	FILE*fp;
	unsigned long addr = 0;
	char*pch;
	char filename[64];
	char line[1024];
	snprintf(filename,sizeof(filename),"/proc/%d/maps",this->pid);
	fp = fopen(filename,"r");
	if(fp!=NULL)
	{
		while(fgets(line,sizeof(line),fp))
		{
			if(strstr(line,module_name))
			{
				pch = strtok(line,"-");
				addr = strtoul(pch,NULL,16);
				if(addr==0x8000)
					addr = 0;
				break;
			}
		}
		fclose(fp);
	}
	return addr;
}
unsigned long 读写::getbss(const char *module_name)
{
	long addr = 0;
	char path[64];
	char line[1024];
	char *buffer;
	char *bss;
	int flag = 0;
	buffer = strdup(module_name);
	module_name = strtok(buffer, ":");
	bss = strtok(NULL, ":");
	sprintf(path, "/proc/%d/maps", this->pid);
	FILE *fp = fopen(path, "r");
	if (fp)
	{
		while (fgets(line, sizeof(line), fp))
		{
			if (strstr(line, module_name) && strstr(line, "r-xp"))
			{
				flag = 1;
				if (!bss)
				{
					sscanf(line, "%lx", &addr);
					continue;
				}
			}
			if (flag == 1 && strstr(line, "[anon:.bss]") && strstr(line, "rw-p"))
			{
				sscanf(line, "%lx", &addr);
				break;
			}
		}
		fclose(fp);
	}
	free(buffer);
	return addr;
}
/*void 读写::getUTF8(char * buf, unsigned long namepy)
{
	unsigned short buf16[16] = { 0 };
	readv(namepy, buf16, 28);
	unsigned short *pTempUTF16 = buf16;
	char *pTempUTF8 = buf;
	char *pUTF8End = pTempUTF8 + 32;
	while (pTempUTF16 < pTempUTF16 + 28)
	{
		if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
		{
			*pTempUTF8++ = (char) * pTempUTF16;
		}
		else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
		{
			*pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
		}
		else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
		{
			*pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
			*pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
		}
		else
		{
			break;
		}
		pTempUTF16++;
	}
}*/

void 读写::getUTF8(UTF8 * buf, unsigned long namepy) {
    UTF16 buf16[250] = { 0 };
    readv(namepy, buf16, 250);
    UTF16 *pTempUTF16 = buf16;
    UTF8 *pTempUTF8 = buf;
    UTF8 *pUTF8End = pTempUTF8 + 250;
    while (pTempUTF16 < pTempUTF16 + 28) {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End) {
            *pTempUTF8++ = (UTF8) * pTempUTF16;
        } else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End) {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        } else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End) {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        } else {
            break;
        }
        pTempUTF16++;
    }
}