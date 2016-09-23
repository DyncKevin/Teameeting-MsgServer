#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <vector>

//注意：当字符串为空时，也会返回一个空字符串
void split(std::string& s, std::string& delim, std::vector< std::string >& ret)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=std::string::npos)
    {
        ret.push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
        //printf("split index:%lu, last:%lu\n", index, last);
    }
    if (index-last>0)
    {
        ret.push_back(s.substr(last,s.length()));
    }
}

void parse_file(const char* filename, FILE* fres)
{
    FILE* pfile = fopen(filename, "r");
    if (!pfile)
    {
        printf("flename:%s is not exists, please check\n", filename);
        exit(0);
    }

    char buf[128] = {0};
    std::list<std::string> text;
    std::string fileName(filename);

    while(fgets(buf, 128, pfile))
    {
        if (feof(pfile) || ferror(pfile))
        {
            //printf("read file flename:%s meet feof or ferror\n", filename);
            break;
        }
        std::string str(buf);
        text.push_back(str);
        //printf("sendfile read:%s\n", str.c_str());
        memset(buf, 0x00, 128);
    }

    fclose(pfile);
    pfile = NULL;

    char result[2048] = {0};
    char textRes[1024] = {0};
    std::string strsplit(":");
    long firstSeqn=0, lastSeqn=0;
    if (text.size()>0)
    {
        std::list<std::string>::iterator it = text.begin();
        std::list<std::string>::reverse_iterator rit = text.rbegin();
        std::string textFirst = *it;
        std::string textLast = *rit;

        std::vector<std::string>  FirstResult;
        split(textFirst, strsplit, FirstResult);
        //for(int i=0;i<(int)FirstResult.size();++i)
        //{
        //    printf("FirstResult[%d]:%s\n", i, FirstResult[i].c_str());
        //}

        std::vector<std::string>  LastResult;
        split(textLast, strsplit, LastResult);
        //for(int i=0;i<(int)LastResult.size();++i)
        //{
        //    printf("LastResult[%d]:%s\n", i, LastResult[i].c_str());
        //}

        firstSeqn = atoll(FirstResult[4].c_str());
        lastSeqn = atoll(LastResult[4].c_str());
        sprintf(textRes, "text line number:%lu, firstSeqn:%ld, lastSeqn:%ld,\t", text.size(), firstSeqn, lastSeqn);
    }

    int lll = (int)(lastSeqn - firstSeqn - text.size() + 1);
    if ((int)(lastSeqn-firstSeqn) == (int)(text.size() - 1))
    {
        sprintf(result, "Result:%s, [**]:%s ok lll is:%d\n", filename, textRes, lll);
    } else {
        sprintf(result, "Result:%s, [**]:%s not ok lll is:%d\n", filename, textRes, lll);
    }
    fwrite(result, 1, (int)strlen(result), fres);
    fflush(fres);
    printf("%s\n", result);
    return;
}

int main(int argc, const char* argv[])
{
    //if (argc < 2)
    //{
    //    std::cout << "params is less, please add data file_path" << std::endl;
    //    exit(0);
    //}
    FILE* result = fopen("./result", "a+");
    parse_file(argv[1], result);
    //parse_file(argv[2], result);
    fclose(result);
    result = NULL;

    return 0;
}
