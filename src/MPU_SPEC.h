/*
 * MPU_SPEC.h
 *
 *  Created on: Sep 13, 2020
 *      Author: Bruno Otávio
 */

#ifndef INC_MPU_SPEC_H_
#define INC_MPU_SPEC_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

//	Global definition

#define USE_SI 					1				//International system of units will be used or not

typedef struct {
	uint8_t register_address;			//holds the i2c address of the internal register
	uint8_t data_cmd;					//holds information that will be send to make some register configuration, each mpu register will have
}MPU_REGISTER;							//a data byte to save the actual information at the register mpu in mcu code.

/*
 * Possible axis that can be disabled
 */
typedef enum{
	 DISABLE_ALL_AXIS = 0x07,
	 DISABLE_X_Y_AXIS =	0x06,
	 DISABLE_X_Z_AXIS =	0x05,
	 DISABLE_X_AXIS	  =	0x04,
	 DISABLE_Y_Z_AXIS =	0x03,
	 DISABLE_Y_AXIS	  =	0x02,
	 DISABLE_Z_AXIS	  =	0x01,
	 NOT_DISABLE      = 0x00
}MPU_DISABLE_AXIS;

#define USE_ADDR1 1
#define USE_ADDR2 2

#define ACCELGYRO_ADDR_1 0x68
#define ACCELGYRO_ADDR_2 0x69

#define USE_I2C1 1
#define USE_I2C2 2
#define USE_I2C3 3


typedef enum{

	X_AXIS =  1,
	Y_AXIS =  2,
	Z_AXIS =  3
}AXIS;

typedef enum{

 DLPF_CFGX	=	0,
 DLPF_CFG0	=	0,
 DLPF_CFG1	=	1,
 DLPF_CFG2	=	2,
 DLPF_CFG3	=	3,
 DLPF_CFG4	=	4,
 DLPF_CFG5	=	5,
 DLPF_CFG6	=	6,
 DLPF_CFG7	=	7
}DLPF;

typedef enum{
	RESET_ALL 			= 0x7,
	RESET_GYRO_ACCEL	= 0x6,
	RESET_GYRO_TEMP		= 0x5,
	RESET_GYRO			= 0x4,
	RESET_ACCEL_TEMP	= 0x3,
	RESET_ACCEL			= 0x2,
	RESET_TEMP			= 0x1
}RESET_SENSOR_SIGNAL_PATH;

I2C_HandleTypeDef mpu_i2c_comm;						//Holds the i2c peripheral registers used by the mcu to connect with MPU

//When the IMU comes, it contain the OTP values of the Accel factory trim. (Application note)
//float accelx_factory_trim;
//float accely_factory_trim;
//float accelz_factory_trim;

/*
 * These values were found previously , if you want to change them, just uncomment the lines 49, 50, 51, To find your MPU trim factory
*/

#define accelx_factory_trim	 1.72753906
#define accely_factory_trim	 29.5117188
#define accelz_factory_trim  5.14160156

/*
 *	All of accelerometer specific definition will be placed at this place
 *
 */

#define SI_ACCELERATION 		9.807				//acceleration in international system of units 1g = 9.8 m/s^2

/*
 * Possible configuration of low pass filter register
 */

#define ACCEL_FCHOICE0_b			1
#define ACCEL_FCHOICE1_b			0

/*
 * Specify all of sensitivity available by the MPU-9250 accelerometer
 */
typedef enum{
	 ACCEL_FULL_SCALE_2g   =	0b00,
	 ACCEL_FULL_SCALE_4g   =	0b01,
	 ACCEL_FULL_SCALE_8g   = 	0b10,
	 ACCEL_FULL_SCALE_16g  =	0b11
}MPU_ACCEL_SCALE;

uint16_t accel_sensitivity_used;			  //Currently accelerometer sensitivity used by the MPU


/*
 * 	 All of gyroscope specific definition will be placed at this place
 */

#define GYRO_FCHOICE00			0b11
#define GYRO_FCHOICE01			0b10
#define GYRO_FCHOICE11			0b00
typedef enum{
	 GYRO_FULL_SCALE_250dps  =  0b00,
	 GYRO_FULL_SCALE_500dps	 = 	0b01,
	 GYRO_FULL_SCALE_1000dps =  0b10,
	 GYRO_FULL_SCALE_2000dps =  0b11
}MPU_GYRO_SCALE;

float gyro_sensitivity_used;			//Currently gyroscope sensitivity used by the MPU

/*
 * 	All of MPU fifo specific definition will be placed at this place
 *
 */
#define FIFO_MODE_NOT_OVERRIDE		1					//new data will not replace the oldest
#define FIFO_MODE_OVERRIDE			0					//new data will replace oldest data

#define FIFO_ENABLE_TEMP		1
#define FIFO_DISABLE_TEMP		0

#define FIFO_ENABLE_GYRO_X		1
#define FIFO_DISABLE_GYRO_X		0

#define FIFO_ENABLE_GYRO_Y		1
#define FIFO_DISABLE_GYRO_Y		0

