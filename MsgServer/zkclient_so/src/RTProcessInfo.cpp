#include "RTProcessInfo.h"

RTProcessInfo::RTProcessInfo()
{}
RTProcessInfo::~RTProcessInfo()
{}

float RTProcessInfo::GetPcpu(pid_t p)
{
    unsigned int total1, total2;
    unsigned int process1, process2;
    total1 = GetCpuTotalOccupy();
    process1 = GetCpuProcessOccupy(p);
    //usleep(1000 * 1000);//延迟500毫秒
    sleep(3);//延迟500毫秒
    total2 = GetCpuTotalOccupy();
    process2 = GetCpuProcessOccupy(p);
    float pcpu = 100.0*(process2 - process1)/(total2 - total1);
    fprintf(stderr,"====pcpu:%.6f====\n",pcpu);
    return pcpu;
}

int RTProcessInfo::GetRmem(pid_t p)
{
    return GetPhyMem(p);
}

float RTProcessInfo::GetPmem(pid_t p)
{
    int phy = GetPhyMem(p);
    int total = GetTotalMem();
    float occupy = (phy*1.0)/(total*1.0);
    fprintf(stderr,"====pmemy:%.6f====\n",occupy);
    return occupy;
}

//////////////////////////////////////////////
//////////////////private/////////////////////
//////////////////////////////////////////////

int RTProcessInfo::GetPhyMem(const pid_t p)
{
    char file[64] = {0};//文件名

    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    sprintf(file,"/proc/%d/status",p);//文件中第11行包含着

    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd

    //获取vmrss:实际物理内存占用
    int i;
    char name[32];//存放项目名称
    int vmrss;//存放内存峰值大小
    for (i=0;i<VMRSS_LINE-1;i++)
    {
        fgets (line_buff, sizeof(line_buff), fd);
    }//读到第15行
    fgets (line_buff, sizeof(line_buff), fd);//读取VmRSS这一行的数据,VmRSS在第15行
    sscanf (line_buff, "%s %d", name,&vmrss);
    //fprintf (stderr, "====%s：%d====\n", name,vmrss);
    fclose(fd);     //关闭文件fd
    return vmrss;
}


int RTProcessInfo::GetTotalMem()
{
    const char* file = "/proc/meminfo";//文件名

    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd

    //获取memtotal:总内存占用大小
    char name[32];//存放项目名称
    int memtotal;//存放内存峰值大小
    fgets (line_buff, sizeof(line_buff), fd);//读取memtotal这一行的数据,memtotal在第1行
    sscanf (line_buff, "%s %d", name,&memtotal);
    //fprintf (stderr, "====%s：%d====\n", name,memtotal);
    fclose(fd);     //关闭文件fd
    return memtotal;
}

unsigned int RTProcessInfo::GetCpuTotalOccupy()
{
    FILE *fd;         //定义文件指针fd
    char buff[1024] = {0};  //定义局部变量buff数组为char类型大小为1024
    TotalCPUOccupy t;

    fd = fopen ("/proc/stat", "r"); //以R读的方式打开stat文件再赋给指针fd
    fgets (buff, sizeof(buff), fd); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里
    /*下面是将buff的字符串根据参数format后转换为数据的结果存入相应的结构体参数 */
    char name[16];//暂时用来存放字符串
    sscanf (buff, "%s %u %u %u %u", name, &t.user, &t.nice,&t.system, &t.idle);

    //fprintf (stderr, "====%s:%u %u %u %u====\n", name, t.user, t.nice,t.system, t.idle);
    fclose(fd);     //关闭文件fd
    return (t.user + t.nice + t.system + t.idle);
}

unsigned int RTProcessInfo::GetCpuProcessOccupy(const pid_t p)
{
    char file[64] = {0};//文件名
    ProcessCPUOccupy t;

    FILE *fd;         //定义文件指针fd
    char line_buff[1024] = {0};  //读取行的缓冲区
    sprintf(file,"/proc/%d/stat",p);//文件中第11行包含着

    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd
    fgets (line_buff, sizeof(line_buff), fd); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里

    sscanf(line_buff,"%u",&t.pid);//取得第一项
    const char* q = GetItems(line_buff,PROCESS_ITEM);//取得从第14项开始的起始指针
    sscanf(q,"%u %u %u %u",&t.utime,&t.stime,&t.cutime,&t.cstime);//格式化第14,15,16,17项

    //fprintf (stderr, "====pid%u:%u %u %u %u====\n", t.pid, t.utime,t.stime,t.cutime,t.cstime);
    fclose(fd);     //关闭文件fd
    return (t.utime + t.stime + t.cutime + t.cstime);
}

const char* RTProcessInfo::GetItems(const char* buffer,int ie)
{
    assert(buffer);
    char* p = (char*)buffer;//指向缓冲区
    int len = (int)strlen(buffer);
    int count = 0;//统计空格数
    if (1 == ie || ie < 1)
    {
        return p;
    }
    int i;

    for (i=0; i<len; i++)
    {
        if (' ' == *p)
        {
            count++;
            if (count == ie-1)
            {
                p++;
                break;
            }
        }
        p++;
    }
    return p;
}
