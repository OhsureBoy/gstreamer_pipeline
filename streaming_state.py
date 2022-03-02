#!/usr/bin/python3

from time import sleep, time
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from ssd1362 import Ssd1362 
from copy import copy
import argparse
import sysv_ipc
import struct

BUFF_SIZE = 16

from type_definitions import *
from image_definitions import *
width = 256
height = 64
class streaming_state:
    def __init__(self, t1, t2, t3, t4):
        self.background_img = Image.open(image_default_path).resize((width, height)).convert("L")
        # self.preview_img = Image.open(preview_img_path).resize((width, height)).convert("L").resize((88,63))
        self.not_preview_img = Image.open(not_connect_path).resize((width, height)).convert("L").resize((88,63))
        self.choice_img = Image.open(choice_img_path).resize((width, height)).convert("L").resize((133,14))

        self.text_First = t1
        self.text_Second = t2
        self.text_Third = t3
        self.text_Fourth = t4
        self.fontSize = 11
        self.choiceMenu = -1;

        self.oled = Ssd1362(spibus=0, spidev=0, io_dc=38)

    def setChoiceMenu(self, number):
        self.choiceMenu = number;

    def drawChoiceNumber(self,number):

        self.choiceMenu = number;
        white_crop = self.choice_img
        black_crop = Image.open(image_default_path).resize((width, height)).convert("L").resize((133,14))

        if number == 0:
            self.background_img.paste(white_crop,(105,4))
            self.background_img.paste(black_crop,(105,18))
            self.background_img.paste(black_crop,(105,32))
            self.background_img.paste(black_crop,(105,46))
        elif number == 1:
            self.background_img.paste(black_crop,(105,4))
            self.background_img.paste(white_crop,(105,18))
            self.background_img.paste(black_crop,(105,32))
            self.background_img.paste(black_crop,(105,46))
        elif number == 2:
            self.background_img.paste(black_crop,(105,4))
            self.background_img.paste(black_crop,(105,18))
            self.background_img.paste(white_crop,(105,32))
            self.background_img.paste(black_crop,(105,46))
        elif number == 3:
            self.background_img.paste(black_crop,(105,4))
            self.background_img.paste(black_crop,(105,18))
            self.background_img.paste(black_crop,(105,32))
            self.background_img.paste(white_crop,(105,46))
        else:
            self.background_img.paste(black_crop,(105,4))
            self.background_img.paste(black_crop,(105,18))
            self.background_img.paste(black_crop,(105,32))
            self.background_img.paste(black_crop,(105,46))

    def drawTextFirst(self,draw):
        x = 105
        y = 4
        font = ImageFont.truetype(font_path, self.fontSize)
          
        if self.choiceMenu == 0:
            draw.text((x,y),self.text_First, (0) ,font = font)
        else:
            draw.text((x,y),self.text_First, (255) ,font = font)

    def drawTextSecond(self,draw):
        x = 105
        y = 18
        font = ImageFont.truetype(font_path, self.fontSize)

        if self.choiceMenu == 1:
            draw.text((x,y),self.text_Second, (0) ,font = font)
        else:
            draw.text((x,y),self.text_Second, (255) ,font = font)
    
    def drawTextThird(self,draw):
        x = 105
        y = 32
        font = ImageFont.truetype(font_path, self.fontSize)

        if self.choiceMenu == 2:
            draw.text((x,y),self.text_Third, (0) ,font = font)
        else:
            draw.text((x,y),self.text_Third, (255) ,font = font)
    
    def drawTextFourth(self,draw):
        x = 105
        y = 46
        font = ImageFont.truetype(font_path, self.fontSize)
        if self.choiceMenu == 3:
            draw.text((x,y),self.text_Fourth, (0) ,font = font)
        else:
            draw.text((x,y),self.text_Fourth, (255) ,font = font)

    def clearAllText(self):
        
        black_crop = Image.open(image_default_path).resize((width, height)).convert("L").resize((133,14))
        
        self.background_img.paste(black_crop,(105,4))
        self.background_img.paste(black_crop,(105,18))
        self.background_img.paste(black_crop,(105,32))
        self.background_img.paste(black_crop,(105,46))
        
        self.text_First = ""
        self.text_Second = ""
        self.text_Third = ""
        self.text_Fourth = ""
    
        


    def drawTextConnectTime(self,draw , connectTime):
        x = 25
        y = 52
        font = ImageFont.truetype(font_path, 10)
    
        draw.text((x,y),connectTime,(255),font = font)
    
    def setDrawTextFirst(self,text_First):
        self.text_First = text_First
        

    def setDrawTextSecond(self,text_Second):
        self.text_Second = text_Second
        

    def setDrawTextThird(self,text_Third):
        self.text_Third = text_Third

    def setDrawTextFourth(self,text_Fourth):
        self.text_Fourth = text_Fourth


    def setLeftAudioLevel(self, audioLevel):
        
        if audioLevel == 0:
            audioLevelMeter = Image.open(image_audio_level_0).resize((5,63))
        elif audioLevel == 1:
            audioLevelMeter = Image.open(image_audio_level_1).resize((5,63))
        elif audioLevel == 2:
            audioLevelMeter = Image.open(image_audio_level_2).resize((5,63))
        elif audioLevel == 3:        
            audioLevelMeter = Image.open(image_audio_level_3).resize((5,63))    
        elif audioLevel == 4:
            audioLevelMeter = Image.open(image_audio_level_4).resize((5,63))
        elif audioLevel == 5:
            audioLevelMeter = Image.open(image_audio_level_5).resize((5,63))
        elif audioLevel == 6:
            audioLevelMeter = Image.open(image_audio_level_6).resize((5,63))
        elif audioLevel == 7:
            audioLevelMeter = Image.open(image_audio_level_7).resize((5,63))
        elif audioLevel == 8:
            audioLevelMeter = Image.open(image_audio_level_8).resize((5,63))
        elif audioLevel == 9:
            audioLevelMeter = Image.open(image_audio_level_9).resize((5,63))
        elif audioLevel == 10:
            audioLevelMeter = Image.open(image_audio_level_10).resize((5,63))
        elif audioLevel == 11:
            audioLevelMeter = Image.open(image_audio_level_11).resize((5,63))
        elif audioLevel == 12:
            audioLevelMeter = Image.open(image_audio_level_12).resize((5,63))
        elif audioLevel == 13:
            audioLevelMeter = Image.open(image_audio_level_13).resize((5,63))
        
        
        self.background_img.paste(audioLevelMeter,(90,1))
        

    def setRightAudioLevel(self, audioLevel):

        if audioLevel == 0:
            audioLevelMeter = Image.open(image_audio_level_0).resize((5,63))
        elif audioLevel == 1:
            audioLevelMeter = Image.open(image_audio_level_1).resize((5,63))
        elif audioLevel == 2:
            audioLevelMeter = Image.open(image_audio_level_2).resize((5,63))
        elif audioLevel == 3:        
            audioLevelMeter = Image.open(image_audio_level_3).resize((5,63))   
        elif audioLevel == 4:
            audioLevelMeter = Image.open(image_audio_level_4).resize((5,63))
        elif audioLevel == 5:
            audioLevelMeter = Image.open(image_audio_level_5).resize((5,63))
        elif audioLevel == 6:
            audioLevelMeter = Image.open(image_audio_level_6).resize((5,63))
        elif audioLevel == 7:
            audioLevelMeter = Image.open(image_audio_level_7).resize((5,63))
        elif audioLevel == 8:
            audioLevelMeter = Image.open(image_audio_level_8).resize((5,63))
        elif audioLevel == 9:
            audioLevelMeter = Image.open(image_audio_level_9).resize((5,63))
        elif audioLevel == 10:
            audioLevelMeter = Image.open(image_audio_level_10).resize((5,63))
        elif audioLevel == 11:
            audioLevelMeter = Image.open(image_audio_level_11).resize((5,63))
        elif audioLevel == 12:
            audioLevelMeter = Image.open(image_audio_level_12).resize((5,63))
        elif audioLevel == 13:
            audioLevelMeter = Image.open(image_audio_level_13).resize((5,63))

        self.background_img.paste(audioLevelMeter,(95,1))
    
    def drawPreview(self,connect_state):
        
        preview_img = Image.open(preview_img_path).resize((width, height)).convert("L").resize((88,63))
        if connect_state == True:   
            self.background_img.paste(preview_img,(0,1))
        else:
            self.background_img.paste(self.not_preview_img,(0,1))
    
    def usbNetworkStateFirst(self,state):
        if state == True:
            usbNetwork = Image.open(image_usb_network_on)
        else:
            usbNetwork = Image.open(image_usb_network_off)
        
        self.background_img.paste(usbNetwork,(240,3))

    def usbNetworkStateSecond(self,state):
        if state == True:
            usbNetwork = Image.open(image_usb_network_on)
        else:
            usbNetwork = Image.open(image_usb_network_off)
        
        self.background_img.paste(usbNetwork,(240,13))
        
    def usbNetworkStateThird(self,state):
        if state == True:
            usbNetwork = Image.open(image_usb_network_on)
        else:
            usbNetwork = Image.open(image_usb_network_off)
        
        self.background_img.paste(usbNetwork,(240,23))
        
    def usbNetworkStateFourth(self,state):
        if state == True:
            usbNetwork = Image.open(image_usb_network_on)
        else:
            usbNetwork = Image.open(image_usb_network_off)
        
        self.background_img.paste(usbNetwork,(240,33))

    def usbNetworkStateFifth(self,state):
        if state == True:
            usbNetwork = Image.open(image_usb_network_on)
        else:
            usbNetwork = Image.open(image_usb_network_off)
        
        self.background_img.paste(usbNetwork,(240,43))
    
    def lanNetworkState(self,state):
        if state == True:
            usbNetwork = Image.open(image_lan_network_on)
        else:
            usbNetwork = Image.open(image_lan_network_off)
        
        self.background_img.paste(usbNetwork,(240,53))
    
    def maintainText(self,):

        streaming.drawTextFirst(draw)
        streaming.drawTextSecond(draw)
        streaming.drawTextThird(draw)
        streaming.drawTextFourth(draw)

