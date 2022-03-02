
#include <gst/gst.h>
void* menu_manager(void *context);
void initRootMenu( int sendMsgPid);
void channelControl(int sendMsgPid , int x,int y);
void viewMainMenu(int sendMsgPid, int d_y);
void viewTliveStudioMenu(int sendMsgPid, int d_y);
void viewEncoderMenu(int sendMsgPid, int d_y);
void viewSystemMenu(int sendMsgPid, int d_y);
void viewSettingMenu(int sendMsgPid, int d_y);
int menuLength(char* SubMenu[],int size);
void mainMenu_x_y_setting(int menuIndex);
void settingStatusMainMenu(int index);
void menuScrollIndex(int sendMsgPid, int maxLength, char* MenuControll[], int d_y, char* subMenuName);
int currentMenuY(int d_y, int currentIndex);
void savePreviousData();
void loadProfileData(int sendMsgPid);
void *netwrok_manager(void *_context);
void *http_send_device_state_manager(void *_context);
void *gst_start(void *_context);
void *gst_tx_start(void *_context);
void root_menu_src_data();
gboolean
message_cb(GstBus *bus, GstMessage *message, gpointer user_data);
GstElement  *sink, *video_encoder;
GstBus *bus;