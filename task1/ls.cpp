/*************************************************************************
    > File Name: ls.cpp
    > Author: Iris Chen
    > Mail: o0lazybear0o@gmail.com 
    > Created Time: 日 11/ 2 16:41:08 2014
 ************************************************************************/

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <string>

void errInfo(char *str)
{
	if (str == NULL)
		return;
	else printf("%s\n", str);
}

void helpInfo()
{
	printf(		"-----\n");
	printf(		"Author: 1253036 Iris Chen\n");
	printf(		"Email : o0lazybear0o@gmail.com\n");
	printf(		"-----\n");
	printf(		"ls		: get the file list of the dir\n");
	printf(		"ls -h		: get the help of the command\n");
	printf(		"ls dir dir2	: get the filename list\n");
	printf(		"ls -l		: get the file list of the dir with file Info\n");
	printf(		"ls -l dir dir2	: get the file Info of the dirs\n\n");
}

void getFileInfo(char *dir, char *name, bool info)
{
	if (! info)
	{
		printf("%s ", name);
		return;
	}
    char perms[1024];
    struct stat sbuf;
    char newname[1024];
    sprintf(newname,"%s/%s",dir,name);
    stat(newname,&sbuf);
    switch(sbuf.st_mode&S_IFMT)
    {
        case S_IFREG:perms[0]=('-');break;
        case S_IFDIR:perms[0]=('d');break;
        case S_IFCHR:perms[0]=('c');break;
        case S_IFBLK:perms[0]=('b');break;
        case S_IFIFO:perms[0]=('l');break;
        case S_IFLNK:perms[0]=('l');break;
        case S_IFSOCK:perms[0]=('l');break;
        default:        perms[0]=('?'); break;
    }
    //用户权限以及处理调整用户id位
    perms[1]=((sbuf.st_mode&S_IRUSR)? 'r':'-');
    perms[2]=((sbuf.st_mode&S_IWUSR)? 'w':'-');
    if((sbuf.st_mode&S_ISUID)!=0)
        perms[3]='s';
    else    perms[3]=((sbuf.st_mode&S_IXUSR)? 'x':'-');
    //组权限以及处理调整组id位
    perms[4]=((sbuf.st_mode&S_IRGRP)? 'r':'-');
    perms[5]=((sbuf.st_mode&S_IWGRP)? 'w':'-');
    if((sbuf.st_mode&S_ISGID)!=0)
        perms[6]='s';
    else    perms[6]=((sbuf.st_mode&S_IXGRP)? 'x':'-');
    //其他人权限以及处理sticky位
    perms[7]=((sbuf.st_mode&S_IROTH)? 'r':'-');
    perms[8]=((sbuf.st_mode&S_IWOTH)? 'w':'-');
    if((sbuf.st_mode&S_ISVTX)!=0)
        perms[9]='t';
    perms[9]=((sbuf.st_mode&S_IXOTH)? 'x':'-');
    //输出访问权限、链接数、用户名与组名
    printf("%s%6d%8s%8s",perms,sbuf.st_nlink,getpwuid(sbuf.st_uid)->pw_name,getgrgid(sbuf.st_gid)->gr_name);
    //输出文件大小、修改时间
    printf("%7d %.20s",(int)sbuf.st_size,ctime(&sbuf.st_mtime));
    printf("%s\n",name);
}

void dealWithDir(char *name, bool info)
{
	struct dirent *dir;
	DIR *dp = opendir(name);
	if (dp == NULL)
	{
		printf("%s cannot open\n", name );
		return;
	}
	printf("%s:\n", name);
	while ((dir = readdir(dp)) != NULL)
		getFileInfo(name, dir->d_name, info);
	closedir(dp);
	printf("\n");
}

int main(int argc, char *argv[])
{
// debug : output all the argvs;
//	printf("%d\n", argc);
//	for (int i = 0; i < argc; ++i)
//		printf("(%s)", argv[i]);
//	printf("\n");
	
	//ls
	if (argc == 1)
		dealWithDir(".", false);
	//ls -l
	else if ((argc >= 2)&&(strcmp(argv[1], "-l") == 0))
	{
		// ls -l
		if (argc == 2)
			dealWithDir(".", true);
		// ls -l dirname
		else {
			for (int i = 2; i < argc; ++i)
				dealWithDir(argv[i], true);
		}
	}
	//ls filename
	else if ((argc >= 2)&&(access(argv[1], F_OK) >= 0))
	{
		for (int i = 2; i < argc; ++i)
			dealWithDir(argv[i], false);
	}
	//ls -h
	else if ((argc == 2)&&(strcmp(argv[1], "-h") == 0))
		helpInfo();
	// error
	else errInfo("Command Error");
	return 0;
}
