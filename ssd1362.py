#!/usr/bin/python3
from enum import Enum
import os, sys
from time import sleep, time

import numpy as np
from spidev import SpiDev

from g4l import Gpio

class Reg(Enum):
    # reg addr, parameter length
    ColAddr = 0x15 ,2
    RowAddr = 0x75, 2
    Contrast = 0x81, 1
    Remap = 0xa0, 1
    DisplayStartLine = 0xa1, 1
    DisplayOffset = 0xa2, 1
    VirtualScrollArea = 0xa3, 2
    DisplayMode_Normal = 0xa4, 0 
    DisplayMode_AllOn = 0xa5, 0 
    DisplayMode_AllOff = 0xa6, 0 
    DisplayMode_Inverse = 0xa7, 0 
    MuxRatio = 0xa8, 1 
    FunctionSel = 0xab, 1
    IrefSelection = 0xad, 1
    DisplayOff = 0xae, 0
    DisplayOn = 0xaf, 0
    PhaseLength = 0xb1, 1
    FrontClockDivOscFreq = 0xb3, 1
    GPIO = 0xb5, 1
    SecondPrechargePeriod = 0xb6, 1
    GrayScaleTable = 0xb8, 15
    LinearLUT = 0xb9, 0
    PreChargeVoltage = 0xbc, 1
    PreChargeVoltageCapacitor = 0xbd, 1
    Vcomh = 0xbe, 1
    CommandLock = 0xfd, 1
    FadeInOutBlink = 0x23, 1

class Error(Exception):
    """Exception raised for errors in the spi.

    Attributes:
        expression -- spi expression in which the error occurred
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message

class LengError(Error):
    pass
class TypeError(Error):
    pass


class Ssd1362:
    width = 256
    height = 64
    pixelram = width * height

    def __init__(self, spibus=0, spidev=0, io_dc=38):
        self.io_dc = Gpio(io_dc)        
        self.io_dc.direction(Gpio.OUT)

        self.spi = SpiDev()
        self.spi.open(spibus, spidev)
        #self.spi.max_speed_hz = 1400000
        self.spi.max_speed_hz = 1000000000
        self.spi.cshigh = False
        self.spi.bits_per_word = 8
        self.spi.no_cs = False
        self.spi.mode = 3
        self.spi.lsbfirst = False
        
        self.buf = []
        self.array = np.zeros((self.height,self.width), dtype=int)
        
        for i in range(self.pixelram//2):
            self.buf.append(0)
        
        self.init_oled()
        
    def init_oled(self):
        self._cmd(Reg.CommandLock, [0x12])
        self._cmd(Reg.DisplayOff)
        
        self._cmd(Reg.ColAddr, [0, 0x7f])
        self._cmd(Reg.RowAddr, [0, 0x3f])
        
        self._cmd(Reg.Contrast, [0x87])
        self._cmd(Reg.Remap, [0x53])
        self._cmd(Reg.DisplayStartLine, [0])
        self._cmd(Reg.DisplayOffset, [0])
        self._cmd(Reg.DisplayMode_Normal)
        self._cmd(Reg.MuxRatio, [0x3f])

        self._cmd(Reg.FunctionSel, [1])
        self._cmd(Reg.IrefSelection, [0x9e])
        self._cmd(Reg.PhaseLength, [0x11])

        self._cmd(Reg.FrontClockDivOscFreq, [0xf0])
        #self._cmd(Reg.SecondPrechargePeriod, [0x04])
        self._cmd(Reg.PreChargeVoltage, [0x04])
        #self._cmd(Reg.PreChargeVoltageCapacitor, [0x01])
        self._cmd(Reg.Vcomh, [0x05])

        #self._cmd(Reg.FadeInOutBlink, [0x30])
        self._cmd(Reg.FadeInOutBlink, [0x00])
        self._cmd(Reg.LinearLUT)
        self._cmd(Reg.DisplayOn)
        sleep(0.1)

    def _cmd(self, reg, val=None):
        addr, leng = reg.value

        if val is not None and len(val) != leng:
            raise LengError(f'cmd length : {len(val)} is not incorrect. {leng} is correct.')

        buf = [addr]
        if isinstance(val, list):
            for v in val:
                buf.append(v)
        elif leng > 0:
            raise TypeError(f'{type(val)} is not available. list type is correct')

        self.io_dc.output('0')
        self.spi.xfer2(buf)

    def show(self, gray_level=15):
        if gray_level > 15:
            gray_level = 15
        if gray_level < 0:
            gray_level = 1
        level = 16-gray_level
        self.array = self.array // level
        arr = self.array.reshape((1,self.width*self.height))[0]
        buf = (((arr[0::2]>>4)&0x0f) | (arr[1::2]&0xf0)).tolist()

        self.io_dc.output('1')
        self.spi.xfer3(buf[0:4096])
        self.spi.xfer3(buf[4096:8192])
            
    def loadframe(self, buf):
        self.array[:self.height, :self.width] = buf[::-1,:self.width]
