////////////////////////// main_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "sender.h"


void updateDrawText(int index, int msqid, char* text) {
	switch(index) {
		case 0:
			setDrawTextFirst(msqid,text);
		break;

		case 1:
			setDrawTextSecond(msqid,text);
		break;

		case 2:
			setDrawTextThird(msqid,text);
		break;

		case 3:
			setDrawTextFourth(msqid,text);
		break;
	}

}

void apllicationToDrawLED(int sendMsgPid, char* selectElement, int index,  char* value )
{	
	printf("%s  %d  %s",selectElement,index,value);
	if(strcmp(selectElement,"network") == 0) {
		int state = atoi(value);
		bool status = false;
		if(state == 0)
			status = false;
		else if(state == 1)
			status = true;
		updateUsbNetworkState(index,sendMsgPid,status);
	}

	else if(strcmp(selectElement,"text") == 0) {
		updateDrawText(index,sendMsgPid,value);
	}

	else if(strcmp(selectElement,"audio") == 0) {
		int level = atoi(value);
		updateStereoAudioLevel(index,sendMsgPid,level);
	}

	else if(strcmp(selectElement,"preview") == 0){
		int state = atoi(value);
		bool status = false;
		if(state == 0)
			status = false;
		else if(state == 1)
			status = true;
		drawPreview(sendMsgPid,status);
	}

	drawPreview(sendMsgPid,false);
}

void updateStereoAudioLevel(int index, int msqid, int level) {
	switch(index) {
		case 0:
			setLeftAudioLevel(msqid, level);
		break;

		case 1:
			setRightAudioLevel(msqid, level);
		break;
	}
}

void setDrawTextFirst( int msqid ,char* text) {

	t_data data;
	data.data_type = SET_DRAW_TEXT_FIRST;
	memset(data.data_buff,0x0,BUFF_SIZE);
	sprintf( data.data_buff, "%s", text);
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setDrawTextSecond(int msqid ,char* text) {
	t_data data;
	data.data_type = SET_DRAW_TEXT_SECOND;
	memset(data.data_buff,0x0,BUFF_SIZE);
	sprintf( data.data_buff, "%s", text);
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setDrawTextThird(int msqid ,char* text) {
	t_data data;
	data.data_type = SET_DRAW_TEXT_THIRD;
	memset(data.data_buff,0x0,BUFF_SIZE);
	sprintf( data.data_buff, "%s", text);
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setDrawTextFourth(int msqid ,char* text) {
	t_data data;
	data.data_type = SET_DRAW_TEXT_FOURTH;
	memset(data.data_buff,0x0,BUFF_SIZE);
	sprintf( data.data_buff, "%s", text);
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setLeftAudioLevel(int msqid, int audiolevel) {
	t_data data;
	data.data_type = SET_LEFT_AUDIO_LEVEL;
	int level = audiolevel;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &level, sizeof(int));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setRightAudioLevel(int msqid, int audiolevel) {
	t_data data;
	data.data_type = SET_RIGHT_AUDIO_LEVEL;
	int level = audiolevel;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &level, sizeof(int));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void setChoiceTextNumber( int msqid, int number) {

	t_data data;
	data.data_type = SET_CHOICE_TEXT_NUMBER;
	int level = number;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &level, sizeof(int));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}
void updateUsbNetworkState(int index, int msqid, bool status)
{
	switch(index)
	{
		case 0:
			usbNetworkStateFirst(msqid, status);
		break;

		case 1:
			usbNetworkStateSecond(msqid, status);
		break;

		case 2:
			usbNetworkStateThird(msqid, status);
		break;

		case 3:
			usbNetworkStateFourth(msqid, status);
		break;

		case 4:
			usbNetworkStateFifth(msqid, status);
		break;

		case 5:
			lanNetworkState(msqid, status);
		break;
	}
}
void usbNetworkStateFirst(int msqid, bool state) {
	t_data data;
	data.data_type = SET_USB_NETWORK_STATE_FIRST;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void usbNetworkStateSecond(int msqid, bool state) {
	t_data data;
	data.data_type = SET_USB_NETWORK_STATE_SECOND;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void usbNetworkStateThird(int msqid, bool state) {
	t_data data;
	data.data_type = SET_USB_NETWORK_STATE_THIRD;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void usbNetworkStateFourth(int msqid, bool state) {
	t_data data;
	data.data_type = SET_USB_NETWORK_STATE_FOURTH;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void usbNetworkStateFifth(int msqid, bool state) {
	t_data data;
	data.data_type = SET_USB_NETWORK_STATE_FIFTH;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void lanNetworkState(int msqid, bool state) {
	t_data data;
	data.data_type = SET_LAN_NETWORK_STATE;
	bool usbNetworkState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &usbNetworkState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}

void drawPreview(int msqid, bool state) {
	t_data data;
	data.data_type = SET_DRAW_PREVIEW;
	bool previewState = state;
	memset(data.data_buff,0x0,BUFF_SIZE);
	memcpy(data.data_buff, &previewState, sizeof(bool));
	if ( -1 == msgsnd( msqid, &data, sizeof( t_data) - sizeof( long), 0))
	{
		perror( "msgsnd() failed");
		exit( 1);
	}
}