#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <math.h>

#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "sender.h"
#include "receiver.h"
#include "http_client.h"
#include "util.h"
#include "define.h"
// GStreamer add

GMainLoop *loop;
GstElement *pipeline, *videosrc, *video_filter1, *video_filter2, *video_convert, *video_rate_raw,*jpegdec, *x_raw;
GstElement *video_pasre;

GstMessage *msg;
GstCaps *caps;
GstElement *tee;

GstElement *video_scale, *video_rate, *jpgenc, *jpg_filesink, *video_rate_caps, *video_scale_caps, *identity;
GstElement *audio_level, *audio_rate_caps, *audio_faac_caps, *audio_src, *audio_convert, *audio_resample, *audio_audiorate, *audio_encode;
GstElement *queue_audio1, *queue_audio2;
GstElement *queue_display1, *queue_display2;
GstElement *queue_video1, *queue_video2, *queue_video3;
GstElement *muxer;

gboolean flag;
guint bitrate = 40000000;

int receiveMsgPid;
int sendMsgPid;

struct device_info p1;
extern Context context;
struct timeval left_start_time = {};
struct timeval right_start_time = {};
struct timeval btn_click_time = {};

static void
send_audio_level_meter(gdouble rms, gdouble peak, gint channel)
{
	int left_level = 0;
	int right_level = 0;
	if (channel == 0)
	{
		if (fabs(rms) < 10)
		{
			left_level = 13;
		}
		else if (fabs(rms) < 17)
		{
			left_level = 12;
		}
		else if (fabs(rms) < 24)
		{
			left_level = 11;
		}
		else if (fabs(rms) < 31)
		{
			left_level = 10;
		}
		else if (fabs(rms) < 38)
		{
			left_level = 9;
		}
		else if (fabs(rms) < 45)
		{
			left_level = 8;
		}
		else if (fabs(rms) < 52)
		{
			left_level = 7;
		}
		else if (fabs(rms) < 59)
		{
			left_level = 6;
		}
		else if (fabs(rms) < 66)
		{
			left_level = 5;
		}
		else if (fabs(rms) < 73)
		{
			left_level = 4;
		}
		else if (fabs(rms) < 80)
		{
			left_level = 3;
		}
		else if (fabs(rms) < 87)
		{
			left_level = 2;
		}
		else if (fabs(rms) < 96)
		{
			left_level = 1;
		}
		else if (fabs(rms) > 96)
		{
			left_level = 0;
		}
		// printf("left level : %d\n",left_level);
		// setLeftAudioLevel(sendMsgPid, left_level);
		if (left_start_time.tv_sec == 0)
			gettimeofday(&left_start_time, NULL);
		struct timeval current_time;
		gettimeofday(&current_time, NULL);
		long msec = (current_time.tv_sec - left_start_time.tv_sec) * 1000 + (current_time.tv_usec - left_start_time.tv_usec) / 1000;
		// printf("left msec : %lld\n",msec);

		if (msec >= 500)
		{
			setLeftAudioLevel(sendMsgPid, left_level);
			gettimeofday(&left_start_time, NULL);
			// printf("left level : %d\n",left_level);
		}
	}
	else if (channel == 1)
	{
		if (fabs(rms) < 10)
		{
			right_level = 13;
		}
		else if (fabs(rms) < 17)
		{
			right_level = 12;
		}
		else if (fabs(rms) < 24)
		{
			right_level = 11;
		}
		else if (fabs(rms) < 31)
		{
			right_level = 10;
		}
		else if (fabs(rms) < 38)
		{
			right_level = 9;
		}
		else if (fabs(rms) < 45)
		{
			right_level = 8;
		}
		else if (fabs(rms) < 52)
		{
			right_level = 7;
		}
		else if (fabs(rms) < 59)
		{
			right_level = 6;
		}
		else if (fabs(rms) < 66)
		{
			right_level = 5;
		}
		else if (fabs(rms) < 73)
		{
			right_level = 4;
		}
		else if (fabs(rms) < 80)
		{
			right_level = 3;
		}
		else if (fabs(rms) < 87)
		{
			right_level = 2;
		}
		else if (fabs(rms) < 96)
		{
			right_level = 1;
		}
		else if (fabs(rms) > 96)
		{
			right_level = 0;
		}
		if (right_start_time.tv_sec == 0)
			gettimeofday(&right_start_time, NULL);
		struct timeval current_time;
		gettimeofday(&current_time, NULL);

		long msec = (current_time.tv_sec - right_start_time.tv_sec) * 1000 + (current_time.tv_usec - right_start_time.tv_usec) / 1000;
		// printf("right msec : %lld\n",msec);

		if (msec >= 500)
		{
			gettimeofday(&right_start_time, NULL);
			setRightAudioLevel(sendMsgPid, right_level);
			// printf("right level : %d\n",right_level);
		}
		// setRightAudioLevel(sendMsgPid, right_level);
	}
}

static void
read_video_props(GstCaps *caps)
{
	gint width, height;
	const GstStructure *str;

	g_return_if_fail(gst_caps_is_fixed(caps));

	str = gst_caps_get_structure(caps, 0);
	if (!gst_structure_get_int(str, "width", &width) ||
		!gst_structure_get_int(str, "height", &height))
	{
		g_print("No width/height available\n");
		return;
	}

	g_print("The video size of this set of capabilities is %dx%d\n",
			width, height);
}

static gboolean
audio_cb(GstBus *bus, GstMessage *message, gpointer user_data)
{
	if (message->type == GST_MESSAGE_ELEMENT)
	{

		const GstStructure *s = gst_message_get_structure(message);
		const gchar *name = gst_structure_get_name(s);

		if (strcmp(name, "level") == 0)
		{
			gint channels;
			GstClockTime endtime;
			gdouble rms_dB, peak_dB, decay_dB;
			gdouble rms;
			const GValue *array_val;
			const GValue *value;
			GValueArray *rms_arr, *peak_arr, *decay_arr;
			gint i;

			if (!gst_structure_get_clock_time(s, "endtime", &endtime))
				g_warning("Could not parse endtime");

			/* the values are packed into GValueArrays with the value per channel */
			array_val = gst_structure_get_value(s, "rms");
			rms_arr = (GValueArray *)g_value_get_boxed(array_val);

			array_val = gst_structure_get_value(s, "peak");
			peak_arr = (GValueArray *)g_value_get_boxed(array_val);

			array_val = gst_structure_get_value(s, "decay");
			decay_arr = (GValueArray *)g_value_get_boxed(array_val);

			/* we can get the number of channels as the length of any of the value
			 * arrays */
			channels = rms_arr->n_values;
			// g_print ("endtime: %" GST_TIME_FORMAT ", channels: %d\n",
			//     GST_TIME_ARGS (endtime), channels);
			for (i = 0; i < channels; ++i)
			{
				value = rms_arr->values + i;
				rms_dB = g_value_get_double(value);

				value = peak_arr->values + i;
				peak_dB = g_value_get_double(value);

				value = decay_arr->values + i;
				decay_dB = g_value_get_double(value);
				// g_print ("    RMS: %f dB, peak: %f dB, decay: %f dB\n",
				//     rms_dB, peak_dB, decay_dB);

				/* converting from dB to normal gives us a value between 0.0 and 1.0 */
				rms = pow(10, rms_dB / 20);
				// g_print ("    normalized rms value: %f\n", rms);

				send_audio_level_meter(rms_dB, peak_dB, i);
			}
		}
		if (strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo), "0") == 0)
		{
			drawPreview(sendMsgPid, false);
		}
		else if (strcmp(name, "GstMultiFileSink") == 0)
		{
			drawPreview(sendMsgPid, true);
		}
		else if (strcmp(name, "v4l2src") == 0)
		{
			printf("OO\n");
		}
	}
	/* we handled the message we want, and ignored the ones we didn't want.
	 * so the core can unref the message for us */
	return TRUE;
}

