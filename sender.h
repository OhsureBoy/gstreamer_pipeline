#define BUFF_SIZE 1024
#include <stdbool.h>
#include "type_definitions.h"


typedef struct {
	long  data_type;
	char  data_buff[BUFF_SIZE];
} t_data;

void setDrawTextFirst(int msqid ,char* text);
void setDrawTextSecond(int msqid ,char* text);
void setDrawTextThird(int msqid ,char* text);
void setDrawTextFourth(int msqid ,char* text);

void updateUsbNetworkState(int index, int msqid, bool status);
void updateDrawText(int index, int msqid, char* text);
void updateStereoAudioLevel(int index, int msqid, int level);

void setLeftAudioLevel(int msqid, int audiolevel);
void setRightAudioLevel(int msqid, int audiolevel);


void apllicationToDrawLED(int sendMsgPid, char* selectElement, int index,  char* value );

void usbNetworkStateFirst(int msqid, bool state);
void usbNetworkStateSecond(int msqid, bool state);
void usbNetworkStateThird( int msqid, bool state);
void usbNetworkStateFourth( int msqid, bool state);
void usbNetworkStateFifth(int msqid, bool state);
void lanNetworkState(int msqid, bool state);

void setChoiceTextNumber(int msqid, int number);

void drawPreview(int msqid, bool state);