/*
 * MPU_Driver.c
 *
 *  Created on: Sep 13, 2020
 *      Author: Bruno Otávio
 */

#include "MPU_SPEC.h"

static void Register_Initialization();
static void I2C_Initialization(uint8_t I2Cx);
static void AccelScaleConfig(MPU_ACCEL_SCALE accel_scale);
static void GyroScaleConfig(MPU_GYRO_SCALE gyro_scale);

void MPU_WRITE(MPU_REGISTER mpu_r);
uint8_t MPU_READ(MPU_REGISTER mpu_r);

/*
 * @brief: MPU initialization function
 * @param: i2c - Specify what i2c peripheral will be used, the values can be ( USE_I2C1, USE_I2C2 or USE_I2C3 )
 * 		   mpu_i2c_addr - For choose what i2c address will be used, if AD0 = HIGH -> mpu_i2c_addr = ADDR_2, otherwise mpu_i2c_addr = ADDR_1
 *		   accel_scale  - Specify the sensitivity of the accelerometer, choose a value at @MPU_ACCEL_SCALE enum
 *		   gyro_scale   - Specify the sensitivity of the gyroscope, choose a value at @MPU_GYRO_SCALE enum
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale){

	I2C_Initialization(i2c);

	if(mpu_i2c_addr == USE_ADDR1)
		addr_used = ADDR_1;

	else
		addr_used = ADDR_2;

	Register_Initialization();

	ACCEL_CONFIG.data_cmd |= accel_scale 	<< 3;
	GYRO_CONFIG.data_cmd  |= gyro_scale     << 3;

	AccelScaleConfig(accel_scale);
	GyroScaleConfig(gyro_scale);

	MPU_WRITE(ACCEL_CONFIG);
	MPU_WRITE(GYRO_CONFIG);
}

/*
 *  @brief: Internal driver function, used to initialize all of MPU registers
 */
