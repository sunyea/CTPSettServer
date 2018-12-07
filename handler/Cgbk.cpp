//
// Created by liaop on 18-7-20.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <locale.h>
#include <cstdlib>


class Cgbk {

public:
    static int gbk2utf8(char *utfstr,const char *srcstr,int maxutfstrlen)
    {
        if(NULL==srcstr)
        {
            printf(" bad parameter\n");
            return -1;
        }
        //首先先将gbk编码转换为unicode编码
        if(NULL==setlocale(LC_ALL,"zh_cn.gbk"))//设置转换为unicode前的码,当前为gbk编码
        {
            printf("setlocale bad parameter\n");
            return -1;
        }
        int unicodelen=mbstowcs(NULL,srcstr,0);//计算转换后的长度
        if(unicodelen<=0)
        {
            printf("can not transfer!!!\n");
            return -1;
        }
        wchar_t *unicodestr=(wchar_t *)calloc(sizeof(wchar_t),unicodelen+1);
        mbstowcs(unicodestr,srcstr,strlen(srcstr));//将gbk转换为unicode


        //将unicode编码转换为utf8编码
        if(NULL==setlocale(LC_ALL,"zh_cn.utf8"))//设置unicode转换后的码,当前为utf8
        {
            printf("bad parameter\n");
            return -1;
        }
        int utflen=wcstombs(NULL,unicodestr,0);//计算转换后的长度
        if(utflen<=0)
        {
            printf("can not transfer!!!\n");
            return -1;
        }
        else if(utflen>=maxutfstrlen)//判断空间是否足够
        {
            printf("dst str memory not enough\n");
            return -1;
        }
        wcstombs(utfstr,unicodestr,utflen);
        utfstr[utflen]=0;//添加结束符
        free(unicodestr);
        return utflen;
    }

};