gboolean
message_cb(GstBus *bus, GstMessage *message, gpointer user_data)
{
	int restart = *(int *)user_data;
	if (restart)
	{
		g_main_loop_quit(loop);
	}
	switch (GST_MESSAGE_TYPE(message))
	{
	case GST_MESSAGE_ERROR:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;
		name = gst_object_get_path_string(message->src);
		gst_message_parse_error(message, &err, &debug);
		g_printerr("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
			g_printerr("Additional debug info:\n%s\n", debug);
		g_error_free(err);
		g_free(debug);
		g_free(name);
		g_main_loop_quit(loop);
		break;
	}

	case GST_MESSAGE_WARNING:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;
		name = gst_object_get_path_string(message->src);
		gst_message_parse_warning(message, &err, &debug);
		g_printerr("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
			g_printerr("Additional debug info:\n%s\n", debug);
		g_error_free(err);
		g_free(debug);
		g_free(name);
		break;
	}
	case GST_MESSAGE_EOS:
	{
		g_print("Got EOS\n");
		g_main_loop_quit(loop);
		gst_element_set_state(pipeline, GST_STATE_NULL);
		g_main_loop_unref(loop);
		gst_object_unref(pipeline);
		exit(0);
		break;
	}
	default:
		break;
	}
	return TRUE;
}
void sigintHandler(int unused)
{
	g_print("You ctrl-c-ed! Sending EoS");
	gst_element_send_event(pipeline, gst_event_new_eos());
}

// GStreamer end

typedef struct
{
	long data_type;
	unsigned char data_buff[BUFF_SIZE];
} receive_data;

typedef struct
{
	int y;
	int index;
} previous_data;

previous_data previous[1] = {10, 10};
int x = 0;
int y = 0;

bool click = false;
int subMenuCheck = 0;

bool rootMenu = false;
int root_memu_check = 0;
int currentIndex = 0;
int get_version_code = 0;

int statusMainMenu[6] = {

	0, // root Menu
	0, // main Menu
	0, // tlivestudioMenu
	0, // encoderMenu
	0, // systemMenu
	0  // settingMenu

};

char *rootMenuListFormat[4] = {
	"Title %s",
	"SRC %s %sfps",
	"DST %s %sfps, %s",
	"%s"};

char *rootMenuList[4] = {
	"",
	"",
	"",
	""};

char *mainMenuControll[5] = {
	"BACK",
	"T live studio >",
	"Encoder >",
	"System >",
	"Load Profile >",
};

char *tlivestudioSubMenuFormat[4] = {
	"BACK",
	"IP %s",
	"Port %s",
	"Event Code %s",
};

char *tlivestudioSubMenu[4] = {
	"BACK",
	"",
	"",
	""};

char *encoderSubMenuFormat[6] = {
	"BACK",
	"Resolution %s ",
	"Frameate %s ",
	"Bitrate(kbps) %s ",
	"Main Stream %s ",
	"Audio %s "};

char *encoderSubMenu[6] = {
	"BACK",
	"",
	"",
	"",
	""};

char *systemSubMenuFormat[5] = {
	"BACK",
	"Check Update",
	"Firmware ver. %s",
	"DeviceID %s",
	"ETH IP %s"};

char *systemSubMenu[5] = {
	"BACK",
	"Check Update",
	"Firmware ver. %s",
	"DeviceID %s",
	"ETH IP %s"};

char *loadProfileSubMenu[4] = {
	"BACK",
	"",
	"",
	""};

char real_token[518];

void *menu_manager(void *_context)
{
	Context *context = (Context *)_context;
	ProfileInfo *profile = context->profileInfo;
	memcpy(p1.userName, profile->name, strlen(profile->name));
	memcpy(p1.serverIp, profile->serverIp, strlen(profile->serverIp));
	sprintf(p1.serverPort, "%d", profile->serverPort);
	memcpy(p1.resolution, profile->resolution, strlen(profile->resolution));
	memcpy(p1.eventCode, profile->eventCode, strlen(profile->eventCode));
	sprintf(p1.frameRate, "%d", profile->fps);
	sprintf(p1.mainStreamCodec, "%d", profile->codec);
	sprintf(p1.bitRate, "%d", profile->bitrate);

	switch (profile->codec)
	{
	case H264:
		strcpy(p1.mainStreamCodec, "H264");
		break;
	case HEVC:
		strcpy(p1.mainStreamCodec, "HEVC");
		break;
	}
	switch (profile->audioPath)
	{
	case HDMI:
		strcpy(p1.audioPath, "HDMI");
		break;
	case AUDIO_IN:
		strcpy(p1.audioPath, "Audio In");
		break;
	}

	receive_data receiveData;
	t_data sendData;

	if (-1 == (receiveMsgPid = msgget((key_t)2345, IPC_CREAT | 0666)))
	{

		perror("msgget() failed");
		exit(1);
	}
	else
	{
		context->callback_network_send_pid(sendMsgPid);
	}

	if (-1 == (sendMsgPid = msgget((key_t)1234, IPC_CREAT | 0666)))
	{
		perror("msgget() failed");
		exit(1);
	}

	initRootMenu(sendMsgPid);
	memset(&btn_click_time, 0x0, sizeof(struct timeval));
	gettimeofday(&btn_click_time, NULL);
	while (1)
	{
		// datatype 0: receive all datatype
		// change datatype to 1 if you want to receive python data
		if (-1 == msgrcv(receiveMsgPid, &receiveData, sizeof(t_data) - sizeof(long), 0, 0))
		{
			perror("msgrcv() failed");
			exit(1);
		}

		struct timeval current_time;
		gettimeofday(&current_time, NULL);

		long msec = (current_time.tv_sec - btn_click_time.tv_sec) * 1000 + (current_time.tv_usec - btn_click_time.tv_usec) / 1000;
		if (msec <= 100)
		{
			printf("~~~~~~~~~~~~~~~~~~~~~~~버튼 무시~~~~~~~~~~~~~~~~~~~~~~\n");
			gettimeofday(&btn_click_time, NULL);

			continue;
		}

		if (receiveData.data_type == PRESS_LEFT_BUTTON)
		{
			// x -= 1;
			// channelControl(sendMsgPid, x, y);
		}

		if (receiveData.data_type == PRESS_RIGHT_BUTTON)
		{
		}

		else if (receiveData.data_type == PRESS_DOWN_BUTTON)
		{

			if (!rootMenu)
			{
				y += 1;
				channelControl(sendMsgPid, x, y);
			}
		}

		else if (receiveData.data_type == PRESS_UP_BUTTON)
		{

			if (!rootMenu)
			{
				y -= 1;
				channelControl(sendMsgPid, x, y);
			}
		}

		else if (receiveData.data_type == PRESS_CENTER_BUTTON)
		{
			click = true;
			channelControl(sendMsgPid, x, y);
			click = false;
		}

		else if (receiveData.data_type == PRESS_STREAM_BUTTON)
		{

			if (context->status->broadcast == 0)
			{
				if (strcmp(context->sourceInfo->fps, "0") != 0)
				{
					context->status->broadcast = 1;
				}
			}
			else
				context->status->broadcast = 0;
			context->gst_control(context->status->broadcast);
		}
		// end
	}
}