static void Register_Initialization(){

	 SELF_TEST_X_GYRO.register_address		= 0x00;
	 SELF_TEST_Y_GYRO.register_address 		= 0x01;
	 SELF_TEST_Z_GYRO.register_address 		= 0x02;
	 SELF_TEST_X_ACCEL.register_address 	= 0x0D;
	 SELF_TEST_Y_ACCEL.register_address 	= 0x0E;
	 SELF_TEST_Z_ACCEL.register_address 	= 0x0F;
	 XG_OFFSET_H.register_address 			= 0x13;
	 XG_OFFSET_L.register_address 			= 0x14;
	 YG_OFFSET_H.register_address 			= 0x15;
	 YG_OFFSET_L.register_address 			= 0x16;
	 ZG_OFFSET_H.register_address 			= 0x17;
	 ZG_OFFSET_L.register_address 			= 0x18;
	 SMPLRT_DIV.register_address 			= 0x19;
	 CONFIG.register_address 				= 0x1A;
	 GYRO_CONFIG.register_address 			= 0x1B;
	 ACCEL_CONFIG.register_address 			= 0x1C;
	 ACCEL_CONFIG2.register_address 		= 0x1D;
	 LP_ACCEL_ODR.register_address 			= 0x1E;
	 WOM_THR.register_address 				= 0x1F;
	 FIFO_EN.register_address 				= 0x23;
	 I2C_MST_CTRL.register_address 			= 0x24;
	 I2C_SLV0_ADDR.register_address 		= 0x25;
	 I2C_SLV0_REG.register_address 			= 0x26;
	 I2C_SLV0_CTRL.register_address			= 0x27;
	 I2C_SLV1_ADDR.register_address 		= 0x28;
	 I2C_SLV1_REG.register_address 			= 0x29;
	 I2C_SLV1_CTRL.register_address 		= 0x2A;
	 I2C_SLV2_ADDR.register_address 		= 0x2B;
	 I2C_SLV2_REG.register_address 			= 0x2C;
	 I2C_SLV2_CTRL.register_address 		= 0x2D;
	 I2C_SLV3_ADDR.register_address			= 0x2E;
	 I2C_SLV3_REG.register_address 			= 0x2F;
	 I2C_SLV3_CTRL.register_address 		= 0x30;
	 I2C_SLV4_ADDR.register_address 		= 0x31;
	 I2C_SLV4_REG.register_address	 		= 0x32;
	 I2C_SLV4_DO.register_address 			= 0x33;
	 I2C_SLV4_CTRL.register_address 		= 0x34;
	 I2C_SLV4_DI.register_address 			= 0x35;
	 I2C_MST_STATUS.register_address 		= 0x36;
	 INT_PIN_CFG.register_address 			= 0x37;
	 INT_ENABLE.register_address 			= 0x38;
	 INT_STATUS.register_address 			= 0x3A;
	 ACCEL_XOUT_H.register_address 			= 0x3B;
	 ACCEL_XOUT_L.register_address 			= 0x3C;
	 ACCEL_YOUT_H.register_address 			= 0x3D;
	 ACCEL_YOUT_L.register_address 			= 0x3E;
	 ACCEL_ZOUT_H.register_address 			= 0x3F;
	 ACCEL_ZOUT_L.register_address 			= 0x40;
	 TEMP_OUT_H.register_address		 	= 0x41;
	 TEMP_OUT_L.register_address 			= 0x42;
	 GYRO_XOUT_H.register_address 			= 0x43;
	 GYRO_XOUT_L.register_address 			= 0x44;
	 GYRO_YOUT_H.register_address 			= 0x45;
	 GYRO_YOUT_L.register_address 			= 0x46;
	 GYRO_ZOUT_H.register_address 			= 0x47;
	 GYRO_ZOUT_L.register_address 			= 0x48;
	 EXT_SENS_DATA_00.register_address 		= 0x49;
	 EXT_SENS_DATA_01.register_address 		= 0x4A;
	 EXT_SENS_DATA_02.register_address 		= 0x4B;
	 EXT_SENS_DATA_03.register_address 		= 0x4C;
	 EXT_SENS_DATA_04.register_address 		= 0x4D;
	 EXT_SENS_DATA_05.register_address 		= 0x4E;
	 EXT_SENS_DATA_06.register_address 		= 0x4F;
	 EXT_SENS_DATA_07.register_address 		= 0x50;
	 EXT_SENS_DATA_08.register_address 		= 0x51;
	 EXT_SENS_DATA_09.register_address 		= 0x52;
	 EXT_SENS_DATA_10.register_address 		= 0x53;
	 EXT_SENS_DATA_11.register_address		= 0x54;
	 EXT_SENS_DATA_12.register_address 		= 0x55;
	 EXT_SENS_DATA_13.register_address 		= 0x56;
	 EXT_SENS_DATA_14.register_address 		= 0x57;
	 EXT_SENS_DATA_15.register_address		= 0x58;
	 EXT_SENS_DATA_16.register_address 		= 0x59;
	 EXT_SENS_DATA_17.register_address 		= 0x5A;
	 EXT_SENS_DATA_18.register_address  	= 0x5B;
	 EXT_SENS_DATA_19.register_address  	= 0x5C;
	 EXT_SENS_DATA_20.register_address  	= 0x5D;
	 EXT_SENS_DATA_21.register_address  	= 0x5E;
	 EXT_SENS_DATA_22.register_address  	= 0x5F;
	 EXT_SENS_DATA_23.register_address 		= 0x60;
	 I2C_SLV0_DO.register_address  			= 0x63;
	 I2C_SLV1_DO.register_address			= 0x64;
	 I2C_SLV2_DO.register_address			= 0x65;
	 I2C_SLV3_DO.register_address  			= 0x66;
	 I2C_MST_DELAY_CTRL.register_address	= 0x67;
	 SIGNAL_PATH_RESET.register_address	 	= 0x68;
	 MOT_DETECT_CTRL.register_address  		= 0x69;
	 USER_CTRL.register_address   			= 0x6A;
	 PWR_MGMT_1.register_address   			= 0x6B;
	 PWR_MGMT_2.register_address   			= 0x6C;
	 FIFO_COUNTH.register_address  			= 0x72;
	 FIFO_COUNTL.register_address  			= 0x73;
	 FIFO_R_W.register_address     			= 0x74;
	 WHO_AM_I.register_address 	   			= 0x75;
	 XA_OFFSET_H.register_address  			= 0x77;
	 XA_OFFSET_L.register_address  			= 0x78;
	 YA_OFFSET_H.register_address  			= 0x7A;
	 YA_OFFSET_L.register_address 			= 0x7B;
	 ZA_OFFSET_H.register_address  			= 0x7D;
	 ZA_OFFSET_L.register_address  			= 0x7E;
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
	mpu_i2c_comm.Init.ClockSpeed 		= 100000;
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
 * @param:  mpu_r - MPU specific register where some configuration will be written
 */
void MPU_WRITE(MPU_REGISTER mpu_r){
	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&mpu_r, sizeof(mpu_r), HAL_MAX_DELAY);
}