#define FIFO_ENABLE_GYRO_Z		1
#define FIFO_DISABLE_GYRO_Z 	0

#define FIFO_ENABLE_ACCEL		1
#define FIFO_DISABLE_ACCEL		0

/*
 *
 * All of MPU magnetometer AK8963 specific definition will be placed at this place
 *
 */
#define AK8963_ADDR 0x0C
#define AK8963_SENSITIVITY 0.1499023


typedef enum{

	MAG_POWER_DOWN 		         = 0b0000,
	MAG_SINGLE_MEASUREMENT 		 = 0b0001,		/* Sensor is measured for one time and data is output. Transits to power-down mode.	*/
	MAG_CONTINUOUS_MEASUREMENT1  = 0b0010,		/* Sensor is measured periodically in 8Hz. */
	MAG_CONTINUOUS_MEASUREMENT2	 = 0b0110,		/* Sensor is measured periodically in 100Hz. */
	MAG_EXTERNAL_TRIGGER		 = 0b0100,		/* Sesor in measured for one time by external trigger. */
	MAG_SELF_TEST 				 = 0b1000,		/* Sensor is self-tested and the result is output. */
	MAG_FUSE_ROOM				 = 0b1111		/* Turn on the circuit needed to read out Fuse ROM. */
}MPU_MAG_OPMODE;

typedef enum{

	_14_BIT,
	_16_BIT
}MPU_MAG_OUTPUT_SETTING;

float magx_Adj, magy_Adj, magz_Adj;


/*
 * MPU-9250 available registers
 *
*/
MPU_REGISTER SELF_TEST_X_GYRO;
MPU_REGISTER SELF_TEST_Y_GYRO;
MPU_REGISTER SELF_TEST_Z_GYRO;
MPU_REGISTER SELF_TEST_X_ACCEL;
MPU_REGISTER SELF_TEST_Y_ACCEL;
MPU_REGISTER SELF_TEST_Z_ACCEL;
MPU_REGISTER XG_OFFSET_H;
MPU_REGISTER XG_OFFSET_L;
MPU_REGISTER YG_OFFSET_H;
MPU_REGISTER YG_OFFSET_L;
MPU_REGISTER ZG_OFFSET_H;
MPU_REGISTER ZG_OFFSET_L;
MPU_REGISTER SMPLRT_DIV;
MPU_REGISTER CONFIG;
MPU_REGISTER GYRO_CONFIG;
MPU_REGISTER ACCEL_CONFIG;
MPU_REGISTER ACCEL_CONFIG2;
MPU_REGISTER LP_ACCEL_ODR;
MPU_REGISTER WOM_THR;
MPU_REGISTER FIFO_EN;
MPU_REGISTER I2C_MST_CTRL;
MPU_REGISTER I2C_SLV0_ADDR;
MPU_REGISTER I2C_SLV0_REG;
MPU_REGISTER I2C_SLV0_CTRL;
MPU_REGISTER I2C_SLV1_ADDR;
MPU_REGISTER I2C_SLV1_REG;
MPU_REGISTER I2C_SLV1_CTRL;
MPU_REGISTER I2C_SLV2_ADDR;
MPU_REGISTER I2C_SLV2_REG;
MPU_REGISTER I2C_SLV2_CTRL;
MPU_REGISTER I2C_SLV3_ADDR;
MPU_REGISTER I2C_SLV3_REG;
MPU_REGISTER I2C_SLV3_CTRL;
MPU_REGISTER I2C_SLV4_ADDR;
MPU_REGISTER I2C_SLV4_REG;
MPU_REGISTER I2C_SLV4_DO;
MPU_REGISTER I2C_SLV4_CTRL;
MPU_REGISTER I2C_SLV4_DI;
MPU_REGISTER I2C_MST_STATUS;
MPU_REGISTER INT_PIN_CFG;
MPU_REGISTER INT_ENABLE;
MPU_REGISTER INT_STATUS;
MPU_REGISTER ACCEL_XOUT_H;
MPU_REGISTER ACCEL_XOUT_L;
MPU_REGISTER ACCEL_YOUT_H;
MPU_REGISTER ACCEL_YOUT_L;
MPU_REGISTER ACCEL_ZOUT_H;
MPU_REGISTER ACCEL_ZOUT_L;
MPU_REGISTER TEMP_OUT_H;
MPU_REGISTER TEMP_OUT_L;
MPU_REGISTER GYRO_XOUT_H;
MPU_REGISTER GYRO_XOUT_L;
MPU_REGISTER GYRO_YOUT_H;
MPU_REGISTER GYRO_YOUT_L;
MPU_REGISTER GYRO_ZOUT_H;
MPU_REGISTER ACCEL_CONFIG2;
MPU_REGISTER GYRO_ZOUT_L;
MPU_REGISTER EXT_SENS_DATA_00;
MPU_REGISTER EXT_SENS_DATA_01;
MPU_REGISTER EXT_SENS_DATA_02;
MPU_REGISTER EXT_SENS_DATA_03;
MPU_REGISTER EXT_SENS_DATA_04;
MPU_REGISTER EXT_SENS_DATA_05;
MPU_REGISTER EXT_SENS_DATA_06;
MPU_REGISTER EXT_SENS_DATA_07;
MPU_REGISTER EXT_SENS_DATA_08;
MPU_REGISTER EXT_SENS_DATA_09;
MPU_REGISTER EXT_SENS_DATA_10;
MPU_REGISTER EXT_SENS_DATA_11;
MPU_REGISTER EXT_SENS_DATA_12;
MPU_REGISTER EXT_SENS_DATA_13;
MPU_REGISTER EXT_SENS_DATA_14;
MPU_REGISTER EXT_SENS_DATA_15;
MPU_REGISTER EXT_SENS_DATA_16;
MPU_REGISTER EXT_SENS_DATA_17;
MPU_REGISTER EXT_SENS_DATA_18;
MPU_REGISTER EXT_SENS_DATA_19;
MPU_REGISTER EXT_SENS_DATA_20;
MPU_REGISTER EXT_SENS_DATA_21;
MPU_REGISTER EXT_SENS_DATA_22;
MPU_REGISTER EXT_SENS_DATA_23;
MPU_REGISTER I2C_SLV0_DO;
MPU_REGISTER I2C_SLV1_DO;
MPU_REGISTER I2C_SLV2_DO;
MPU_REGISTER I2C_SLV3_DO;
MPU_REGISTER I2C_MST_DELAY_CTRL;
MPU_REGISTER SIGNAL_PATH_RESET;
MPU_REGISTER MOT_DETECT_CTRL;
MPU_REGISTER USER_CTRL;
MPU_REGISTER PWR_MGMT_1;
MPU_REGISTER PWR_MGMT_2;
MPU_REGISTER FIFO_COUNTH;
MPU_REGISTER FIFO_COUNTL;
MPU_REGISTER FIFO_R_W;
MPU_REGISTER WHO_AM_I;
MPU_REGISTER XA_OFFSET_H;
MPU_REGISTER XA_OFFSET_L;
MPU_REGISTER YA_OFFSET_H;
MPU_REGISTER YA_OFFSET_L;
MPU_REGISTER ZA_OFFSET_H;
MPU_REGISTER ZA_OFFSET_L;


