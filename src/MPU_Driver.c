/*
 * MPU_Driver.c
 *
 *  Created on: Sep 13, 2020
 *      Author: Bruno Otávio
 */

/*
 * 1 - FIFO read is not working yet
 */

#include "MPU_SPEC.h"

static void I2C_Initialization(uint8_t I2Cx);
static void AccelScaleConfig(MPU_ACCEL_SCALE accel_scale);
static void GyroScaleConfig(MPU_GYRO_SCALE gyro_scale);

void MPU_WRITE(uint8_t mpu_r, uint8_t data_to_send);
uint8_t MPU_READ(uint8_t mpu_r,uint8_t number_of_bytes);

/*
 * @brief: MPU initialization function
 * @param: i2c - Specify what i2c peripheral will be used, the values can be ( USE_I2C1, USE_I2C2 or USE_I2C3 )
 * 		   mpu_i2c_addr - For choose what i2c address will be used, if AD0 = HIGH -> mpu_i2c_addr = ADDR_2, otherwise mpu_i2c_addr = ADDR_1
 *		   accel_scale  - Specify the sensitivity of the accelerometer, choose a value at @MPU_ACCEL_SCALE enum
 *		   gyro_scale   - Specify the sensitivity of the gyroscope, choose a value at @MPU_GYRO_SCALE enum
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale){

	I2C_Initialization(i2c);

	uint8_t accel_data = 0;
	uint8_t gyro_data = 0;

	if(mpu_i2c_addr == USE_ADDR1)
		addr_used = ADDR_1;

	else
		addr_used = ADDR_2;

	accel_data |= accel_scale 	<< 3;
	gyro_data  |= gyro_scale     << 3;

	AccelScaleConfig(accel_scale);
	GyroScaleConfig(gyro_scale);

	MPU_WRITE(ACCEL_CONFIG, accel_data);
	MPU_WRITE(GYRO_CONFIG, gyro_data);
}


/*
 * @brief: Internal driver function used to initialize the i2c peripheral chosen by the user
 *
 */
static void I2C_Initialization(uint8_t I2Cx){

	if(I2Cx == USE_I2C1){
		mpu_i2c_comm.Instance = I2C1;
	}

	else if(I2Cx == USE_I2C2){
		mpu_i2c_comm.Instance = I2C2;
	}
	else{
		mpu_i2c_comm.Instance = I2C3;
	}
	mpu_i2c_comm.Init.ClockSpeed 		= 400000;
	mpu_i2c_comm.Init.DutyCycle 		= I2C_DUTYCYCLE_2;
	mpu_i2c_comm.Init.OwnAddress1 		= 0;
	mpu_i2c_comm.Init.AddressingMode	= I2C_ADDRESSINGMODE_7BIT;
	mpu_i2c_comm.Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLE;
	mpu_i2c_comm.Init.OwnAddress2 		= 0;
	mpu_i2c_comm.Init.GeneralCallMode 	= I2C_GENERALCALL_DISABLE;
	mpu_i2c_comm.Init.NoStretchMode 	= I2C_NOSTRETCH_DISABLE;

	HAL_I2C_Init(&mpu_i2c_comm);
}

/*
 * @brief:	MPU_WRITE is used by the high-level methods for send configuration parameters
 * 			The user can change by himself some MPU registers just writing the corrects parameters
 * 			at the buffer of the desired command.
 * 			Use this function only if you know how to properly configure the MPU register
 * @param:  mpu_r - MPU specific register where some configuration will be written
 */
void MPU_WRITE(uint8_t mpu_r, uint8_t data_to_send){

	struct i2c_send{
		uint8_t addr;
		uint8_t data;
	};

	struct i2c_send aux_send;

	aux_send.addr = mpu_r;
	aux_send.data = data_to_send;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&aux_send, sizeof(aux_send), HAL_MAX_DELAY);
}


/* @brief:	MPU_READ is used by the high-level methods, like MPU_ReadAccelerometer, for read configuration parameters and data registers
 * 			The user can read by himself some device register just by sending the desired address registers
 * 			Use this function only if you know how to properly configure the MPU register
 * @param:  mpu_r - MPU specific register that will be read some configuration or information
 * @retval: Information read from the register specified at mpu_r parameter
 */
uint8_t MPU_READ(uint8_t mpu_r, uint8_t number_of_bytes){

	uint8_t data_register;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&mpu_r, sizeof(mpu_r), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, number_of_bytes, HAL_MAX_DELAY);
	return data_register;
}

/*@brief: Function to read last accelerometer data
 *@param: axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS144
 *@retval: Raw information that is coming from MPU accelerometer ADC
 */
int16_t MPU_ReadAccelerometer(uint8_t axis){

	int16_t data_return;

	if(axis == X_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_XOUT_H,1) << 8);
		data_return |= MPU_READ(ACCEL_XOUT_L,1);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_YOUT_H,1) << 8);
		data_return |= MPU_READ(ACCEL_YOUT_L,1);
	}
	else{
		data_return = (int16_t)(MPU_READ(ACCEL_ZOUT_H,1) << 8);
		data_return |= MPU_READ(ACCEL_ZOUT_L,1);
	}
	return data_return;
}

