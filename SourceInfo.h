#ifndef __SOURCEINFO_H__
#define __SOURCEINFO_H__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
 

typedef struct _SourceInfo SourceInfo;
struct _SourceInfo
{
    int width;
    int height;
    char* fps;
    int format;      // format 값이 1이면 p 2면 i
    
    char* (*v4l12src_get_resolution)(void*);
    char* (*v4l12src_get_fps)(void*);
    void (*root_menu_src)();
};

SourceInfo *new_sourceInfo();

void free_sourceInfo(SourceInfo *sourceInfo);
char* v4l12src_get_resolution(SourceInfo *SourceInfo);
void *v4l2src_data_manager(void *_context);
char* v4l12src_get_fps(SourceInfo *SourceInfo);
#endif