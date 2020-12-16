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
static void Register_Initialization();

void MPU_WRITE(MPU_REGISTER mpu_r, uint8_t addr);
uint8_t MPU_READ(MPU_REGISTER mpu_r,uint8_t number_of_bytes, uint8_t addr);

/*
 * @brief: MPU initialization function
 * @param: i2c - Specify what i2c peripheral will be used, the values can be ( USE_I2C1, USE_I2C2 or USE_I2C3 )
 * 		   mpu_i2c_addr - For choose what i2c address will be used, if AD0 = HIGH -> mpu_i2c_addr = ADDR_2, otherwise mpu_i2c_addr = ADDR_1
 *		   accel_scale  - Specify the sensitivity of the accelerometer, choose a value at @MPU_ACCEL_SCALE enum
 *		   gyro_scale   - Specify the sensitivity of the gyroscope, choose a value at @MPU_GYRO_SCALE enum
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale){

	I2C_Initialization(i2c);

	Register_Initialization();

	if(mpu_i2c_addr == USE_ADDR1)
		addr_used = ACCELGYRO_ADDR_1;
	else
		addr_used = ACCELGYRO_ADDR_2;

	ACCEL_CONFIG.data_cmd |= accel_scale 	<< 3;
	GYRO_CONFIG.data_cmd  |= gyro_scale     << 3;

	AccelScaleConfig(accel_scale);
	GyroScaleConfig(gyro_scale);

	MPU_WRITE(ACCEL_CONFIG, addr_used);
	MPU_WRITE(GYRO_CONFIG, addr_used);

	CNTL.data_cmd = 1 << 5;					//16 bit output
	MPU_WRITE(CNTL, AK8963_ADDR);
//	accelx_factory_trim = (MPU_READ(XA_OFFSET_H, 1) << 8 | MPU_READ(XA_OFFSET_L, 1))/2048.0;
//	accely_factory_trim = (MPU_READ(YA_OFFSET_H, 1) << 8 | MPU_READ(YA_OFFSET_L, 1))/2048.0;
//	accelz_factory_trim = (MPU_READ(ZA_OFFSET_H, 1) << 8 | MPU_READ(ZA_OFFSET_L, 1))/2048.0;
	magx_Adj = MPU_READ(ASAX, 1, AK8963_ADDR);
	magy_Adj = MPU_READ(ASAY, 1, AK8963_ADDR);
	magz_Adj = MPU_READ(ASAZ, 1, AK8963_ADDR);
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

	 //AK8963
	 WIA.register_address 	 = 0x00;
	 INFO.register_address 	 = 0x01;
	 ST1.register_address 	 = 0x02;
	 HXL.register_address 	 = 0x03;
	 HXH.register_address 	 = 0x04;
	 HYL.register_address 	 = 0x05;
	 HYH.register_address 	 = 0x06;
	 HZL.register_address 	 = 0x07;
	 HZH.register_address 	 = 0x08;
	 ST2.register_address 	 = 0x09;
	 CNTL.register_address  = 0x0A;
	 RSV.register_address 	 = 0x0B;
	 ASTC.register_address 	 = 0x0C;
	 TS1.register_address 	 = 0x0D;
	 TS2.register_address 	 = 0x0E;
	 I2CDIS.register_address = 0x0F;
	 ASAX.register_address 	 = 0x10;
	 ASAY.register_address	 = 0x11;
	 ASAZ.register_address   = 0x12;
}

/*
 * @brief:  Internal driver function used to initialize the i2c peripheral chosen by the user
 * @param:  What I2C peripheral will be used (I2C1, I2C2, I2C3)
 * @retval: None
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
 * @retval: None
 */
void MPU_WRITE(MPU_REGISTER mpu_r, uint8_t addr){
	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)&mpu_r, sizeof(mpu_r), HAL_MAX_DELAY);
}


/* @brief:	MPU_READ is used by the high-level methods, like MPU_ReadAccelerometer, for read configuration parameters and data registers
 * 			The user can read by himself some device register just by sending the desired address registers
 * 			Use this function only if you know how to properly configure the MPU register
 * @param:  mpu_r - MPU specific register that will be read some configuration or information
 * 			number_of_bytes - Number of bytes that will be read from MPU register
 * @retval: Information read from the register specified at mpu_r parameter
 */
