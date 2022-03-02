#ifndef __DEFINE_H__
#define __DEFINE_H__
#include "SourceInfo.h"
#define PROFILE_FILE_NAME "profile.cfg"

#define STR_H264 "h.264"
#define STR_HEVC "hevc"
#define STR_HDMI "hdmi"
#define STR_AUDIO_IN "audio-in"
#define STR_1080P "1080p"
#define STR_720P "720p"

#define STR_DISCONNECT "Disconnect"
#define STR_CONNECTED "Connected"
#define STR_STREAMING "Streaming"

#define VERSION "1.4.0"
#define VERSION_CODE 0

#define START_STREAMING 1
#define STOP_STREAMING 0
typedef enum __DEVICE_STATE
{
    DISCONNECT = 0, //서버와 연결이 끊긴상태
    CONNECTED,      //서버와 연결중인상태
    STREAMING,      //스트리밍중인 상태
} DEVICE_STATE;
typedef enum __AUDIO_PATH
{
    HDMI = 0,
    AUDIO_IN,
} AUDIO_PATH;

typedef enum __CODEC
{
    H264 = 0,
    HEVC,
} CODEC;

typedef enum __DEVICE
{
    BOND = 0,
    TX,
} DEVICE;

typedef enum __TLS_SERVER
{
    UnKnown = 0,
    SERVER = 9
} TLS_SERVER;

typedef struct _ProtocolRecvTime ProtocolRecvTime;
struct _ProtocolRecvTime
{
    uint64_t profile_time;
    uint64_t server_status_time;
};

typedef struct _Context Context;
struct _Context
{
    SourceInfo *sourceInfo;
    int device_state; //디바이스 상태
    bool is_run;      //모든 스레드에서 사용하는 flag
    // int     is_streaming;
    int pipeline_restart;

    pthread_t menu_thread_id;
    pthread_t network_manager_thread_id;
    pthread_t http_send_devie_state_thread_id;
    pthread_t v4l2src_data_manager_thread_id;
    pthread_t gst_pipline_thread_id;
  
    int device_type;

    void (*callback_recv_profile)(void);
    void (*callback_network_send_pid)(int);

    void (*gst_control)(bool);
    void (*callback_change_bitrate)(int64_t, int);
    void (*callback_server_status)(int64_t, int);
};

#endif