if __name__ == "__main__":

    # parser = argparse.ArgumentParser()
    # parser.add_argument("-t1", action='store', dest="text_First", type=str, help="enter message")
    # parser.add_argument("-t2", action='store', dest="text_Second", type=str, help="enter message")
    # parser.add_argument("-t3", action='store', dest="text_Third",  type=str, help="enter message")
    # parser.add_argument("-t4", action='store', dest="text_Fourth", type=str, help="enter message")
    # args = parser.parse_args()

    streaming = streaming_state("", "", "", "");
    try:

        mq = sysv_ipc.MessageQueue(1234, sysv_ipc.IPC_CREAT)
   
        while True:
            try:
                img = copy(streaming.background_img)    
                draw = ImageDraw.Draw(img)

                message, mtype = mq.receive()
                # print("*** New message received ***")
                # print(f"Raw message: {message}")

                if mtype == SET_LEFT_AUDIO_LEVEL:
                    level = struct.unpack("i", message[:4])
                    streaming.setLeftAudioLevel(level[0])
                    streaming.maintainText()
            
                if mtype == SET_RIGHT_AUDIO_LEVEL:
                    level = struct.unpack("i", message[:4])
                    streaming.setRightAudioLevel(level[0])
                    streaming.maintainText()
            
                if mtype == SET_DRAW_TEXT_FIRST:
                    txt = message.decode()
                    streaming.setDrawTextFirst(txt)
                    streaming.maintainText()
            
                if mtype == SET_DRAW_TEXT_SECOND:
                    txt = message.decode()
                    streaming.setDrawTextSecond(txt)
                    streaming.maintainText()
            
                if mtype == SET_DRAW_TEXT_THIRD:
                    txt = message.decode()
                    streaming.setDrawTextThird(txt)
                    streaming.maintainText()
            
                if mtype == SET_DRAW_TEXT_FOURTH:
                    txt = message.decode()
                    streaming.setDrawTextFourth(txt)
                    streaming.maintainText()
            
                if mtype == SET_USB_NETWORK_STATE_FIRST:
                    state = struct.unpack("?", message[:1])
                    streaming.usbNetworkStateFirst(state[0])
                    streaming.maintainText()
            
                if mtype == SET_USB_NETWORK_STATE_SECOND:
                    state = struct.unpack("?", message[:1])
                    streaming.usbNetworkStateSecond(state[0])
                    streaming.maintainText()

                if mtype == SET_USB_NETWORK_STATE_THIRD:
                    state = struct.unpack("?", message[:1])
                    streaming.usbNetworkStateThird(state[0])
                    streaming.maintainText()

                if mtype == SET_USB_NETWORK_STATE_FOURTH:
                    state = struct.unpack("?", message[:1])
                    streaming.usbNetworkStateFourth(state[0])
                    streaming.maintainText()

                if mtype == SET_USB_NETWORK_STATE_FIFTH:
                    state = struct.unpack("?", message[:1])
                    streaming.usbNetworkStateFifth(state[0])
                    streaming.maintainText()

                if mtype == SET_LAN_NETWORK_STATE:
                    state = struct.unpack("?", message[:1])
                    streaming.lanNetworkState(state[0])
                    streaming.maintainText()
            
                if mtype == SET_DRAW_TEXT_CONNECT_TIME:
                    connectTime = message.decode()
                    streaming.drawTextConnectTime(draw , connectTime)
                    streaming.maintainText()

                if mtype == SET_CHOICE_TEXT_NUMBER:
                    number = struct.unpack("i", message[:4])
                    streaming.drawChoiceNumber(number[0])
                    streaming.maintainText()

                if mtype == SET_DRAW_PREVIEW:
                    state = struct.unpack("?", message[:1])
                    streaming.drawPreview(state[0])
                    streaming.maintainText()

                if mtype == SET_CLEAR_ALL_TEXT:
                    streaming.clearAllText()
                    streaming.maintainText()
            except:
                print("OO")

            frame = np.asarray(img, dtype=np.uint8)
            streaming.oled.loadframe(frame)
            streaming.oled.show(15)

    except sysv_ipc.ExistentialError:
        print("ERROR: message queue creation failed")