void initRootMenu(int sendMsgPid)
{

	if (x == 0 && y == 0)
	{
		setChoiceTextNumber(sendMsgPid, -1);

		//메뉴 초기 값 세팅하기
		char name[100];
		sprintf(name, rootMenuListFormat[0], p1.userName);
		rootMenuList[0] = name;

		char src[100];
		sprintf(src, rootMenuListFormat[1], context.sourceInfo->v4l12src_get_resolution(context.sourceInfo), context.sourceInfo->v4l12src_get_fps(context.sourceInfo));
		rootMenuList[1] = src;

		char dst[100];
		sprintf(dst, rootMenuListFormat[2], p1.resolution, p1.frameRate, p1.mainStreamCodec);
		rootMenuList[2] = dst;

		char connect_state[100];
		if (strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo), "0") == 0 && context.device_state == DISCONNECT)
		{
			sprintf(connect_state, rootMenuListFormat[3], "Not Connected");
		}
		else if (strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo), "0") != 0 && context.device_state == STREAMING)
		{
			sprintf(connect_state, "SND % kbps", context.status->sending_bitrate);
		}
		else if (strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo), "0") != 0 && context.device_state == CONNECTED)
			sprintf(connect_state, rootMenuListFormat[3], "Ready to send");

		rootMenuList[3] = connect_state;

		for (int i = 0; i < sizeof(rootMenuList); i++)
		{
			updateDrawText(i, sendMsgPid, rootMenuList[i]);
		}

		if (root_memu_check == 0)
		{
			root_memu_check += 1;
			setLeftAudioLevel(sendMsgPid, 0);
			setRightAudioLevel(sendMsgPid, 0);
		}
		usbNetworkStateFirst(sendMsgPid, false);
		usbNetworkStateSecond(sendMsgPid, false);
		usbNetworkStateThird(sendMsgPid, false);
		usbNetworkStateFourth(sendMsgPid, false);
		usbNetworkStateFifth(sendMsgPid, false);
		lanNetworkState(sendMsgPid, true);

		rootMenu = true;
	}
}

void channelControl(int sendMsgPid, int d_x, int d_y)
{

	// printf("\n\n click : %d \n\n", click);
	if (d_y <= 0)
	{
		y = 0;
	}

	//루트 상태에서, 선택 버튼을 누르면 메인 메뉴 화면으로 변경
	if (rootMenu && click)
	{
		rootMenu = false;
		settingStatusMainMenu(1);
	}

	//메뉴 화면
	if (statusMainMenu[1] == 1 || x == -5)
	{
		settingStatusMainMenu(1);
		x = -2;
		if (previous->y != 10 && previous->index != 10)
		{
			y = previous->y;
			currentIndex = previous->index;
			previous->y = 10;
			previous->index = 10;
		}
		viewMainMenu(sendMsgPid, y);
	}

	// root 메뉴
	if (d_x == -2 && d_y == 0 && click)
	{
		x = 0;
		y = 0;
		settingStatusMainMenu(0);
		initRootMenu(sendMsgPid);
	}

	// TliveStudio 메뉴
	else if (d_x == -2 && d_y == 1 && click || statusMainMenu[2] == 1)
	{

		savePreviousData();
		mainMenu_x_y_setting(statusMainMenu[2]);
		settingStatusMainMenu(2);
		viewTliveStudioMenu(sendMsgPid, y);
	}

	// Encoder 메뉴
	else if (d_x == -2 && d_y == 2 && click || statusMainMenu[3] == 1)
	{

		savePreviousData();
		mainMenu_x_y_setting(statusMainMenu[3]);
		settingStatusMainMenu(3);
		viewEncoderMenu(sendMsgPid, y);
	}
	// System Menu
	else if (d_x == -2 && d_y == 3 && click || statusMainMenu[4] == 1)
	{

		savePreviousData();
		mainMenu_x_y_setting(statusMainMenu[4]);
		settingStatusMainMenu(4);
		viewSystemMenu(sendMsgPid, y);
	}
	// LoadProfile Menu
	else if (d_x == -2 && d_y == 4 && click || statusMainMenu[5] == 1)
	{

		savePreviousData();
		mainMenu_x_y_setting(statusMainMenu[5]);
		settingStatusMainMenu(5);
		viewSettingMenu(sendMsgPid, y);
	}
}

void viewMainMenu(int sendMsgPid, int d_y)
{

	menuScrollIndex(sendMsgPid, sizeof(mainMenuControll), mainMenuControll, d_y, "main");
}

