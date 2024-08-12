import sys, os

sys.path.clear()
sys.path.append('C:/GitProject/WirelessMonitor/thirdTarget_I2CTool/config/AHT10/lib')
sys.path.append('C:/Path/miniconda3/')
sys.path.append('C:/Path/miniconda3/Lib')
sys.path.append('C:/Path/miniconda3/Lib/site-packages')
sys.path.append('C:/Path/miniconda3/Scripts')
sys.path.append('C:/Path/miniconda3/DLLs')
sys.path.append('C:/Path/miniconda3/Library/bin')
sys.path.append('C:/GitProject/WirelessMonitor/thirdTarget_I2CTool/config/AHT10/lib')
# os.chdir("C:/Path/miniconda3/Lib/site-packages")

import requests
import time

def WriteI2C(DevicWriteAddrT: str, DataAddrT: str, DataT: str, DataLenT: int):
    # 将参数转换为字符串
    # DevicWriteAddrT = f'0x{int(arg1):02X}'
    # DataAddrT = f'0x{int(arg2):02X}'
    #
    # # 将numpy数组转换为逗号分隔的字符串
    # DataT = '-'.join(f'0x{val:02X}' for val in arg3)
    #
    # DataLenT = str(arg4)

    # 构建URL
    url = f"http://127.0.0.1:19230/WriteI2C/{DevicWriteAddrT}/{DataAddrT}/{DataT}/{DataLenT}"

    # 发送请求
    response = requests.get(url)

    # 返回请求结果
    return response.text


def ReadI2C(DevicReadAddrT: str, DataAddrT: str, DataLenT: int):
    # 将参数转换为字符串
    # DevicWriteAddrT = f'0x{int(arg1):02X}'
    # DataAddrT = f'0x{int(arg2):02X}'
    #
    # # 将numpy数组转换为逗号分隔的字符串
    # DataT = '-'.join(f'0x{val:02X}' for val in arg3)
    #
    # DataLenT = str(arg4)

    # 构建URL
    url = f"http://127.0.0.1:19230/ReadI2C/{DevicReadAddrT}/{DataAddrT}/{DataLenT}"

    # 发送请求
    response = requests.get(url)

    # 返回请求结果
    return response.text


if __name__ == "__main__":
    print(ReadI2C("0x71", "0x00", 1))
