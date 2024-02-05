class I2CWork():
    # MPU6050 I2C Python translation from C

    # Constants
    MPU6050_ADDR = 0xD0
    SMPLRT_DIV_REG = 0x19
    GYRO_CONFIG_REG = 0x1B
    ACCEL_CONFIG_REG = 0x1C
    ACCEL_XOUT_H_REG = 0x3B
    TEMP_OUT_H_REG = 0x41
    GYRO_XOUT_H_REG = 0x43
    PWR_MGMT_1_REG = 0x6B
    WHO_AM_I_REG = 0x75

    # Global variables for sensor readings
    Accel_X_RAW = 0
    Accel_Y_RAW = 0
    Accel_Z_RAW = 0
    Gyro_X_RAW = 0
    Gyro_Y_RAW = 0
    Gyro_Z_RAW = 0
    Temp_RAW = 0

    def __init__(self):
        print("MPU6050 init")
        pass

    def fuction_28(self, a):
        print(a)
        return "hello"

    def fuction_3B(self, Rec_Data):
        # Read accelerometer data
        Rec_Data = bytes(Rec_Data, encoding='utf-8')
        Accel_X_RAW = int.from_bytes(Rec_Data[0:2], 'big', signed=True)
        Accel_Y_RAW = int.from_bytes(Rec_Data[2:4], 'big', signed=True)
        Accel_Z_RAW = int.from_bytes(Rec_Data[4:6], 'big', signed=True)
        Ax = Accel_X_RAW / 16384.0
        Ay = Accel_Y_RAW / 16384.0
        Az = Accel_Z_RAW / 16384.0
        return "x:" + str(Ax) + ",y:" + str(Ay) + ",z:" + str(Az)

    def fuction_43(self, Rec_Data):
        # Read gyroscope data
        Rec_Data = bytes(Rec_Data, encoding='utf-8')
        Gyro_X_RAW = int.from_bytes(Rec_Data[0:2], 'big', signed=True)
        Gyro_Y_RAW = int.from_bytes(Rec_Data[2:4], 'big', signed=True)
        Gyro_Z_RAW = int.from_bytes(Rec_Data[4:6], 'big', signed=True)
        Gx = Gyro_X_RAW / 131.0
        Gy = Gyro_Y_RAW / 131.0
        Gz = Gyro_Z_RAW / 131.0
        return "x:" + str(Gx) + ",y:" + str(Gy) + ",z:" + str(Gz)

    def fuction_41(self, Rec_Data):
        # Read temperature data

        Temp_RAW = int.from_bytes(Rec_Data, 'big', signed=True)
        Temp = 36.53 + Temp_RAW / 340.0
        return str(Temp)