void viewTliveStudioMenu(int sendMsgPid, int d_y)
{

	//메뉴 초기 값 세팅하기
	char serverIp[100];
	sprintf(serverIp, tlivestudioSubMenuFormat[1], p1.serverIp);
	tlivestudioSubMenu[1] = serverIp;

	char serverPort[100];
	sprintf(serverPort, tlivestudioSubMenuFormat[2], p1.serverPort);
	tlivestudioSubMenu[2] = serverPort;

	char eventCode[100];
	sprintf(eventCode, tlivestudioSubMenuFormat[3], p1.eventCode);
	tlivestudioSubMenu[3] = eventCode;

	menuScrollIndex(sendMsgPid, sizeof(tlivestudioSubMenu), tlivestudioSubMenu, d_y, "tliveStudio");
}

void viewEncoderMenu(int sendMsgPid, int d_y)
{
	//메뉴 초기 값 세팅하기
	char resolution[100];
	sprintf(resolution, encoderSubMenuFormat[1], p1.resolution);
	encoderSubMenu[1] = resolution;

	char framerate[100];
	sprintf(framerate, encoderSubMenuFormat[2], p1.frameRate);
	encoderSubMenu[2] = framerate;

	char bitrate[100];
	sprintf(bitrate, encoderSubMenuFormat[3], p1.bitRate);
	encoderSubMenu[3] = bitrate;

	char mainStream[100];
	sprintf(mainStream, encoderSubMenuFormat[4], p1.mainStreamCodec);
	encoderSubMenu[4] = mainStream;

	char audioPath[100];
	sprintf(audioPath, encoderSubMenuFormat[5], p1.audioPath);
	encoderSubMenu[5] = audioPath;

	menuScrollIndex(sendMsgPid, sizeof(encoderSubMenu), encoderSubMenu, d_y, "encoder");
}

void viewSystemMenu(int sendMsgPid, int d_y)
{
	char mac[100];
	char serial[100];
	char version_info[100];
	char eth0_ip[100];
	char ipAddr[16];
	get_user_key(mac);
	sprintf(serial, systemSubMenuFormat[3], mac);
	systemSubMenu[3] = serial;

	sprintf(version_info, systemSubMenuFormat[2], VERSION);
	systemSubMenu[2] = version_info;

	getIpAddrFromInterface("eth0", ipAddr);
	sprintf(eth0_ip, systemSubMenuFormat[4], ipAddr);
	systemSubMenu[4] = eth0_ip;

	menuScrollIndex(sendMsgPid, sizeof(systemSubMenu), systemSubMenu, d_y, "system");
}

void viewSettingMenu(int sendMsgPid, int d_y)
{
	//텍스트 지우고, 텍스트 선택 지우고
	if (subMenuCheck == 0)
	{
		int resultNumber;
		http_get_token(context.profileInfo->key, real_token);
		resultNumber = http_get_device_info(real_token, context.profileInfo->key);
		if (resultNumber != 0)
		{
			loadProfileSubMenu[1] = "";
			loadProfileSubMenu[2] = "      SUCCESS !!!!    ";
			read_device_profile("profile.cfg", &p1);

			// main 에 프로파일 수신 알림
			context.callback_recv_profile();
		}
		else
		{
			loadProfileSubMenu[1] = "";
			loadProfileSubMenu[2] = "      FAIL !!!!    ";
		}
	}
	menuScrollIndex(sendMsgPid, sizeof(loadProfileSubMenu), loadProfileSubMenu, d_y, "loadProfile");
}

int menuLength(char *SubMenu[], int size)
{
	int max_len = size / sizeof(SubMenu[0]);

	int return_y_value = -1;

	for (int i = 0; i < max_len; i++)
	{

		if (strcmp("", SubMenu[i]) != 0)
		{
			return_y_value += 1;
		}
	}
	return return_y_value;
}

void mainMenu_x_y_setting(int menuIndex)
{
	if (menuIndex == 0)
	{
		x = -4;
		y = 0;
	}
}

void menuScrollIndex(int sendMsgPid, int size, char *MenuControll[], int d_y, char *subMenuName)
{

	int maxLength = (size / sizeof(MenuControll[0]));
	int index = (size / sizeof(MenuControll[0])) % 4;
	int maxYPosition = menuLength(MenuControll, size);
	int startIndex = 0;

	if (d_y > maxYPosition)
	{
		d_y = maxYPosition;
		y = maxYPosition;
	}

	//메뉴가 4개 이상일 때
	if (index > 0)
	{

		if (currentIndex == 1 && d_y <= 4)
		{
			currentIndex = 1;
			d_y = currentMenuY(d_y, currentIndex);

			if (d_y < 0)
			{
				currentIndex = 0;
				d_y = currentMenuY(d_y, currentIndex);
			}
		}
		else if (currentIndex == 2 && d_y <= 5)
		{
			currentIndex = 2;
			d_y = currentMenuY(d_y, currentIndex);
			if (d_y < 0)
			{
				currentIndex = 1;
				d_y = currentMenuY(d_y, currentIndex);
			}
		}
		else
		{
			currentIndex = d_y - maxLength + index + 1;
			d_y = currentMenuY(d_y, currentIndex);
		}
	}
	//메뉴가 4개거나 작을 때
	else
	{
		currentIndex = 0;
	}
	if (currentIndex < 0)
		startIndex = 0;
	else if (currentIndex == 1)
		startIndex = 1;
	else if (currentIndex == 2)
		startIndex = 2;

	// loadPorfile은 Back 고정으로만 둔다.
	if (strcmp(subMenuName, "loadProfile") == 0)
		d_y = 0;

	setChoiceTextNumber(sendMsgPid, d_y);
	register int j;

	for (j = 0; j < 4; j++)
	{
		updateDrawText(j, sendMsgPid, MenuControll[startIndex]);
		startIndex += 1;
	}

	if (strcmp(subMenuName, "main") != 0 && subMenuCheck == 0)
	{
		subMenuCheck += 1;
	}
	else if (subMenuCheck != 0 && click != 0)
	{

		//여기서 Controll 할 메뉴를 함수화 해서 넣으면 가능할 거 같음
		//지금은 업데이트만
		if (strcmp(subMenuName, "system") == 0 && strcmp(MenuControll[d_y], "Check Update") == 0)
		{

			printf("\n\nCheck Update %s \n\n", real_token);

			setChoiceTextNumber(sendMsgPid, -1);
			setDrawTextFirst(sendMsgPid, "");
			setDrawTextSecond(sendMsgPid, "");
			setDrawTextThird(sendMsgPid, "");
			setDrawTextFourth(sendMsgPid, "");

			setDrawTextSecond(sendMsgPid, "   Update in progress ...");
			int get_version_code = 0;
			http_get_token(context.profileInfo->key, real_token);
			if (strcmp(real_token, "NULL") != 0)
			{

				http_get_app_version(real_token, "tlc-bond", &get_version_code);
				printf("\n\n get_version_code : %d ,  VERSION_CODE : %d   token : %s \n", get_version_code, VERSION_CODE, real_token);
				if (get_version_code > VERSION_CODE && get_version_code != -1)
				{
					setDrawTextSecond(sendMsgPid, "   Update  Start ...");
					http_get_app_update_file(real_token, "tlc-bond");
					sleep(5);
					system("./update_start.sh");
					sleep(5);
				}
				else
				{
					setDrawTextSecond(sendMsgPid, "Final Version, No Update");
					setDrawTextThird(sendMsgPid, "   ( press any key )");
					printf(" \n get_version_code IS NULL \n");
				}
			}
			else
			{
				setDrawTextSecond(sendMsgPid, " Update Fail");
				setDrawTextThird(sendMsgPid, "   ( press any key )");
				printf(" \n TOKEN IS NULL \n");
			}
		}

		if (strcmp(MenuControll[d_y], "BACK") == 0)
		{
			x -= 1;
			subMenuCheck = 0;
			channelControl(sendMsgPid, x, y);
		}
	}
	else if (strcmp(subMenuName, "main") == 0)
	{
		subMenuCheck = 0;
	}
}

