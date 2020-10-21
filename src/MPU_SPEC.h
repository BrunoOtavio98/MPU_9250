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

//	Global definition


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

#define ADDR_1 0x68
#define ADDR_2 0x69

#define USE_I2C1 1
#define USE_I2C2 2
#define USE_I2C3 3

#define X_AXIS   1
#define Y_AXIS   2
#define Z_AXIS   3

I2C_HandleTypeDef mpu_i2c_comm;						//Holds the i2c peripheral registers used by the mcu to connect with MPU
uint8_t addr_used;									// holds the mpu i2c address

/*
 *	All of accelerometer specific definition will be placed at this place
 *
 */
#define SI_ACCELERATION 		9.806				//acceleration in international system of units 1g = 9.8 m/s^2

/*
 * Possible configuration of low pass filter register
 */

#define ACCEL_FCHOICE0_b			1
#define ACCEL_FCHOICE1_b			0

#define A_DLPF_CFGX				0
#define A_DLPF_CFG0				0
#define A_DLPF_CFG1				1
#define A_DLPF_CFG2				2
#define A_DLPF_CFG3				3
#define A_DLPF_CFG4				4
#define A_DLPF_CFG5				5
#define A_DLPF_CFG6				6
#define A_DLPF_CFG7				7

/*
 * Specify all of sensitivity available by the MPU-9250 accelerometer
 */
typedef enum{
	 ACCEL_FULL_SCALE_2g   =	00,
	 ACCEL_FULL_SCALE_4g   =	01,
	 ACCEL_FULL_SCALE_8g   = 	10,
	 ACCEL_FULL_SCALE_16g  =	11
}MPU_ACCEL_SCALE;

uint16_t accel_sensitivity_used;			  //Currently accelerometer sensitivity used by the MPU


/*
 * 	 All of gyroscope specific definition will be placed at this place
 */