//Magnetometer registers
MPU_REGISTER WIA;
MPU_REGISTER INFO;
MPU_REGISTER ST1;
MPU_REGISTER HXL;
MPU_REGISTER HXH;
MPU_REGISTER HYL;
MPU_REGISTER HYH;
MPU_REGISTER HZL;
MPU_REGISTER HZH;
MPU_REGISTER ST2;
MPU_REGISTER CNTL1;
MPU_REGISTER CNTL2;
MPU_REGISTER RSV;
MPU_REGISTER ASTC;
MPU_REGISTER TS1;
MPU_REGISTER TS2;
MPU_REGISTER I2CDIS;
MPU_REGISTER ASAX;
MPU_REGISTER ASAY;
MPU_REGISTER ASAZ;

/*										 Driver functions															*/

/*
 * General MPU functions
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale);
uint8_t MPU_WhoAmI();
void MPU_DisableComponents(MPU_DISABLE_AXIS disable_accel, MPU_DISABLE_AXIS disable_gyroscope);
void MPU_ResetDataRegisters();
void MPU_SignalPathReset(RESET_SENSOR_SIGNAL_PATH sensor_to_reset);
void MPU_ResetWholeIC();
uint8_t MPU_ReadAllSensores(float accel_data[], float gyro_data[], float mag_data[]);
/*
 * Fifo functions
 */
int16_t MPU_FifoReadData();
int16_t MPU_FifoCounter();
void MPU_FifoConfig(uint8_t enable_mpu_components, uint8_t fifo_mode);

/*
 * Accelerometer functions
 */
float MPU_AccelRead(AXIS axis);
void MPU_AccelScaleChange(MPU_ACCEL_SCALE new_scale);
void MPU_AccelLowPassFilterConfig(uint8_t ACCEL_FCHOICE, uint8_t A_DLPF_CFG);
void MPU_AccelOffset(AXIS axis, float value);

/*
 * Gyroscope functions
 */
float MPU_GyroRead(AXIS axis);
void MPU_GyroScaleChange(MPU_GYRO_SCALE new_scale);
void MPU_GyroTempLowPassFilterConfig(uint8_t FCHOICE, DLPF DLPF_CFG);
void MPU_GyroOffset(AXIS axis, float value);

/*
 * Temperature sensor functions
 */
int16_t MPU_ReadIC_Temperature();

/*
 * Magnetometer functions
 *
 */
uint8_t MPU_MagGetInfo();
uint8_t MPU_MagGetStatus1();
uint8_t MPU_MagGetStatus2();
float MPU_MagRead(AXIS axis);
uint8_t MPU_MagWhoAmI();
uint8_t MPU_MagConfigControl2(uint8_t reset);
void MPU_MagI2CDisable();

#endif /* INC_MPU_SPEC_H_ */