/*@brief: Function to read last gyroscope data
 *@param: axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS144
 *@retval: Raw information that is coming from MPU gyroscope ADC
 */
int16_t MPU_ReadGyro(uint8_t axis){

	int16_t data_return = 0;

	if(axis == X_AXIS){
		data_return = (int16_t)(MPU_READ(GYRO_XOUT_H,1) << 8);
		data_return |= MPU_READ(GYRO_XOUT_L,1);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(GYRO_YOUT_H,1) << 8);
		data_return |= MPU_READ(GYRO_YOUT_L,1);
	}
	else{
		data_return = (int16_t)(MPU_READ(GYRO_ZOUT_H,1) << 8);
		data_return |= MPU_READ(GYRO_ZOUT_L,1);
	}
	return data_return;
}

/*
 * @brief: Function to read last temperature data
 *@retval: Raw information that is coming from temperature register
 */
int16_t MPU_ReadIC_Temperature(){

//	int16_t data_return;

//	data_return = (int16_t)(MPU_READ(TEMP_OUT_H) << 8);
//	data_return |= MPU_READ(TEMP_OUT_L);
//	return data_return;
	return 0;
}

/*
 * @brief -	Return the device identity
 * @retval - Device identity
 */
uint8_t MPU_Identity(){

	return MPU_READ(WHO_AM_I,1);
}

/* @brief: Function used to change the sensitivity of the accelerometer at any desired instant
 * @param: New sensitivity that will be used, can be one value of @MPU_ACCEL_SCALE enum
 */
void MPU_ChangeAccelScale(MPU_ACCEL_SCALE new_scale){
	uint8_t accel_data = 0;

	accel_data |= new_scale << 3;

	AccelScaleConfig(new_scale);
	MPU_WRITE(ACCEL_CONFIG, accel_data);
}

/* @brief: Function used to change the sensitivity of the gyroscope at any desired instant
 * @param: New sensitivity that will be used, can be one value of @MPU_GYRO_SCALE enum
 */
void MPU_ChangeGyroScale(MPU_GYRO_SCALE new_scale){
	uint8_t gyro_scale = 0;

	gyro_scale |= new_scale << 3;

	GyroScaleConfig(new_scale);
	MPU_WRITE(GYRO_CONFIG,gyro_scale);
}

/*
 *@brief - Configure the accelerometer resolution
 * 		   Greater the range that the accelerometer must read, smaller the resolution
 *@param - accel_scale: A MPU_ACCEL_SCALE parameter that defines the accelerometer sensitivity
 *@retval - None
 */
static void AccelScaleConfig(MPU_ACCEL_SCALE accel_scale){

	if(accel_scale == ACCEL_FULL_SCALE_16g){
		accel_sensitivity_used = 2048;
	}
	else if(accel_scale == ACCEL_FULL_SCALE_8g){
		accel_sensitivity_used = 4096;
	}
	else if(accel_scale == ACCEL_FULL_SCALE_4g){
		accel_sensitivity_used = 8192;
	}
	else{
		accel_sensitivity_used = 16384;
	}
}

/*
 *
 * @brief Configure the gyroscope resolution
 * 		  Greater the range that the gyroscope must read, smaller the resolution
 * @param - gyro_scale: A MPU_GYRO_SCALE parameter tha defines the gyroscope sensitivity
 * @retval: None
 */
static void GyroScaleConfig(MPU_GYRO_SCALE gyro_scale){

	if(gyro_scale == GYRO_FULL_SCALE_2000dps){
		gyro_sensitivity_used = 16.4;
	}
	else if(gyro_scale == GYRO_FULL_SCALE_1000dps){
		gyro_sensitivity_used = 32.8;
	}
	else if(gyro_scale == GYRO_FULL_SCALE_500dps){
		gyro_sensitivity_used = 65.5;
	}
	else{
		gyro_sensitivity_used = 131;
	}
}

/*
 *@brief Change the information read by the accelerometer from ADC resolution to g
 * 		 resolution = 1/accel_sensitivity
 *
 */
float MPU_TransformAccelRead(int16_t raw_data_read){

	return (float)(raw_data_read * (1.0/accel_sensitivity_used));
}

 /* @brief Configuration of low pass filter for accelerometer. Configuration can be made with the following table:
 *
 * 																				OUTPUT
 * 			ACCEL_FCHOICE	A_DLPF_CFG		BANDWITH(HZ)		Delay(ms)		Noise Density		Rate(Hz):
 * 				0				X			   1.13k			  0.75				250					4
 * 				1				0			    460				  1.94				250					1
 * 				1				1				184				  5.80				250					1
 * 				1				2				92  			  7.80				250					1
 * 				1				3				41				  11.80				250					1
 * 				1				4				20				  19.80				250					1
 * 				1				5				10				  35.70				250					1
 * 				1				6				 5				  66.96				250					1
 * 				1				7				460				  1.94				250					1
 *
 * @param:	ACCEL_FCHOICE - used to bypass the digital low pass filter as the table above
 * 			A_DLPF_CFG:   - accelerometer low pass filter configuration as the table above
 * @retval: None
 */

