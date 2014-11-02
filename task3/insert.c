/*************************************************************************
    > File Name: insert.c
    > Author: Iris Chen
    > Mail: o0lazybear0o@gmail.com 
    > Created Time: Sun Nov  2 23:29:48 2014
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

char* getContent(int f, char *temp, off_t index, off_t *length){
    int read_length, offset = 0;
    char *buffer = malloc(1024);
    
    lseek(f, index, SEEK_SET);
    while((read_length=read(f, buffer, 2)) > 0){
        if(offset + read_length > sizeof(temp)){
            temp = realloc(temp, 2*offset+1024);
        }
        strncpy(temp + offset, buffer, read_length);
        offset += read_length;
    }
    
    temp[offset] = '\0';
    *length = offset;
    
    free(buffer);
    return temp;
}

int main(int argc, char const *argv[]){
    off_t index;
    const char *fileSrc, *fileDes;
    char *write_temp, *read_temp;
    int fs = 0, fd = 0;
    off_t write_length, read_length;
    //判断参数是否合法
    if(argc != 4){
        fprintf(stderr, "invalid option\n");
        return 1;
    }
    
    index = atoi(argv[3]);
    if(index < 0) index = 0;
    fileSrc = argv[1];
    fileDes = argv[2];
    
    fs = open(fileSrc, O_RDONLY|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    fd = open(fileDes, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    
    if((fs == -1)||(fd == -1)){
        if(ENOENT == errno) fprintf(stderr, "file: %s dose not exist\n", fileSrc);
        else fprintf(stderr, "file open error\n");
        return 1;
    }
    
    write_temp = malloc(1024);
    write_temp = getContent(fs, write_temp, 0, &write_length);
    off_t currpos = lseek(fd, 0, SEEK_END);
    if(currpos < index) index = currpos;
    read_temp = malloc(1024);
    read_temp = getContent(fd, read_temp, index, &read_length);
    
    lseek(fd, index, SEEK_SET);
    write(fd, write_temp, write_length-1);
    write(fd, read_temp, read_length);
    
    free(write_temp);
    free(read_temp);
    close(fs); close(fd);
    return 0;
}