uint8_t MPU_READ(MPU_REGISTER mpu_r, uint8_t number_of_bytes, uint8_t addr){

	uint8_t data_register;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)&mpu_r.register_address, sizeof(mpu_r.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)&data_register, number_of_bytes, HAL_MAX_DELAY);
	return data_register;
}


/*
 * @brief:  Function to read last temperature data
 * @param:  None
 * @retval: Raw information that is coming from temperature register
 */
int16_t MPU_Temperature_Read(){

//	int16_t data_return;

//	data_return = (int16_t)(MPU_READ(TEMP_OUT_H) << 8);
//	data_return |= MPU_READ(TEMP_OUT_L);
//	return data_return;
	return 0;
}

/*
 * @brief:	Return the device identity
 * @param:  None
 * @retval: Device identity
 */
uint8_t MPU_Identity(){

	return MPU_READ(WHO_AM_I,1,addr_used);
}

/* @brief:  Function to read last accelerometer data
 * @param:  axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS
 * @retval: Raw information that is coming from MPU accelerometer ADC
 */
int16_t MPU_AccelRead(uint8_t axis){

	int16_t data_return;

	if(axis == X_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_XOUT_H,1,addr_used) << 8);
		data_return |= MPU_READ(ACCEL_XOUT_L,1,addr_used);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(ACCEL_YOUT_H,1,addr_used) << 8);
		data_return |= MPU_READ(ACCEL_YOUT_L,1,addr_used);
	}
	else{
		data_return = (int16_t)(MPU_READ(ACCEL_ZOUT_H,1,addr_used) << 8);
		data_return |= MPU_READ(ACCEL_ZOUT_L,1,addr_used);
	}
	return data_return;
}

/* @brief:  Function used to change the sensitivity of the accelerometer at any desired instant
 * @param:  New sensitivity that will be used, can be one value of @MPU_ACCEL_SCALE enum
 * @retval: None
 */
void MPU_AccelScaleChange(MPU_ACCEL_SCALE new_scale){

	ACCEL_CONFIG.data_cmd |= new_scale << 3;
	AccelScaleConfig(new_scale);

	MPU_WRITE(ACCEL_CONFIG,addr_used);
}

/*
 *@brief   Configure the accelerometer resolution
 * 		   Greater the range that the accelerometer must read, smaller the resolution
 *@param:  accel_scale: A @MPU_ACCEL_SCALE parameter that defines the accelerometer sensitivity
 *@retval: None
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
 * @brief:  Change the information read by the accelerometer from ADC resolution to g
 * 		    resolution = 1/accel_sensitivity
 * @param:  raw_data_read: Information that is coming from accelerometer ADC
 * @retval: Meaningful acceleration data
 */