void settingStatusMainMenu(int index)
{

	for (int i = 0; i < sizeof(statusMainMenu); i++)
	{
		if (i == index)
			statusMainMenu[i] = 1;
		else
			statusMainMenu[i] = 0;
	}
}

int currentMenuY(int d_y, int currentIndex)
{
	if (currentIndex < 0)
		return d_y;
	else if (d_y == -1)
		return 0;
	else
		return d_y - currentIndex;
}

void savePreviousData()
{
	if (previous->y == 10 && previous->index == 10)
	{
		previous->y = y;
		previous->index = currentIndex;
	}
}

void root_menu_src_data()
{
	if (rootMenu)
	{
		char src[100];
		char connect_state[100];
		// if(context.sourceInfo->v4l12src_get_resolution(context.sourceInfo) == NULL) {
		// sprintf(src,rootMenuListFormat[1], "unknown", context.sourceInfo->v4l12src_get_fps(context.sourceInfo));
		// } else {
		sprintf(src, rootMenuListFormat[1], context.sourceInfo->v4l12src_get_resolution(context.sourceInfo), context.sourceInfo->v4l12src_get_fps(context.sourceInfo));
		// }

		// printf(" \n\n context.device_state : %d \n\n",context.device_state);
		// HDMI 입력

		switch (context.device_state)
		{
		case DISCONNECT:
			// if(context.sourceInfo->width == 0)
			{
				sprintf(connect_state, rootMenuListFormat[3], "Not Connected");
				if (context.sourceInfo->width == 0)
					drawPreview(sendMsgPid, false);
			}

			break;
		case CONNECTED:
		{
			if (context.sourceInfo->width == 0)
			{
				sprintf(connect_state, rootMenuListFormat[3], "Connected");
				drawPreview(sendMsgPid, false);
			}
			else
			{
				sprintf(connect_state, rootMenuListFormat[3], "Ready to send");
				drawPreview(sendMsgPid, true);
			}
		}
		break;
		case STREAMING:
			sprintf(connect_state, "SND %d kbps", context.status->sending_bitrate);
			break;
		}

		// if(strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo),"0") == 0  && context.device_state == DISCONNECT ) {
		// 	sprintf(connect_state, rootMenuListFormat[3],"Not Connected");
		// 	drawPreview(sendMsgPid, false);
		// }
		// else if(strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo),"0") == 0  && context.device_state == CONNECTED) {
		// 	sprintf(connect_state, rootMenuListFormat[3],"Connected");
		// 	drawPreview(sendMsgPid, false);

		// } else if( strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo),"0") != 0 && context.device_state == STREAMING) {
		// 		sprintf(connect_state, "SND %d kbps", context.status->sending_bitrate);

		// } else if( strcmp(context.sourceInfo->v4l12src_get_fps(context.sourceInfo),"0") != 0 && context.device_state == CONNECTED) {
		// 		sprintf(connect_state, rootMenuListFormat[3],"Ready to send");
		// 		drawPreview(sendMsgPid, true);
		// }

		setDrawTextSecond(sendMsgPid, src);
		setDrawTextFourth(sendMsgPid, connect_state);
		// printf(" AAA : %s \n\n",src);
	}
}

void *netwrok_manager(void *_context)
{

	Context *context = (Context *)_context;
	ProfileInfo *profile = context->profileInfo;

	FILE *fp;
	char buf[200], ifname[20];
	char r_bytes[20], t_bytes[20], r_packets[20], t_packets[20], class_name[20], t_packets3[20], t_packets4[20];
	char data[20];

	int before = 0;
	int after = 0;

	// CPU
	long double a[4], b[4];
	int loadavg;

	while (true)
	{
		int network_state[6] = {
			0,
			0,
			0,
			0,
			0,
			0,
		};

		if ((fp = popen("lsusb -t", "r")) == NULL)
		{
			return 1;
		}

		while (fgets(buf, 200, fp) != NULL)
		{
			if (strcmp(buf, "/:  Bus 02.Port 1: Dev 1, Class=root_hub, Driver=tegra-xusb/4p, 5000M\n") == 0 || strcmp(buf, "/:  Bus 01.Port 1: Dev 1, Class=root_hub, Driver=tegra-xusb/5p, 480M\n") == 0)
			{
				continue;
			}
			else
			{
				sscanf(buf, "%[^:]: %s %s %s %s %s %s %s %s",
					   ifname, r_bytes, r_packets, t_bytes, t_packets, class_name, t_packets3, t_packets4);

				if (strcmp(class_name, "Class=CDC") == 0)
				{
					if (strcmp(ifname, "    |__ Port 3") == 0)
						network_state[5] = 1;
					if (strcmp(ifname, "        |__ Port 1") == 0)
						network_state[1] = 1;
					if (strcmp(ifname, "        |__ Port 2") == 0)
						network_state[2] = 1;
					if (strcmp(ifname, "        |__ Port 3") == 0)
						network_state[3] = 1;
					if (strcmp(ifname, "        |__ Port 4") == 0)
						network_state[4] = 1;
				}
			}
		}

		if (get_network_status("eth0") == 1)
		{
			network_state[0] = 1;
		}

		for (int i = 0; i < 6; i++)
		{
			if (network_state[i] == 1)
				updateUsbNetworkState(i, sendMsgPid, true);
			else
				updateUsbNetworkState(i, sendMsgPid, false);
		}

		fclose(fp);
		uint64_t before_size = 0;
		uint64_t after_size = 0;

		before = get_send_byte();
		if (context->status->broadcast == 1)
			g_object_get(sink, "send-size", &before_size, NULL);

		// cpu
		fp = fopen("/proc/stat", "r");
		fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
		fclose(fp);

		sleep(1);
		after = get_send_byte();
		after = after - before;
		after = (after * 8.0) / 1024.0;
		if (context->status->broadcast == 1)
			g_object_get(sink, "send-size", &after_size, NULL);

		// cpu
		fp = fopen("/proc/stat", "r");
		fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
		fclose(fp);

		int encoding_bps = ((after_size - before_size) * 8) / 1024;

		float cpu_temp = get_cpu_temp();
		context->status->temp = cpu_temp;
		context->status->encoding_bitrate = encoding_bps;
		context->status->sending_bitrate = after;
		context->status->cpu = (((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) / ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]))) * 100.00;
		;
	}
}

