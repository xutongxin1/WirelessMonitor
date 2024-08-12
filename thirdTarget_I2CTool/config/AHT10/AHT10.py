import time

import I2CBase
class I2CWork():

    def __init__(self):
        print("ATH10 init")
        pass


    def Calibration(self):
        # print(a)
        I2CBase.WriteI2C("0x70", "0xe1", "0x08-0x00", 2)
        time.sleep(0.01)
        return str(I2CBase.ReadI2C("0x71", "0x00", 1))

    def Measurement(self):
        I2CBase.WriteI2C("0x70", "0xac", "0x33-0x00", 2)
        time.sleep(0.01)
        # print(a)

        rec=str(I2CBase.ReadI2C("0x71", "0x00", 1))

        databuff={0}
        databuff[0] = rec[1]
        databuff[1] = rec[2]
        databuff[2] = rec[3]
        databuff[3] = rec[4]
        databuff[4] = 0x00
        SRH = (databuff[0] << 12) + (databuff[1] << 4) + (databuff[2] >> 4)
        ST = ((databuff[2] & 0X0f) << 16) + (databuff[3] << 8) + (databuff[4])
        humidity = (int) (SRH * 100.0 / 1024 / 1024 + 0.5)
        temperature = ((int) (ST * 2000.0 / 1024 / 1024 + 0.5)) / 10.0 - 50
        return {"humidity":humidity,"temperature":temperature}

    def SoftReset(self):
        I2CBase.WriteI2C("0x70", "0xba", "0x00", 1)
        time.sleep(0.01)
        return str(I2CBase.ReadI2C("0x71", "0x00", 1))

if __name__ == "__main__":
    a = I2CWork()
    print(a.Read_0x00())
    pass