
#include "util.h"
#include "gst_tlc.h"
#include "define.h"
#include "receiver.h"
#include "pipe.h"
#include "type_definitions.h"

typedef struct _TlcProfile TlcProfile;
struct _TlcProfile
{
};
Context context;

/**
 * @brief 서버에 연결되었을 경우 호출됨
 *
 */
void connected_function()
{
}
/**
 * @brief 서버와 연결이 끊겼을 경우 호출됨.
 *
 */
void disconnected_function()
{
}
/**
 * @brief 서버의 영상 수신상태를 수신했을 경우 호출됨
 *
 * @param buffer 수신버퍼의 데이터량
 */
void queue_status_function(int buffer)
{
}

void recv_profile()
{
    gst_pipline_stop();
    if (context.sourceInfo->width != 0)
        pthread_create(&context.gst_pipline_thread_id, NULL, gst_start, (void *)&context);
}

/**
 * @brief T live studio로부터 Change Bitrate 프로토콜을 수신하면 호출된다.
 *
 * @param time
 * @param bitrate
 */
void recv_change_bitrate(uint64_t time, int bitrate)
{
    printf("\n#################Callback Change Bitrate[%d]#################\n", bitrate);
    g_object_set(video_encoder, "bitrate", bitrate * 1024, NULL);
}

/**
 * @brief 서버의 패킷 수신상태를 수신
 *
 * @param time
 * @param buffer
 */
void recv_server_status(uint64_t time, int buffer)
{
}
void recv_network_send_pid(int send_pid)
{

    printf("\n#################Callback Recv Network Pid#################\n");
    pthread_create(&context.network_manager_thread_id, NULL, netwrok_manager, (void *)&context);
    printf("\n#################network_manager_thread_id  Success#################\n");
}

void gst_pipline_stop()
{
    context.pipeline_restart = true;
    void *t_return;
    g_object_set(sink, "finish", true, NULL);
    printf("\n#################gst_pipline_stop join Start#################\n");
    pthread_join(context.gst_pipline_thread_id, &t_return);
    printf("\n#################gst_pipline_stop join End#################\n");
}
/**
 * @brief Gstreamer Pipeline을 종료후 시작한다.
 *
 * @param streaming
 */
void gst_pipline_control(bool streaming)
{
    // //파이프라인 종료
    printf("#############streaming : %d\n", streaming);
    context.pipeline_restart = true;
    void *t_return;
    g_object_set(sink, "finish", true, NULL);
    pthread_join(context.gst_pipline_thread_id, &t_return);

    context.pipeline_restart = false;
    if (streaming)
        context.device_state = STREAMING;
    else
        context.device_state = CONNECTED;

    sleep(1);
    if (context.sourceInfo->width != 0)
        pthread_create(&context.gst_pipline_thread_id, NULL, gst_start, (void *)&context);
}

void initialize()
{
    //모든 context 초기화.
    memset(&context.protocolRecvTime, 0x0, sizeof(ProtocolRecvTime));
    context.is_run = true;
    context.device_state = DISCONNECT;
    context.pipeline_restart = false;
    context.callback_recv_profile = recv_profile;
    context.gst_control = gst_pipline_control;
    context.callback_change_bitrate = recv_change_bitrate;
    context.callback_server_status = recv_server_status;
    context.callback_network_send_pid = recv_network_send_pid;
    context.profileInfo = new_profile();
    context.device_type = BOND;        //디바이스 타입 셋팅 argv 에 따라서 셋팅함.
    if (context.status == NULL)        //다른스레드들에서 사용하므로 최초에 한번만 메모리 할당
        context.status = new_status(); // status는 모든 talkback clienet들이 공유함. status에 상태값을 셋팅.
    context.gst_pipline_thread_id = 0;
    
}

int main(int argc, char *argv[])
{
    if (argv[1] != NULL)
    {
        if (strcmp(argv[1], "-tx") == 0)
        {
             signal(SIGPIPE, SIG_IGN);
            memset(&context, 0x0, sizeof(Context));

            char event_code[20];
            int frameRate;
            int bitRate;
            char main_stream_codec[20];
            char server_ip[30];
            int server_port;
            char user_name[30];
            char resolution[20];
            char audio_path[20];

            printf("User Name : ");
            scanf("%s", &user_name);
            printf("Server IP : ");
            scanf("%s", &server_ip);
            printf("Server Port : ");
            scanf("%d", &server_port);
            printf("Resolution : ");
            scanf("%s", &resolution);
            printf("Codec : ");
            scanf("%s", &main_stream_codec);
            printf("Bitrate : ");
            scanf("%d", &bitRate);
            printf("Framerate : ");
            scanf("%s", &frameRate);
            printf("Audio Path :");
            scanf("%s", &audio_path);
            printf("Event Code :");
            scanf("%s", &event_code);

            tx_profile_write(user_name, server_ip, server_port, resolution, main_stream_codec, bitRate, frameRate, audio_path, event_code);

           

            pthread_create(&context.gst_pipline_thread_id, NULL, gst_tx_start, (void *)&context);

            while (true)
            {
            sleep(100);
            }

            free(context.status);
            free_profile(context.profileInfo);
        }
    }
    else
    {

        
        signal(SIGPIPE, SIG_IGN);
        memset(&context, 0x0, sizeof(Context));

        context.sourceInfo = new_sourceInfo();
        pthread_create(&context.v4l2src_data_manager_thread_id, NULL, v4l2src_data_manager, (void *)&context);
        pthread_create(&context.menu_thread_id, NULL, menu_manager, (void *)&context);

        // if(context.sourceInfo->width != 0)
        //     pthread_create(&context.gst_pipline_thread_id, NULL, gst_start, (void *)&context);
        while (true)
        {
            sleep(100);
        }

        free(context.status);
        free_profile(context.profileInfo);
        free_sourceInfo(context.sourceInfo);
    }
}