void MPU_AccelerometerLowPassFilterConfig(uint8_t ACCEL_FCHOICE, uint8_t A_DLPF_CFG){

	uint8_t accel_data = 0;


	accel_data |= ACCEL_FCHOICE << 3;

	if(A_DLPF_CFG <= A_DLPF_CFG7){
		accel_data |= A_DLPF_CFG;
	}

	MPU_WRITE(ACCEL_CONFIG2, accel_data);

}

/*
 * @brief: Configuration of FIFO mode of operation and component enabling
 * @param:
 * 			If:
 *				enable_mpu_components[7] = 1 ->  temperature data (TEMP_OUT_H, TEMP_OUT_L) will be buffered at fifo, even if data path is in standby
 * 				enable_mpu_components[6] = 1 ->  Gyro X axis data (GYRO_XOUT_X, GYRO_XOUT_L)  will be buffered at fifo, even if data path is in standby
 *				enable_mpu_components[5] = 1 ->  Gyro Y axis data (GYRO_YOUT_X, GYRO_YOUT_L)  will be buffered at fifo, even if data path is in standby
 * 				enable_mpu_components[4] = 1 ->  Gyro Z axis data (GYRO_ZOUT_X, GYRO_ZOUT_L)  will be buffered at fifo, even if data path is in standby
 *				enable_mpu_components[3] = 1 -> accel data (all axis)  will be buffered at fifo, even if data path is in standby
 * 				enable_mpu_components[2] = 1 -> associated with auxiliary i2c, TODO
 *				enable_mpu_components[1] = 1 -> associated with auxiliary i2c, TODO
 * 				enable_mpu_components[0] = 1 -> associated with auxiliary i2c, TODO
 *
 * fifo_mode - if fifo_mode = FIFO_NOT_OVERRIDE, new incoming data will not replace the oldest data
 * 			   if fifo_mode = FIFO_OVERRIDE, new incoming data will replace the oldest
 * @retval: None
 */

void MPU_FifoConfig(uint8_t enable_mpu_components, uint8_t fifo_mode){

	uint8_t user_data = 0;
	uint8_t fifo_data = 0;
	uint8_t cfg_data = 0;

   user_data |= 1 << 6;						//FIFO Enable
   user_data |= 1 << 2;						//Reset Fifo module
   MPU_WRITE(USER_CTRL, user_data);

   fifo_data |= enable_mpu_components;		//controls what data will be put at fifo
   MPU_WRITE(FIFO_EN, fifo_data);

   cfg_data |= fifo_mode << 6;					//controls if new data override or not the oldest
   MPU_WRITE(CONFIG, cfg_data);
}

/*
 * @brief: Controls the number of bytes available at FIFO
 * 		   If the register FIFO_COUNTH is sent, the return is two bytes that contains the number of words writes at fifo, but at the first byte that is
 * 		   coming only the five first bits are relevant information, so a mask was applied (0x1F) to disregard the 3 lasts bits
 * @param: None
 * @reval: Number of bytes writes at FIFO
 *
 */
int16_t MPU_FifoCounter(){

	uint8_t data_register[2] = {0};
	uint16_t to_return;

	uint8_t fifo_counth = FIFO_COUNTH;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&fifo_counth, sizeof(fifo_counth), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

	to_return = ((uint16_t)(data_register[0] & 0x1F) << 8) | data_register[1];
	return to_return;
}

/*
 *@brief: Returns the data on the top of fifo
 *		  Informations are written at fifo with register address order, for example, if temperature sensor and gyroscope is configured to write at fifo
 *		  the first data that will be written are temperature data because it register address is smaller than gyroscope register address
 *		  For enable or disable one component to write at fifo, change @enable_mpu_components at @MPU_FIFOConfig function
 *
 *
 *@param: None
 *@retval: One signed byte that represents data of gyroscope, temperature sensor or accelerometer
 */
int16_t MPU_FifoReadData(){

	uint8_t data_register[2] = {0};
	uint16_t to_return;

	uint8_t fifo_r_w = FIFO_R_W;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&fifo_r_w, sizeof(fifo_r_w), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

	to_return = ((uint16_t)(data_register[0]  << 8)) | data_register[1];
	return to_return;
}


/*
 * @brief: Disable one or more MPU components
 * @param:
 * 		  disable_accel can be one value of @MPU_DISABLE_AXIS
 *		  disable_gyroscope can be one value of @MPU_DISABLE_AXIS
 *
 *@retval: None
 */
void MPU_DisableComponents(MPU_DISABLE_AXIS disable_accel, MPU_DISABLE_AXIS disable_gyroscope){

	uint8_t pwr_mgmt2_data = 0;

	pwr_mgmt2_data = (disable_accel << 3) | disable_gyroscope;;		//information of what accel axis (last three bits) and gyro axis(first three bits) must be disable
	MPU_WRITE(PWR_MGMT_2, pwr_mgmt2_data);
}



/*
 * TODO
 */
float MPU_TransformGyroRead(int16_t raw_data_read){

//	return (float)(raw_data_read * (1.0/gyro_sensitivity_used));
	return 0.0;
}



