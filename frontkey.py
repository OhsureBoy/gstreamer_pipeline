#!/usr/bin/python3
import time
from g4l import Gpio
from loguru import logger
import sysv_ipc
from type_definitions import *
BUFF_SIZE = 16

global count
count = 0

class IoCtrl(Gpio):
    def __init__(self, num, name=None, evt=None):
        super().__init__(num)
        self.name = name
        self.setevent(evt)
        self.setisr(self.isr)
    
    def isr(self):
        try:
            global count   
            if count > 6:
                mq = sysv_ipc.MessageQueue(2345, sysv_ipc.IPC_CREAT)   
                

                if self.name == 'right':
                    mq.send('right\0', True, type = PRESS_RIGHT_BUTTON)

                if self.name == 'left':
                    mq.send('left\0', True, type = PRESS_LEFT_BUTTON)

                if self.name == 'down':
                    mq.send('down\0', True, type = PRESS_DOWN_BUTTON)

                if self.name == 'up':
                    mq.send('up\0', True, type = PRESS_UP_BUTTON)

                if self.name == 'center':
                    mq.send('center\0', True, type = PRESS_CENTER_BUTTON)

                if self.name == 'stream':
                    mq.send('stream\0', True, type = PRESS_STREAM_BUTTON)    
            logger.info(self.name)
            count += 1
            

        except sysv_ipc.ExistentialError:
            print("ERROR: message queue creation failed")

    def setevent(self, evt):
        self.evt = evt

    def getName(self):
        return self.name
    
        

class FrontKey:
    SW_MENU_A = 504
    SW_MENU_B = 505
    SW_MENU_C = 506
    SW_MENU_D = 507
    SW_MENU_CENTER = 508
    SW_STRM_CENTER = 509
    OLED_FR = 511

    def __init__(self):
        self.io = dict()
        self.io['left'] = IoCtrl(self.SW_MENU_A, name='left')  # A
        self.io['down'] = IoCtrl(self.SW_MENU_B, name='down')  # B
        self.io['right'] = IoCtrl(self.SW_MENU_C, name='right') # C
        self.io['up'] = IoCtrl(self.SW_MENU_D, name='up')    # D
        self.io['menuCenter'] = IoCtrl(self.SW_MENU_CENTER, name='center')
        self.io['strmCenter'] = IoCtrl(self.SW_STRM_CENTER, name='stream')
        self.io['oledfr'] = IoCtrl(self.OLED_FR, name='oledfr')

        for key, io in self.io.items():            
            io.direction(IoCtrl.IN)
            io.edge(IoCtrl.RISING)

            
if __name__ == '__main__':
    frontkey = FrontKey()

    print('poll start') 
        
    cnt = 0
    while True:
        print(f'poll {cnt}')    
        cnt += 1
        time.sleep(6)  
   