float MPU_AccelTransformRead(int16_t raw_data_read){

	if(!USE_SI)
		return (float)(raw_data_read * (1.0/accel_sensitivity_used));
	else
		return (float)(raw_data_read * (1.0/accel_sensitivity_used))*SI_ACCELERATION;
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
 * 			A_DLPF_CFG:   - accelerometer low pass filter configuration as the table above, can be one value of @DLPF
 * @retval: None
 */
void MPU_AccelLowPassFilterConfig(uint8_t ACCEL_FCHOICE, DLPF A_DLPF_CFG){

	ACCEL_CONFIG2.data_cmd |= ACCEL_FCHOICE << 3;

	ACCEL_CONFIG2.data_cmd |= A_DLPF_CFG;

	MPU_WRITE(ACCEL_CONFIG2,addr_used);
}

/* @brief: Used to remove DC bias from accel sensor data output, the values in these registers are subtracted from the accel going into the sensor registers
 *
 * @param:
 *			axis  - Must be one value of @AXIS enum
 *			value - Value that should be subtracted from the accel data output
 *					If the value that are displaying at one axis is x m/s², and the desired value is 0 m/s², the value parameter must be -x
 * @retval: None
 */
void MPU_AccelOffset(uint8_t axis, float value){

	int16_t raw_data;


	MPU_REGISTER OFFSET_ACCEL_L;
	MPU_REGISTER OFFSET_ACCEL_H;

	if(axis == X_AXIS){

		raw_data = (int16_t)(value + accelx_factory_trim) * 2048;
		memcpy(&OFFSET_ACCEL_H, &XA_OFFSET_H, sizeof(XA_OFFSET_H));
		memcpy(&OFFSET_ACCEL_L, &XA_OFFSET_L, sizeof(XA_OFFSET_L));
	}
	else if(axis == Y_AXIS){

		raw_data = (int16_t)(value + accely_factory_trim) * 2048;
		memcpy(&OFFSET_ACCEL_H, &YA_OFFSET_H, sizeof(YA_OFFSET_H));
		memcpy(&OFFSET_ACCEL_L, &YA_OFFSET_L, sizeof(YA_OFFSET_L));
	}
	else{

		raw_data = (int16_t)(value + accelz_factory_trim) * 2048;
		memcpy(&OFFSET_ACCEL_H, &ZA_OFFSET_H, sizeof(ZA_OFFSET_H));
		memcpy(&OFFSET_ACCEL_L, &ZA_OFFSET_L, sizeof(ZA_OFFSET_L));
	}

	OFFSET_ACCEL_H.data_cmd = 0;
	OFFSET_ACCEL_L.data_cmd = 0;

	OFFSET_ACCEL_H.data_cmd |= (raw_data >> 8) & 0xFF;
	OFFSET_ACCEL_L.data_cmd |= raw_data & 0xFF;

	MPU_WRITE(OFFSET_ACCEL_H, addr_used);
	MPU_WRITE(OFFSET_ACCEL_L, addr_used);
}

/*@brief: 	Function to read last gyroscope data
 *@param: 	axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS
 *@retval: 	Raw information that is coming from MPU gyroscope ADC
 */
int16_t MPU_GyroRead(uint8_t axis){

	int16_t data_return = 0;

	if(axis == X_AXIS){
		data_return = (int16_t)(MPU_READ(GYRO_XOUT_H, 1, addr_used) << 8);
		data_return |= MPU_READ(GYRO_XOUT_L, 1, addr_used);
	}
	else if(axis == Y_AXIS){
		data_return = (int16_t)(MPU_READ(GYRO_YOUT_H, 1, addr_used) << 8);
		data_return |= MPU_READ(GYRO_YOUT_L, 1, addr_used);
	}
	else{
		data_return = (int16_t)(MPU_READ(GYRO_ZOUT_H, 1, addr_used) << 8);
		data_return |= MPU_READ(GYRO_ZOUT_L, 1, addr_used);
	}
	return data_return;
}

/*
 *
 * @brief:	Configure the gyroscope resolution
 * 		  	Greater the range that the gyroscope must read, smaller the resolution
 * @param: 	gyro_scale: A MPU_GYRO_SCALE parameter tha defines the gyroscope sensitivity
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

/* @brief: Function used to change the sensitivity of the gyroscope at any desired instant
 * @param: New sensitivity that will be used, can be one value of @MPU_GYRO_SCALE enum
 */
void MPU_GyroScaleChange(MPU_GYRO_SCALE new_scale){

	GYRO_CONFIG.data_cmd |= new_scale << 3;
	GyroScaleConfig(new_scale);

	MPU_WRITE(GYRO_CONFIG, addr_used);
}

/*
 *@brief:  Change the information read by the gyroscope ADC to °/s
 * 		   resolution = 1/gyro_sensitivity_used
 *@param:  raw_data_read: Information that is coming from gyroscope ADC
 *@retval: Meaningful angular velocity data
 */
float MPU_GyroTransformRead(int16_t raw_data_read){

	return (float)(raw_data_read * (1.0/gyro_sensitivity_used));
}

/*
 *@brief: Digital Low Pass Filter Configuration, the gyroscope and temperature sensor are filtered according to the value of DLPF_CFG and FCHOICE_B
 *		  as shown in the table below:
 *
 *	      FCHOICE     DLPF_CFG				Gyroscope					Temperature Sensor
 *	     <1>  <0>				 BandWith	Delay(ms)	Fs(kHz)		BandWith		Delay(ms)
 *	      x	   1		x		   8800		 0.064		  32		  4000			  0.04
 *	      0    1		x		   3600      0.11		  32		  4000			  0.04
 *	      1    1		0		    250      0.97		   8		  4000			  0.04
 *	      1    1		1			184      2.9	       1		   188			  1.9
 *	      1    1		2			92       3.9		   1		    98			  2.8
 *	      1    1		3			41       5.9		   1			42			  4.8
 *	      1    1		4			20       9.9		   1			20			  8.3
 *	      1    1 		5			10       17.85		   1			10			  13.4
 *	      1    1		6			 5       33.48		   1			 5			  18.6
 *	      1    1		7		   3600		 0.17		   8		  4000			  0.04
 *@param:  FCHOICE: Used to bypass DLPF as show in the table above
 *		   DLPG_CFG: For the digital low pass filter to be used, can be one value of @DLPF
 *@retval: None
 */
void MPU_GyroTempLowPassFilterConfig(uint8_t FCHOICE, DLPF DLPF_CFG){

	CONFIG.data_cmd |= DLPF_CFG;
	MPU_WRITE(CONFIG, addr_used);

	GYRO_CONFIG.data_cmd |= FCHOICE;
	MPU_WRITE(GYRO_CONFIG, addr_used);
}

/* @brief: Used to remove DC bias from gyro sensor data output, the values in these registers are subtracted from the gyro going into the sensor registers
 * @param:
 *			axis  - Must be one value of @AXIS enum
 *			value - Value that should be subtracted from the gyro data output
 *					If the value that are displaying at one axis is x °/s, and the desired value is 0 °/s, the value parameter must be -x
 * @retval: None
 */
void MPU_GyroOffset(AXIS axis, float value){
	int16_t raw_data = (int16_t)(value * 32.8);		//According with the application note of InvenSense, the value of the bias inputed needs to be in +-1000dps sensitivity range

	MPU_REGISTER OFFSET_TO_SEND_H;
	MPU_REGISTER OFFSET_TO_SEND_L;

	if(axis == X_AXIS){

		memcpy(&OFFSET_TO_SEND_H, &XG_OFFSET_H, sizeof(XG_OFFSET_H));
		memcpy(&OFFSET_TO_SEND_L, &XG_OFFSET_L, sizeof(XG_OFFSET_L));
	}
	else if(axis == Y_AXIS){

		memcpy(&OFFSET_TO_SEND_H, &YG_OFFSET_H, sizeof(YG_OFFSET_H));
		memcpy(&OFFSET_TO_SEND_L, &YG_OFFSET_L, sizeof(YG_OFFSET_L));
	}
	else{

		memcpy(&OFFSET_TO_SEND_H, &ZG_OFFSET_H, sizeof(ZG_OFFSET_H));
		memcpy(&OFFSET_TO_SEND_L, &ZG_OFFSET_L, sizeof(ZG_OFFSET_L));
	}

	OFFSET_TO_SEND_H.data_cmd = 0;
	OFFSET_TO_SEND_L.data_cmd = 0;
	OFFSET_TO_SEND_H.data_cmd |= (raw_data >> 8) && 0xff;
	OFFSET_TO_SEND_L.data_cmd |= (raw_data & 0xFF);

	MPU_WRITE(OFFSET_TO_SEND_H, addr_used);
	MPU_WRITE(OFFSET_TO_SEND_L, addr_used);
}

/*
 * @brief: Configuration of FIFO mode of operation and component enabling
 * @param:
 * 			If:
 *				enable_mpu_components[7] = 1 ->  temperature data (TEMP_OUT_H, TEMP_OUT_L)    will be buffered at fifo, even if data path is in standby
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

	USER_CTRL.data_cmd |= 1 << 6;						//FIFO Enable
	USER_CTRL.data_cmd |= 1 << 2;						//Reset Fifo module
	MPU_WRITE(USER_CTRL, addr_used);

	FIFO_EN.data_cmd |= enable_mpu_components;		//controls what data will be put at fifo
	MPU_WRITE(FIFO_EN, addr_used);

	CONFIG.data_cmd |= fifo_mode << 6;					//controls if new data override or not the oldest
	MPU_WRITE(CONFIG, addr_used);
}

/*
 * @brief:  Controls the number of bytes available at FIFO
 * 		    If the register FIFO_COUNTH is sent, the return is two bytes that contains the number of words writes at fifo, but at the first byte that is
 * 		    coming only the five first bits are relevant information, so a mask was applied (0x1F) to disregard the 3 lasts bits
 * @param:  None
 * @retval: Number of bytes writes at FIFO
 *
 */
int16_t MPU_FifoCounter(){

	uint8_t data_register[2] = {0};
	uint16_t to_return;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&FIFO_COUNTH.register_address, sizeof(FIFO_COUNTH.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

	to_return = ((uint16_t)(data_register[0] & 0x1F) << 8) | data_register[1];
	return to_return;
}

/*
 * @brief: Returns the data on the top of fifo
 *		   Informations are written at fifo with register address order, for example, if temperature sensor and gyroscope is configured to write at fifo
 *		   the first data that will be written are temperature data because it register address is smaller than gyroscope register address
 *		   For enable or disable one component to write at fifo, change @enable_mpu_components at @MPU_FIFOConfig function
 *
 *
 * @param: None
 * @retval: One signed byte that represents data of gyroscope, temperature sensor or accelerometer
 */
int16_t MPU_FifoReadData(){

	uint8_t data_register[2] = {0};
	uint16_t to_return;

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&FIFO_R_W.register_address, sizeof(FIFO_R_W.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr_used << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

	to_return = ((uint16_t)(data_register[0]  << 8)) | data_register[1];
	return to_return;
}


/*
 *@brief: Device information for Magnetometer
 *@param: None
 *@retval: Device information for Magnetometer
 */

uint8_t MPU_MagGetInfo(){

	return MPU_READ(INFO, 1, AK8963_ADDR);
}


/*
 *@brief: Register status 1 for magnetometer
 *		  BIT 0: DRDY (Data Ready) = "0" Normal; "1" Data is ready
 *		  		 Bit turns "1" when data is ready in single measurement mode or self-test mode. It returns "0" when any one of ST2 register or measurement
 *		  		 data register (HXL to HZH) is read
 *		  BIT 1: DOR (Data Overrun) = "0" Normal; "1" Data overrun
 *		  		 DOR bit turns to "1" when data has been skipped in continuous measurement mode or external trigger measurement model mode. It returns to "0"
 *		  		 when any one of ST2 register or measurement data register is read
 *@param: None
 *@retval: One byte of data with BIT 0 and BIT 1 with meaningful data has described above
 */
uint8_t MPU_MagGetStatus1(){

	return MPU_READ(ST1, 1, AK8963_ADDR);
}

/*
 *@brief: Register status 2 for magnetometer
 *		BIT 3: HOFL (Magnetic sensor overflow) = "0" Normal; "1" Magnetic sensor overflow occurred
 *				In single measurement mode, continuous measurement mode, external trigger measurement mode and self-test mode, magnetic
 *				sensor may overflow even though measurement data register is not saturated. In this case, measurement data is not correct and HOFL
 *s				bit turns to “1”. When next measurement stars, it returns to “0”
 *		BIT 4: BITM (Output bit setting (mirror) ) = "0" 14-bit output; "1" 16-bit output
 *				Mirror data of BIT bit of CNTL1 register. ST2 register has a role as data reading end register, also. When any of measurement data
 *				register is read in continuous measurement mode or external trigger measurement mode, it means data reading start and taken as  data
 *				reading  until  ST2  register  is  read.  Therefore,  when  any  of  measurement  data  is  read,  be  sure to read ST2 register at the end.
 *
 *@param: None
 *@retval: One byte of data with BIT 3 and BIT 4 with meaningful data has described above
 *
 */
uint8_t MPU_MagGetStatus2(){

	return MPU_READ(ST2, 1, AK8963_ADDR);
}

/*
 *@brief: Measurement data of magnetic sensor X-axis/Y-axis/Z-axis
 *@param: Desired axis
 *@retval: information that is coming from MPU magnetometer em uT
 */
int16_t MPU_MagRead(AXIS axis){
	int16_t to_return;

	if(axis == X_AXIS){
		int16_t magx;
		magx = (MPU_READ(HXH, 1, AK8963_ADDR) << 8) | MPU_READ(HXL, 1, AK8963_ADDR);
		to_return = magx * ( (magx_Adj - 128)/256.0  + 1 );

	}
	else if(axis == Y_AXIS){
		int16_t magy;
		magy = (MPU_READ(HYH, 1, AK8963_ADDR) << 8) | MPU_READ(HYL, 1, AK8963_ADDR);
		to_return = magy * ( (magy_Adj - 128)/256.0  + 1 );

	}else{
		int16_t magz;
		magz = (MPU_READ(HZH, 1, AK8963_ADDR) << 8) | MPU_READ(HZL, 1, AK8963_ADDR);
		to_return = magz * ( (magz_Adj - 128)/256.0  + 1 );
	}
	return to_return;
}

/*
 *@brief: Device ID of AKM. It is described in one byte and fixed value
 *@param: None
 *@retval: Device ID of AKM
 *
 */
uint8_t MPU_MagWhoAmI(){

	return MPU_READ(WIA, 1, AK8963_ADDR);
}


/*
 *@brief: Configuration mode of magnetometer
 *@param:
 *		mode - An @MPU_MAG_OPMODE value that describe one operation mode, when each mode is set, AK8963 transits to set mode
 *		output_mode - An @MPU_MAG_OUTPUT_SETTING
 *@retval:None
 *
 */
void MPU_MagConfigControl(MPU_MAG_OPMODE mode, MPU_MAG_OUTPUT_SETTING output_mde){

	CNTL.data_cmd = mode | (output_mde << 5);
	MPU_WRITE(CNTL, AK8963_ADDR);
}


/*
 * @brief: his  register  disables  I2C  bus  interface.  I2C  bus  interface  is  enabled  in  default.
 * Once I2C bus interface is disabled, it is impossible to write other value to I2CDIS register. To enable I2C bus interface,
 * reset AK8963 or input start condition 8 times continuously
 * @param: None
 * @retval: None
 */
void MPU_MagI2CDisable(){
	I2CDIS.data_cmd = 0b00011011;						//AS THE RM defines
	MPU_WRITE(I2CDIS, AK8963_ADDR);
}

/*
 * @brief: Disable one or more MPU components
 * @param:
 * 		  disable_accel can be one value of @MPU_DISABLE_AXIS
 *		  disable_gyroscope can be one value of @MPU_DISABLE_AXIS
 *
 * @retval: None
 */
void MPU_DisableComponents(MPU_DISABLE_AXIS disable_accel, MPU_DISABLE_AXIS disable_gyroscope){

	PWR_MGMT_2.data_cmd = (disable_accel << 3) | disable_gyroscope;;		//information of what accel axis (last three bits) and gyro axis(first three bits) must be disable
	MPU_WRITE(PWR_MGMT_2,addr_used);
}

/* @brief:  Reset all gyro, accel and digital temp signal path. This bit also clears all the data sensor registers
 * @param:  None
 * @retval: None
 */
void MPU_ResetDataRegisters(){

	USER_CTRL.data_cmd |= 1 << 0;
	MPU_WRITE(USER_CTRL, addr_used);
	USER_CTRL.data_cmd &= ~(1 << 0);
}

/* @brief: Reset one or more sensor data path, the sensor register will not be cleared, for this, use @MPU_SignalPathReset
 * @param: sensor_to_reset: One value of @RESET_SENSOR_SIGNAL_PATH
 * @retval: None
 *
 */
void MPU_SignalPathReset(RESET_SENSOR_SIGNAL_PATH sensor_to_reset){

	SIGNAL_PATH_RESET.data_cmd  = sensor_to_reset;
	MPU_WRITE(SIGNAL_PATH_RESET, addr_used);
	SIGNAL_PATH_RESET.data_cmd  = 0;
}

/* @brief:  Reset internal registers and restores default settings
 * @param:  None
 * @retval: None
 */
void MPU_ResetWholeIC(){

	PWR_MGMT_1.data_cmd |= 1 << 7;
	MPU_WRITE(PWR_MGMT_1, addr_used);
	PWR_MGMT_1.data_cmd &= ~(1 << 7);
}