typedef enum{
	 GYRO_FULL_SCALE_250dps  =  00,
	 GYRO_FULL_SCALE_500dps	 = 	01,
	 GYRO_FULL_SCALE_1000dps =  10,
	 GYRO_FULL_SCALE_2000dps =  11
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
 * MPU-9250 available registers
*/


#define  SELF_TEST_X_GYRO		 0x00
#define	 SELF_TEST_Y_GYRO 		 0x01
#define	 SELF_TEST_Z_GYRO 		 0x02
#define	 SELF_TEST_X_ACCEL 		 0x0D
#define	 SELF_TEST_Y_ACCEL 		 0x0E
#define	 SELF_TEST_Z_ACCEL 		 0x0F
#define	 XG_OFFSET_H 			 0x13
#define	 XG_OFFSET_L 			 0x14
#define	 YG_OFFSET_H 			 0x15
#define	 YG_OFFSET_L 			 0x16
#define	 ZG_OFFSET_H 			 0x17
#define	 ZG_OFFSET_L 			 0x18
#define  SMPLRT_DIV 			 0x19
#define	 CONFIG 				 0x1A
#define	 GYRO_CONFIG 			 0x1B
#define  ACCEL_CONFIG 			 0x1C
#define	 ACCEL_CONFIG2 			 0x1D
#define	 LP_ACCEL_ODR 			 0x1E
#define	 WOM_THR 				 0x1F
#define	 FIFO_EN 				 0x23
#define	 I2C_MST_CTRL 			 0x24
#define	 I2C_SLV0_ADDR 			 0x25
#define	 I2C_SLV0_REG 			 0x26
#define	 I2C_SLV0_CTRL			 0x27
#define	 I2C_SLV1_ADDR 			 0x28
#define	 I2C_SLV1_REG 			 0x29
#define	 I2C_SLV1_CTRL 			 0x2A
#define	 I2C_SLV2_ADDR 			 0x2B
#define	 I2C_SLV2_REG 			 0x2C
#define	 I2C_SLV2_CTRL 			 0x2D
#define	 I2C_SLV3_ADDR			 0x2E
#define	 I2C_SLV3_REG 			 0x2F
#define	 I2C_SLV3_CTRL 			 0x30
#define	 I2C_SLV4_ADDR 			 0x31
#define	 I2C_SLV4_REG	 		 0x32
#define	 I2C_SLV4_DO 			 0x33
#define	 I2C_SLV4_CTRL 			 0x34
#define	 I2C_SLV4_DI 			 0x35
#define	 I2C_MST_STATUS 		 0x36
#define	 INT_PIN_CFG 			 0x37
#define	 INT_ENABLE 			 0x38
#define	 INT_STATUS 			 0x3A
#define	 ACCEL_XOUT_H 			 0x3B
#define	 ACCEL_XOUT_L 			 0x3C
#define	 ACCEL_YOUT_H 			 0x3D
#define	 ACCEL_YOUT_L 			 0x3E
#define	 ACCEL_ZOUT_H 			 0x3F
#define	 ACCEL_ZOUT_L 			 0x40
#define	 TEMP_OUT_H		 		 0x41
#define	 TEMP_OUT_L 			 0x42
#define	 GYRO_XOUT_H 			 0x43
#define	 GYRO_XOUT_L 			 0x44
#define	 GYRO_YOUT_H 			 0x45
#define	 GYRO_YOUT_L 			 0x46
#define	 GYRO_ZOUT_H 			 0x47
#define	 GYRO_ZOUT_L 			 0x48
#define	 EXT_SENS_DATA_00 		 0x49
#define	 EXT_SENS_DATA_01 		 0x4A
#define	 EXT_SENS_DATA_02 		 0x4B
#define	 EXT_SENS_DATA_03 		 0x4C
#define	 EXT_SENS_DATA_04 		 0x4D
#define	 EXT_SENS_DATA_05 		 0x4E
#define	 EXT_SENS_DATA_06 		 0x4F
#define	 EXT_SENS_DATA_07 		 0x50
#define	 EXT_SENS_DATA_08 		 0x51
#define	 EXT_SENS_DATA_09 		 0x52
#define	 EXT_SENS_DATA_10 		 0x53
#define	 EXT_SENS_DATA_11		 0x54
#define	 EXT_SENS_DATA_12 		 0x55
#define	 EXT_SENS_DATA_13 		 0x56
#define	 EXT_SENS_DATA_14 		 0x57
#define	 EXT_SENS_DATA_15		 0x58
#define	 EXT_SENS_DATA_16 		 0x59
#define  EXT_SENS_DATA_17 		 0x5A
#define	 EXT_SENS_DATA_18  		 0x5B
#define	 EXT_SENS_DATA_19  		 0x5C
#define	 EXT_SENS_DATA_20  		 0x5D
#define	 EXT_SENS_DATA_21  		 0x5E
#define	 EXT_SENS_DATA_22  	 	 0x5F
#define	 EXT_SENS_DATA_23 		 0x60
#define	 I2C_SLV0_DO  			 0x63
#define	 I2C_SLV1_DO			 0x64
#define	 I2C_SLV2_DO			 0x65
#define	 I2C_SLV3_DO  			 0x66
#define	 I2C_MST_DELAY_CTRL		 0x67
#define	 SIGNAL_PATH_RESET	 	 0x68
#define	 MOT_DETECT_CTRL  		 0x69
#define	 USER_CTRL   			 0x6A
#define  PWR_MGMT_1   			 0x6B
#define  PWR_MGMT_2   			 0x6C
#define  FIFO_COUNTH  			 0x72
#define  FIFO_COUNTL  			 0x73
#define  FIFO_R_W     			 0x74
#define  WHO_AM_I 	   			 0x75
#define	 XA_OFFSET_H  			 0x77
#define  XA_OFFSET_L  			 0x78
#define	 YA_OFFSET_H  			 0x7A
#define	 YA_OFFSET_L 			 0x7B
#define	 ZA_OFFSET_H  			 0x7D
#define  ZA_OFFSET_L  			 0x7E

//										 Driver functions

/*
 * General MPU functions
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale);
uint8_t MPU_Identity();
void MPU_DisableComponents(MPU_DISABLE_AXIS disable_accel, MPU_DISABLE_AXIS disable_gyroscope);

/*
 * Fifo functions
 */
int16_t MPU_FifoReadData();
int16_t MPU_FifoCounter();
void MPU_FifoConfig(uint8_t enable_mpu_components, uint8_t fifo_mode);

/*
 * Accelerometer functions
 */
int16_t MPU_ReadAccelerometer(uint8_t axis);
void MPU_ChangeAccelScale(MPU_ACCEL_SCALE new_scale);
float MPU_TransformAccelRead(int16_t raw_data_read);
void MPU_AccelerometerLowPassFilterConfig(uint8_t ACCEL_FCHOICE, uint8_t A_DLPF_CFG);

/*
 * Gyroscope functions
 */
int16_t MPU_ReadGyro(uint8_t axis);
float MPU_TransformGyroRead(int16_t raw_data_read);
void MPU_ChangeGyroScale(MPU_GYRO_SCALE new_scale);


/*
 * Temperature sensor functions
 */
int16_t MPU_ReadIC_Temperature();


#endif /* INC_MPU_SPEC_H_ */
