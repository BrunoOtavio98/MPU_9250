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

typedef struct {
	uint8_t register_address;			//holds the i2c address of the internal register
	uint8_t data_cmd;					//holds information that will be send to make some register configuration
}MPU_REGISTER;

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
int8_t MPU_FifoReadData();
int16_t MPU_FifoCounter();
void MPU_FifoConfig(uint8_t enable_mpu_components, uint8_t fifo_mode);
void MPU_FifoReset();
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