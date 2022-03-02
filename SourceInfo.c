
#include "define.h"
#include "SourceInfo.h"
#include "receiver.h"

SourceInfo *new_sourceInfo()
{
    SourceInfo *sourceInfo = (SourceInfo *)malloc(sizeof(SourceInfo));

    sourceInfo->width = 0;
    sourceInfo->height = 0;
    sourceInfo->format = 0;

    sourceInfo->fps = (char *)malloc(sizeof(char));
    sourceInfo->v4l12src_get_resolution = v4l12src_get_resolution;
    sourceInfo->v4l12src_get_fps = v4l12src_get_fps;
    sourceInfo->root_menu_src = root_menu_src_data;

    return sourceInfo;
}

void free_sourceInfo(SourceInfo *sourceInfo)
{
    if (sourceInfo->fps != NULL)
        free(sourceInfo->fps);
    if (sourceInfo != NULL)
        free(sourceInfo);
}

char *v4l12src_get_resolution(SourceInfo *SourceInfo)
{
    if (SourceInfo->width == 1920)
    {
        if (SourceInfo->height == 1080)
        {
            if (SourceInfo->format == 1)
                return "1080p";
            else if (SourceInfo->format == 2)
                return "1080i";
        }
    }
    else if (SourceInfo->width == 1280)
    {
        if (SourceInfo->height == 720)
        {
            if (SourceInfo->format == 1)
                return "720p";
            else if (SourceInfo->format == 2)
                return "720i";
        }
    }
    else if (SourceInfo->width == 640)
    {
        if (SourceInfo->height == 360)
        {
            if (SourceInfo->format == 1)
                return "360p";
            else if (SourceInfo->format == 2)
                return "360i";
        }
    }
    else
    {
        return "unknown";
    }
}

char *v4l12src_get_fps(SourceInfo *SourceInfo)
{
    if (SourceInfo->fps == NULL)
        return "0";
    else
        return SourceInfo->fps;
}

void *v4l2src_data_manager(void *_context)
{
    FILE *fp = NULL;

    char line[10240];
    char *width;
    char *height;
    char *format;
    char *fps;
    Context *context = (Context *)_context;
    SourceInfo *sourceInfo = context->sourceInfo;

    while (true)
    {
        /* 명령어 수행에 대한 pipe를 호출함 */
        if ((fp = popen("v4l2-ctl --query-dv-timings", "r")) == NULL)
        {
            return 1;
        }

        while (fgets(line, 10240, fp) != NULL)
        {
            if (strstr(line, "Active width:"))
            {

                width = strchr(line, ':');
                width = strtok(width + 1, " ");
                sourceInfo->width = atoi(width);
            }
            else if (strstr(line, "Active height:"))
            {

                height = strchr(line, ':');
                height = strtok(height + 1, " ");
                sourceInfo->height = atoi(height);
            }
            else if (strstr(line, "Frame format:"))
            {

                format = strchr(line, ':');
                format = strtok(format + 1, " ");

                if (strcmp(format, "interlaced\n") == 0)
                {

                    sourceInfo->format = 2;
                }
                else if (strcmp(format, "progressive\n") == 0)
                {
                    sourceInfo->format = 1;
                }
            }
            else if (strstr(line, "Pixelclock:"))
            {
                if (strstr(line, "Hz (") == NULL)
                {
                    strcpy(sourceInfo->fps, "0");
                }
                else
                {
                    strstr(line, "Hz (");
                    fps = strchr(line, '(');
                    fps = strtok(fps + 1, " ");

                    char *split = strtok(fps, ".");
                    char *compare = strtok(NULL, ".");

                    if (strcmp(compare, "00") == 0)
                    {
                        strcpy(sourceInfo->fps, split);
                    }
                    else
                    {
                        strcat(split, ".");
                        strcat(split, compare);
                        strcpy(sourceInfo->fps, split);
                    }
                }
            }
        }
        pclose(fp);
        sourceInfo->root_menu_src();
        sleep(1);
    }
}