void *http_send_device_state_manager(void *_context)
{
	Context *context = (Context *)_context;
	ProfileInfo *profile = context->profileInfo;
	char *send_data;

	while (true)
	{
		http_get_token(profile->key, real_token);
		if (strcmp(real_token, "NULL") != 0)
		{

			switch (context->device_state)
			{
			case 0:
				send_data = "offline";
				break;

			case 1:
				send_data = "ready";
				break;

			case 2:
				send_data = "onair";
				break;
			}
			send_device_status(real_token, send_data);
		}
		sleep(10);
	}
}

void *gst_start(void *_context)
{

	Context *context = (Context *)_context;
	memset(&right_start_time, 0x0, sizeof(struct timeval));
	memset(&left_start_time, 0x0, sizeof(struct timeval));

	// SIGPIPE 시그널을 무시한다.
	signal(SIGPIPE, SIG_IGN);
	gst_init(NULL, NULL);

	pipeline = gst_pipeline_new(NULL);

	muxer = gst_element_factory_make("mpegtsmux", NULL);
	queue_audio1 = gst_element_factory_make("queue", NULL);
	queue_audio2 = gst_element_factory_make("queue", NULL);
	audio_src = gst_element_factory_make("alsasrc", NULL);
	audio_rate_caps = gst_element_factory_make("capsfilter", NULL);
	audio_faac_caps = gst_element_factory_make("capsfilter", NULL);
	audio_convert = gst_element_factory_make("audioconvert", NULL);
	audio_resample = gst_element_factory_make("audioresample", NULL);
	audio_audiorate = gst_element_factory_make("audiorate", NULL);
	audio_encode = gst_element_factory_make("faac", NULL);
	audio_level = gst_element_factory_make("level", "audio_level ");

	if (!audio_encode)
	{
		g_error("Audio Encoder is Failed");
	}

	if (!queue_audio1 || !queue_audio2 || !audio_src || !audio_rate_caps || !audio_faac_caps || !audio_convert || !audio_resample || !audio_audiorate)
	{
		g_error("Failed to Audio create elements");
		return -1;
	}

	// hw:1,2 hdmi in
	// hw:1,0 line in

	switch (context->profileInfo->audioPath)
	{
	case HDMI:
		g_object_set(audio_src, "device", "hw:1,2", NULL);
		break;
	case AUDIO_IN:
		g_object_set(audio_src, "device", "hw:1,0", NULL);

		break;
	}
	g_object_set(audio_level, "post-messages", TRUE, NULL);

	caps = gst_caps_from_string("audio/x-raw, rate=48000, channels=2");
	g_object_set(G_OBJECT(audio_rate_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("bitrate=128000 rate-control=2");
	g_object_set(G_OBJECT(audio_faac_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	queue_video1 = gst_element_factory_make("queue", NULL);
	queue_video2 = gst_element_factory_make("queue", NULL);
	queue_video3 = gst_element_factory_make("queue", NULL);
	videosrc = gst_element_factory_make("v4l2src", NULL);
	printf("context->profileInfo->codec %d\n", context->profileInfo->codec);
	switch (context->profileInfo->codec)
	{
	case HEVC:
	{
		video_pasre = gst_element_factory_make("h265parse", NULL);
		video_encoder = gst_element_factory_make("nvv4l2h265enc", NULL);
	}
	break;
	case H264:
	{
		video_pasre = gst_element_factory_make("h264parse", NULL);
		video_encoder = gst_element_factory_make("nvv4l2h264enc", NULL);
	}
	break;
	default:
		break;
	}
	video_filter1 = gst_element_factory_make("capsfilter", NULL);
	video_filter2 = gst_element_factory_make("capsfilter", "video_filter2");
	video_convert = gst_element_factory_make("nvvidconv", NULL);
	video_rate_raw = gst_element_factory_make("videorate", NULL);
	video_scale = gst_element_factory_make("videoscale", NULL);
	video_rate = gst_element_factory_make("videorate", NULL);
	jpgenc = gst_element_factory_make("jpegenc", NULL);
	jpg_filesink = gst_element_factory_make("multifilesink", NULL);
	queue_display1 = gst_element_factory_make("queue", NULL);
	video_rate_caps = gst_element_factory_make("capsfilter", NULL);
	video_scale_caps = gst_element_factory_make("capsfilter", NULL);
	identity = gst_element_factory_make("identity", NULL);

	//	sink = gst_element_factory_make("fakesink", NULL);
	sink = gst_element_factory_make("mcts", NULL);

	tee = gst_element_factory_make("tee", "tee");

	if (!muxer)
	{
		g_error("MUX is Failed");
	}
	if (!pipeline || !videosrc || !video_filter1 || !video_filter2 || !video_encoder || !video_convert || !sink || !muxer)
	{
		g_error("Failed to create elements");
		return -1;
	}

	if (!video_scale || !video_rate || !jpgenc || !jpg_filesink || !queue_display1 || !tee)
	{
		g_error("Failed to create elements");
		return -1;
	}

	// setup pipeline
	g_object_set(videosrc, "device", "/dev/video0", NULL);
	g_object_set(jpg_filesink, "location", "./res/image/test.jpg", "post-messages", TRUE, NULL);
	g_object_set(video_encoder, "insert-sps-pps", TRUE, NULL);
	g_object_set(video_encoder, "iframeinterval", context->profileInfo->fps, NULL);
	g_object_set(video_encoder, "profile", 1, NULL);
	g_object_set(video_encoder, "EnableTwopassCBR", TRUE, NULL);
	g_object_set(video_encoder, "control-rate", 1, NULL); // CBR
	g_object_set(video_encoder, "bitrate", context->profileInfo->bitrate * 1024, NULL);

	g_object_set(identity, "sync", TRUE, NULL);

	printf("###################gst-start %d\n", context->device_state);

	if (context->device_state == STREAMING)
	{
		printf("server-ip %s\n", context->profileInfo->tlsInfo->mcts_ip);
		printf("server-port %d\n", context->profileInfo->tlsInfo->mcts_port);
		printf("streaming %d\n", context->device_state);

		printf("key %s\n", context->profileInfo->key);
		printf("name %s\n", context->profileInfo->name);

		g_object_set(sink, "server-ip", context->profileInfo->tlsInfo->mcts_ip, NULL);
		g_object_set(sink, "server-port", context->profileInfo->tlsInfo->mcts_port, NULL);
		g_object_set(sink, "streaming", START_STREAMING, NULL);
		g_object_set(sink, "uuid", context->profileInfo->key, NULL);
		g_object_set(sink, "device-name", context->profileInfo->name, NULL);
	}

	// videio
	static char cap_str[256] = {
		0,
	};
	// sprintf(cap_str, "video/x-raw,format=(string)UYVY, width=(int)%d, height=(int)%d, framerate=(fraction)%s/1",
	// 	context->sourceInfo->width, context->sourceInfo->height,context->sourceInfo->fps);

	sprintf(cap_str, "video/x-raw,format=(string)UYVY, width=(int)%d, height=(int)%d",
			context->sourceInfo->width, context->sourceInfo->height);
	printf("caps 1 : %s\n", cap_str);

	// caps = gst_caps_from_string("video/x-raw,format=(string)UYVY, width=(int)1920, height=(int)1080, framerate=(fraction)60/1");
	caps = gst_caps_from_string(cap_str);

	g_object_set(G_OBJECT(video_filter1), "caps", caps, NULL);
	gst_caps_unref(caps);

	memset(cap_str, 0x0, sizeof(cap_str));
	sprintf(cap_str, "video/x-raw(memory:NVMM),format=(string)I420, width=(int)%d, height=(int)%d,framerate=%d/1",
			context->profileInfo->width, context->profileInfo->height, context->profileInfo->fps);

	// caps = gst_caps_from_string("video/x-raw(memory:NVMM),format=(string)I420 width=(int)1920, height=(int)1080, framerate=(fraction)60/1");
	caps = gst_caps_from_string(cap_str);
	printf("caps 2 : %s\n", cap_str);
	g_object_set(G_OBJECT(video_filter2), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("video/x-raw, framerate=2/1");
	g_object_set(G_OBJECT(video_rate_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("video/x-raw, width=80,height=60");
	g_object_set(G_OBJECT(video_scale_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add_many(GST_BIN(pipeline), videosrc, video_filter1, tee, queue_video1, video_convert, video_filter2, video_rate_raw, queue_video2, video_encoder, video_pasre,
					 queue_video3, muxer, sink,
					 queue_display1, video_scale, video_scale_caps, video_rate, video_rate_caps, identity, jpgenc, jpg_filesink,
					 audio_src, audio_level, audio_convert, audio_resample, audio_audiorate, audio_rate_caps,
					 queue_audio1, audio_encode, audio_faac_caps, queue_audio2, NULL);

	if (!gst_element_link_many(videosrc, video_filter1, tee, NULL)

		|| !gst_element_link_many(tee, queue_video1, video_convert, video_filter2, video_rate_raw, queue_video2, video_encoder, video_pasre,
								  queue_video3, muxer, NULL) ||
		!gst_element_link_many(audio_src, audio_level, audio_convert, audio_resample, audio_audiorate, audio_rate_caps,
							   queue_audio1, audio_encode, audio_faac_caps, queue_audio2, muxer, NULL)

		|| !gst_element_link_many(tee, queue_display1, video_scale, video_scale_caps, identity, video_rate, video_rate_caps, jpgenc, jpg_filesink, NULL))
	{
		g_error("Failed to link elementsses");
		return -2;
	}

	if (!gst_element_link_many(muxer, sink, NULL))
	{
		g_error("ASFSF");
		return -2;
	}

	loop = g_main_loop_new(NULL, FALSE);
	int data = 12;
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_signal_watch(bus);
	g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(message_cb), &context->pipeline_restart);
	g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(audio_cb), NULL);

	gst_object_unref(GST_OBJECT(bus));
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Starting loop");
	g_main_loop_run(loop);

	g_print("/////////////////////////////////End loop\n");

	gst_element_set_state(pipeline, GST_STATE_NULL);
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(pipeline);

	gst_object_unref(GST_OBJECT(muxer));
	gst_object_unref(GST_OBJECT(queue_audio1));
	gst_object_unref(GST_OBJECT(queue_audio2));
	gst_object_unref(GST_OBJECT(audio_src));
	gst_object_unref(GST_OBJECT(audio_rate_caps));
	gst_object_unref(GST_OBJECT(audio_faac_caps));
	gst_object_unref(GST_OBJECT(audio_convert));
	gst_object_unref(GST_OBJECT(audio_resample));
	gst_object_unref(GST_OBJECT(audio_audiorate));
	gst_object_unref(GST_OBJECT(audio_encode));
	gst_object_unref(GST_OBJECT(audio_level));
	gst_object_unref(GST_OBJECT(queue_video1));
	gst_object_unref(GST_OBJECT(queue_video2));
	gst_object_unref(GST_OBJECT(queue_video3));
	gst_object_unref(GST_OBJECT(videosrc));
	gst_object_unref(GST_OBJECT(video_pasre));
	gst_object_unref(GST_OBJECT(video_filter1));
	gst_object_unref(GST_OBJECT(video_filter2));
	gst_object_unref(GST_OBJECT(video_convert));
	gst_object_unref(GST_OBJECT(video_encoder));
	gst_object_unref(GST_OBJECT(video_scale));
	gst_object_unref(GST_OBJECT(video_rate));
	gst_object_unref(GST_OBJECT(jpgenc));
	gst_object_unref(GST_OBJECT(jpg_filesink));
	gst_object_unref(GST_OBJECT(queue_display1));
	gst_object_unref(GST_OBJECT(video_rate_caps));
	gst_object_unref(GST_OBJECT(video_scale_caps));
	gst_object_unref(GST_OBJECT(identity));
	gst_object_unref(GST_OBJECT(sink));
	gst_object_unref(GST_OBJECT(tee));

	return 0;
}

void *gst_tx_start(void *_context)
{

	Context *context = (Context *)_context;
	memset(&right_start_time, 0x0, sizeof(struct timeval));
	memset(&left_start_time, 0x0, sizeof(struct timeval));

	// SIGPIPE 시그널을 무시한다.
	signal(SIGPIPE, SIG_IGN);
	gst_init(NULL, NULL);

	pipeline = gst_pipeline_new(NULL);

	muxer = gst_element_factory_make("mpegtsmux", NULL);
	queue_audio1 = gst_element_factory_make("queue", NULL);
	queue_audio2 = gst_element_factory_make("queue", NULL);
	audio_src = gst_element_factory_make("alsasrc", NULL);
	audio_rate_caps = gst_element_factory_make("capsfilter", NULL);
	audio_faac_caps = gst_element_factory_make("capsfilter", NULL);
	audio_convert = gst_element_factory_make("audioconvert", NULL);
	audio_resample = gst_element_factory_make("audioresample", NULL);
	audio_audiorate = gst_element_factory_make("audiorate", NULL);
	audio_encode = gst_element_factory_make("faac", NULL);
	audio_level = gst_element_factory_make("level", "audio_level ");

	if (!audio_encode)
	{
		g_error("Audio Encoder is Failed");
	}

	if (!queue_audio1 || !queue_audio2 || !audio_src || !audio_rate_caps || !audio_faac_caps || !audio_convert || !audio_resample || !audio_audiorate)
	{
		g_error("Failed to Audio create elements");
		return -1;
	}
	switch (context->profileInfo->audioPath)
	{
	case HDMI:
		// g_object_set(audio_src, "device", "hw:1,2", NULL);
		g_object_set(audio_src, "device", "hw:2,0", NULL);
		break;
	case AUDIO_IN:
		g_object_set(audio_src, "device", "hw:1,0", NULL);

		break;
	}
	g_object_set(audio_level, "post-messages", TRUE, NULL);

	caps = gst_caps_from_string("audio/x-raw, rate=48000, channels=2");
	g_object_set(G_OBJECT(audio_rate_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("bitrate=128000 rate-control=2");
	g_object_set(G_OBJECT(audio_faac_caps), "caps", caps, NULL);
	gst_caps_unref(caps);

	queue_video1 = gst_element_factory_make("queue", NULL);
	queue_video2 = gst_element_factory_make("queue", NULL);
	queue_video3 = gst_element_factory_make("queue", NULL);
	videosrc = gst_element_factory_make("v4l2src", NULL);
	printf("context->profileInfo->codec %d\n", context->profileInfo->codec);
	switch (context->profileInfo->codec)
	{
	case HEVC:
	{
		video_pasre = gst_element_factory_make("h265parse", NULL);
		video_encoder = gst_element_factory_make("nvv4l2h265enc", NULL);
	}
	break;
	case H264:
	{
		video_pasre = gst_element_factory_make("h264parse", NULL);
		video_encoder = gst_element_factory_make("nvv4l2h264enc", NULL);
	}
	break;
	default:
		break;
	}
	video_filter1 = gst_element_factory_make("capsfilter", NULL);
	video_filter2 = gst_element_factory_make("capsfilter", NULL);
	video_convert = gst_element_factory_make("nvvidconv", NULL);
	jpegdec = gst_element_factory_make("nvjpegdec", NULL);
	x_raw = gst_element_factory_make("capsfilter", NULL);

	sink = gst_element_factory_make("mcts", NULL);

	if (!muxer)
	{
		g_error("MUX is Failed");
	}
	if (!pipeline || !videosrc || !video_filter1 || !video_filter2 || !video_encoder || !video_convert || !sink || !jpegdec || !muxer)
	{
		g_error("Failed to create elements");
		return -1;
	}

	// setup pipeline
	g_object_set(videosrc, "device", "/dev/video0", "io-mode", 2, NULL);
	;

	g_object_set(video_encoder, "insert-sps-pps", TRUE, NULL);
	g_object_set(video_encoder, "iframeinterval", context->profileInfo->fps, NULL);
	g_object_set(video_encoder, "profile", 1, NULL);
	g_object_set(video_encoder, "EnableTwopassCBR", TRUE, NULL);
	g_object_set(video_encoder, "control-rate", 1, NULL); // CBR
	g_object_set(video_encoder, "bitrate", context->profileInfo->bitrate * 1024, NULL);

	printf("###################gst-start %d\n", context->device_state);

	
		printf("server-ip %s\n", context->profileInfo->tlsInfo->mcts_ip);
		printf("server-port %d\n", context->profileInfo->tlsInfo->mcts_port);
		printf("streaming %d\n", context->device_state);

		printf("key %s\n", context->profileInfo->key);
		printf("name %s\n", context->profileInfo->name);

		g_object_set(sink, "server-ip", context->profileInfo->tlsInfo->mcts_ip, NULL);
		g_object_set(sink, "server-port", context->profileInfo->tlsInfo->mcts_port, NULL);
		g_object_set(sink, "streaming", START_STREAMING, NULL);
		g_object_set(sink, "uuid", context->profileInfo->key, NULL);
		g_object_set(sink, "device-name", context->profileInfo->name, NULL);
	

	// videio
	static char cap_str[256] = {
		0,
	};
	sprintf(cap_str, "image/jpeg, width=(int)%d, height=(int)%d,framerate=%d/1",
			context->profileInfo->width, context->profileInfo->height,context->profileInfo->fps);
	caps = gst_caps_from_string(cap_str);
	g_object_set(G_OBJECT(video_filter1), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("video/x-raw");
	g_object_set(G_OBJECT(x_raw), "caps", caps, NULL);
	gst_caps_unref(caps);

	caps = gst_caps_from_string("video/x-raw(memory:NVMM),format=(string)I420");
	g_object_set(G_OBJECT(video_filter2), "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add_many(GST_BIN(pipeline), videosrc, video_filter1, jpegdec, x_raw, queue_video1, video_convert, video_filter2, queue_video2, video_encoder, video_pasre,
					 queue_video3, muxer, sink,

					 audio_src, audio_level, audio_convert, audio_resample, audio_audiorate, audio_rate_caps,
					 queue_audio1, audio_encode, audio_faac_caps, queue_audio2, NULL);

	if (!gst_element_link_many(videosrc, video_filter1, jpegdec, x_raw, queue_video1, video_convert, video_filter2, queue_video2, video_encoder, video_pasre,
							   queue_video3, muxer, NULL) ||
		!gst_element_link_many(audio_src, audio_level, audio_convert, audio_resample, audio_audiorate, audio_rate_caps,
							   queue_audio1, audio_encode, audio_faac_caps, queue_audio2, muxer, NULL))
	{
		g_error("Failed to link elementsses");
		return -2;
	}

	if (!gst_element_link_many(muxer, sink, NULL))
	{
		g_error("ASFSF");
		return -2;
	}

	loop = g_main_loop_new(NULL, FALSE);

	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_signal_watch(bus);
	// g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(message_cb), NULL);

	gst_object_unref(GST_OBJECT(bus));

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Starting loop");
	g_main_loop_run(loop);
	return 0;
}