/* @brief:	MPU_READ is used by the high-level methods, like MPU_ReadAccelerometer, for read configuration parameters and data registers
 * 			The user can read by himself some device register just by sending the desired address registers
 * @param:  mpu_r - MPU specific register that will be read some configuration or information
 * @retval: Information read from the register specified at mpu_r parameter
 */
uint8_t MPU_READ(MPU_REGISTER mpu_r){

	uint8_t data_register;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&mpu_r.register_address, sizeof(mpu_r.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, sizeof(data_register), HAL_MAX_DELAY);
	return data_register;
}

/*@brief: Function to read last accelerometer data
 *@param: axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS144
 *@retval: Raw information that is coming from MPU accelerometer ADC
 */
int16_t MPU_ReadAccelerometer(uint8_t axis){

	int16_t data_return;

	if(axis == X_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_XOUT_H) << 8);
		data_return |= MPU_READ(ACCEL_XOUT_L);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_YOUT_H) << 8);
		data_return |= MPU_READ(ACCEL_YOUT_L);
	}
	else{
		data_return = (int16_t)(MPU_READ(ACCEL_ZOUT_H) << 8);
		data_return |= MPU_READ(ACCEL_ZOUT_L);
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
		data_return = (int16_t)(MPU_READ(GYRO_XOUT_H) << 8);
		data_return |= MPU_READ(GYRO_XOUT_L);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(GYRO_YOUT_H) << 8);
		data_return |= MPU_READ(GYRO_YOUT_L);
	}
	else{
		data_return = (int16_t)(MPU_READ(GYRO_ZOUT_H) << 8);
		data_return |= MPU_READ(GYRO_ZOUT_L);
	}
	return data_return;
}

/*
 * @brief: Function to read last temperature data
 *@retval: Raw information that is coming from temperature register
 */
int16_t MPU_ReadIC_Temperature(){

	int16_t data_return;

	data_return = (int16_t)(MPU_READ(TEMP_OUT_H) << 8);
	data_return |= MPU_READ(TEMP_OUT_L);
	return data_return;
}

/*
 * @brief -	Return the device identity
 * @retval - Device identity
 */
uint8_t MPU_Identity(){
	uint8_t data_return;

	data_return = MPU_READ(WHO_AM_I);
	return data_return;
}

/* @brief: Function used to change the sensitivity of the accelerometer at any desired instant
 * @param: New sensitivity that will be used, can be one value of @MPU_ACCEL_SCALE enum
 */
void MPU_ChangeAccelScale(MPU_ACCEL_SCALE new_scale){
	ACCEL_CONFIG.data_cmd |= new_scale << 3;

	AccelScaleConfig(new_scale);
	MPU_WRITE(ACCEL_CONFIG);
}

/* @brief: Function used to change the sensitivity of the gyroscope at any desired instant
 * @param: New sensitivity that will be used, can be one value of @MPU_GYRO_SCALE enum
 */
void MPU_ChangeGyroScale(MPU_GYRO_SCALE new_scale){
	GYRO_CONFIG.data_cmd |= new_scale << 3;

	GyroScaleConfig(new_scale);
	MPU_WRITE(GYRO_CONFIG);
}

/*@param - accel_scale: A MPU_ACCEL_SCALE parameter that defines the accelerometer sensitivity
 *@brief - Configure the accelerometer resolution
 * 		   Greater the range that the accelerometer must read, smaller the resolution
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
 * @param - gyro_scale: A MPU_GYRO_SCALE parameter tha defines the gyroscope sensitivity
 * @brief Configure the gyroscope resolution
 * 		  Greater the range that the gyroscope must read, smaller the resolution
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

/*@brief Configuration of low pass filter for accelerometer. Configuration can be made with the following table:
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
 */

void MPU_AccelerometerLowPassFilterConfig(uint8_t ACCEL_FCHOICE, uint8_t A_DLPF_CFG){


	ACCEL_CONFIG2.data_cmd = 0;
	ACCEL_CONFIG2.data_cmd |= ACCEL_FCHOICE << 3;

	if(A_DLPF_CFG <= A_DLPF_CFG7){
		ACCEL_CONFIG2.data_cmd |= A_DLPF_CFG;
	}

	MPU_WRITE(ACCEL_CONFIG2);
	ACCEL_CONFIG2.data_cmd = 0;
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
 * fifo_mode - if fifo_mode = FIFO_NOT_OVERRIDE, new incoming data will not replaced the oldest data
 * 			   if fifo_mode = FIFO_OVERRIDE, new incoming data will replace the oldest
 */

void MPU_FifoConfig(uint8_t enable_mpu_components, uint8_t fifo_mode){

   USER_CTRL.data_cmd = 0;
   USER_CTRL.data_cmd |= 1 << 6;						//FIFO Enable
   MPU_WRITE(USER_CTRL);

   FIFO_EN.data_cmd = 0;
   FIFO_EN.data_cmd |= enable_mpu_components;		//controls what data will be put at fifo
   MPU_WRITE(FIFO_EN);

   CONFIG.data_cmd |= fifo_mode << 6;					//controls if new data override or not the oldest
   MPU_WRITE(CONFIG);
}

/*
 * @brief: Controls the number of bytes available at FIFO
 * @reval: Number of bytes writes at FIFO
 *
 */
int16_t MPU_FifoCounter(){

	uint16_t data_register;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&FIFO_COUNTH.register_address, sizeof(FIFO_COUNTH.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, sizeof(data_register), HAL_MAX_DELAY);
	return data_register;
}

/*
 *@brief: Returns the data on the top of fifo
 *		  Informations are written at fifo with register address order, for example, if temperature sensor and gyroscope is configured to write at fifo
 *		  the first data that will be written are temperature data because it register address is smaller than gyroscope register address
 *		  For enable or disable one component to write at fifo, change @enable_mpu_components at @MPU_FIFOConfig function
 *
 *		  Remember that the data thats is coming from the FIFO is raw data, for a value that make sense you should read
 *		  two times the FIFO data, for example, x-axis of accelerometer, will be write first the High part, after the Low part
 *		  Read two times data, and make the following operation:
 *		  			meaningfull_accelerometer_data = FIRST_FIFO_READ << 8 || SECOND_FIFO_READ
 *		  After, call @MPU_TransformAccelRead and pass as parameter meaningfull_accelerometer_data to get information in g unity
 *
 *
 *@retval: One signed byte that represents data of gyroscope, temperature sensor or accelerometer
 */
int8_t MPU_FifoReadData(){

	return (int8_t)MPU_READ(FIFO_R_W);
}

/*
 * TODO
 */
float MPU_TransformGyroRead(int16_t raw_data_read){

//	return (float)(raw_data_read * (1.0/gyro_sensitivity_used));
}



