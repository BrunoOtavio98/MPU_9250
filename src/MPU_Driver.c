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
#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define G 9.8065

static void I2C_Initialization(uint8_t I2Cx);
static void AccelScaleConfig(MPU_ACCEL_SCALE accel_scale);
static void GyroScaleConfig(MPU_GYRO_SCALE gyro_scale);
static void Register_Initialization();
static float MPU_AccelTransformRead(int16_t raw_data_read);
static float MPU_GyroTransformRead(int16_t raw_data_read);
static void MPU_MagConfigControl(MPU_MAG_OPMODE mode, MPU_MAG_OUTPUT_SETTING output_mde);
static uint8_t oneAxisAccelCalibration(AXIS axis, uint8_t up, uint16_t numberOfSamples, float *accumulatedx, float *accumulatedy, float *accumulatedz, UART_HandleTypeDef *uart, char strDebug[]);
static float AccelRawReading(uint8_t axis);
static float GyroRawReading(uint8_t axis);
static void accelCalibration(float *rawData);
static void matrixMult(float *m1, float *m2, float *mr, int m1Line, int m1Column, int m2Column);
static void matrixTransp(float *m1, float *transp, int lines, int columns);
static void matrixInv(float *mfrom, float *mto, int lines, int columns);
static void matrixCopy(float *mfrom, float *mto, int lines, int columns);
static void eye(float *m, int lines, int columns);

static void hardCodedAccelParam();

void __MAG_WRITE(MPU_REGISTER reg_to_write);
uint8_t __MAG_READ(MPU_REGISTER reg_to_read, uint8_t bytes, uint8_t data_vet[]);
void __MPU_WRITE(MPU_REGISTER mpu_r, uint8_t addr);
void __MPU_READ(MPU_REGISTER mpu_r,uint8_t number_of_bytes, uint8_t data_return[], uint8_t addr);

uint8_t MPU_ADDR_USED;									// holds the mpu i2c address

static float gyroxStaticBias;
static float gyroyStaticBias;
static float gyrozStaticBias;
static uint8_t flagGyroCalibrated = 0;

static uint8_t flagAccelCalibrated = 0;
static uint8_t flagMagCalibrated = 0;

static float *accelCalibrationParam;
static float magCalibrationParam[6];

static float X0;
static float Y0;
static float Z0;

static float M_OSx;
static float M_OSy;
static float M_OSz;

static float A,B,C;

static float M_SCx;
static float M_SCy;
static float M_SCz;

/*
 * @brief: MPU initialization function
 * @param: i2c - Specify what i2c peripheral will be used, the values can be ( USE_I2C1, USE_I2C2 or USE_I2C3 )
 * 		   mpu_i2c_addr - For choose what i2c address will be used, if AD0 = HIGH -> mpu_i2c_addr = ADDR_2, otherwise mpu_i2c_addr = ADDR_1
 *		   accel_scale  - Specify the sensitivity of the accelerometer, choose a value at @MPU_ACCEL_SCALE enum
 *		   gyro_scale   - Specify the sensitivity of the gyroscope, choose a value at @MPU_GYRO_SCALE enum
 *		   enable_mag   - true if magnetometer must be enabled, false otherwise
 */
void MPU_Init(uint8_t i2c, uint8_t mpu_i2c_addr, MPU_ACCEL_SCALE accel_scale, MPU_GYRO_SCALE gyro_scale){

	I2C_Initialization(i2c);

	Register_Initialization();

	if(mpu_i2c_addr == USE_ADDR1)
		MPU_ADDR_USED = ACCELGYRO_ADDR_1;
	else
		MPU_ADDR_USED = ACCELGYRO_ADDR_2;

	ACCEL_CONFIG.data_cmd |= accel_scale 	<< 3;
	GYRO_CONFIG.data_cmd  |= gyro_scale     << 3;

	AccelScaleConfig(accel_scale);
	GyroScaleConfig(gyro_scale);

	__MPU_WRITE(ACCEL_CONFIG, MPU_ADDR_USED);
	__MPU_WRITE(GYRO_CONFIG, MPU_ADDR_USED);

	HAL_Delay(1);								/* To change from power-down mode to another mode, its necessary at least 100 us (AK8963 datasheet Rev. 10/2013) */
	MPU_MagConfigControl(MAG_CONTINUOUS_MEASUREMENT2, _16_BIT);

	MPU_GyroCalibrate(1000);

	accelCalibrationParam = malloc(4 * 3 * sizeof(float));


	hardCodedAccelParam();

}

static void hardCodedAccelParam()
{

	accelCalibrationParam[0] = 1.0041;
	accelCalibrationParam[1] = 0.017506;
	accelCalibrationParam[2] = -0.083167;

	accelCalibrationParam[3] = 0.0164;
	accelCalibrationParam[4] = 1.0067;
	accelCalibrationParam[5] = 0.029136;

	accelCalibrationParam[6] = -0.025811;
	accelCalibrationParam[7] = 0.040776;
	accelCalibrationParam[8] = 0.99886;

	accelCalibrationParam[9] = -0.59662;
	accelCalibrationParam[10] = -0.12791;
	accelCalibrationParam[11] = 1.4257;

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
	 CNTL1.register_address  = 0x0A;
	 CNTL2.register_address  = 0x0B;
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
 * @brief:	__MPU_WRITE is used by the high-level methods for send configuration parameters
 * 			The user can change by himself some MPU registers just writing the corrects parameters
 * 			at the buffer of the desired command.
 * 			Use this function only if you know how to properly configure the MPU registers
 * @param:  mpu_r - MPU specific register where some configuration will be written
 * @retval: None
 */
void __MPU_WRITE(MPU_REGISTER mpu_r, uint8_t addr){
	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)&mpu_r, sizeof(mpu_r), HAL_MAX_DELAY);
}


/* @brief:	__MPU_READ is used by the high-level methods, like __MPU_READAccelerometer, for read configuration parameters and data registers
 * 			The user can read by himself some device register just by sending the desired address registers
 * 			Use this function only if you know how to properly configure the MPU registers
 * @param:  mpu_r - MPU specific register that will be read some configuration or information
 * 			number_of_bytes - Number of bytes that will be read from MPU register
 * @retval: Information read from the register specified at mpu_r parameter
 */
void __MPU_READ(MPU_REGISTER mpu_r, uint8_t number_of_bytes, uint8_t *data_return, uint8_t addr){

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)&mpu_r.register_address, sizeof(mpu_r.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(addr << 1), (uint8_t*)data_return, number_of_bytes, HAL_MAX_DELAY);
}

/*
 * @brief:  Function to read last temperature data
 * @param:  None
 * @retval: IC temperature
 */
float MPU_Temperature_Read(){

	uint8_t raw_temp[2];
	int16_t signed_raw;
	float tempSensor;

	__MPU_READ(TEMP_OUT_H, 2, raw_temp, MPU_ADDR_USED);

	signed_raw = raw_temp[0] << 8 | raw_temp[1];

	tempSensor = signed_raw/TEMP_SENSITIVITY + 21;

	return tempSensor;
}


/*
 *	@brief: Read all sensors at once
 *	@param:
 *			accel_data: three element float vector where acceleration data will be placed
 *			giro_data: three element float vector where gyroscope data will be placed
 *			mag_data: three element float vector where magnetometer data will be placed
 *	@retval: Code error TODO
*/
uint8_t MPU_ReadAllSensores(float accel_data[], float gyro_data[], float mag_data[])
{

	int16_t raw_data[6];
	uint8_t return_data[14];
	float rawAccel[4];


	if(!flagAccelCalibrated)
	{
		M_OSx = M_OSy = M_OSz = 0;
		M_SCx = M_SCy = M_SCz = 1;
	}

	__MPU_READ(ACCEL_XOUT_H, 14, return_data, MPU_ADDR_USED);

	raw_data[0] =  return_data[0] << 8 | return_data[1];
	raw_data[1] =  return_data[2] << 8 | return_data[3];
	raw_data[2] =  return_data[4] << 8 | return_data[5];

	raw_data[3] =  return_data[8] << 8 | return_data[9];
	raw_data[4] =  return_data[10] << 8 | return_data[11];
	raw_data[5] =  return_data[12] << 8 | return_data[13];


	rawAccel[0] = MPU_AccelTransformRead(raw_data[0]);
	rawAccel[1] = MPU_AccelTransformRead(raw_data[1]);
	rawAccel[2] = MPU_AccelTransformRead(raw_data[2]);
	rawAccel[3] = 1;

	matrixMult(rawAccel, accelCalibrationParam, accel_data, 1, 4, 3);

	gyro_data[0] = MPU_GyroTransformRead(raw_data[3]) - gyroxStaticBias;
	gyro_data[1] = MPU_GyroTransformRead(raw_data[4]) - gyroyStaticBias;
	gyro_data[2] = MPU_GyroTransformRead(raw_data[5]) - gyrozStaticBias;

	uint8_t mag_return[6];
	int16_t raw_mag_data[3];
	uint8_t st;

	HAL_Delay(1);
	__MAG_READ(ST1, 1, &st);

	if(st & 0x01 || st & 0x11){

	__MAG_READ(HXL, 6, mag_return);

	raw_mag_data[0] = mag_return[0] | mag_return[1] << 8;
	raw_mag_data[1] = mag_return[2] | mag_return[3] << 8;
	raw_mag_data[2] = mag_return[4] | mag_return[5] << 8;

	mag_data[0] = (AK8963_SENSITIVITY * raw_mag_data[0] * magx_Adj - M_OSx)/M_SCx;
	mag_data[1] = (AK8963_SENSITIVITY * raw_mag_data[1] * magy_Adj - M_OSy)/M_SCy;
	mag_data[2] = (AK8963_SENSITIVITY * raw_mag_data[2] * magz_Adj - M_OSz)/M_SCz;

	}

	__MAG_READ(ST2, 1, &st);

	return 0;
}


/*
 * @brief:	Return the device identity
 * @param:  None
 * @retval: Device identityss
 */
uint8_t MPU_WhoAmI(){

	uint8_t mpu_identity;

	__MPU_READ(WHO_AM_I,1, &mpu_identity, MPU_ADDR_USED);

	return mpu_identity;
}

/* @brief:  Function to read last accelerometer data
 * @param:  axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS
 * @retval: Raw information that is coming from MPU accelerometer ADC
 */
float MPU_AccelRead(uint8_t axis){

	int16_t data_return;
	uint8_t accel_data[6];
	float accelx, accely, accelz;
	float rawAccel[3];
	float calAccel[3];

	__MPU_READ(ACCEL_XOUT_H, 6, accel_data, MPU_ADDR_USED);

	data_return = accel_data[0] << 8 | accel_data[1];
	accelx = MPU_AccelTransformRead(data_return);

	data_return = accel_data[2] << 8 | accel_data[3];
	accely = MPU_AccelTransformRead(data_return);

	data_return = accel_data[4] << 8 | accel_data[5];
	accelz = MPU_AccelTransformRead(data_return);

	rawAccel[0] = accelx;
	rawAccel[1] = accely;
	rawAccel[2] = accelz;
	rawAccel[3] = 1;

	matrixMult(rawAccel, accelCalibrationParam, calAccel, 1, 4, 3);
	if(axis == X_AXIS){
		return calAccel[0];
	}
	else if(axis == Y_AXIS){
		return calAccel[1];
	}
	else{
		return calAccel[2];
	}

}

static float AccelRawReading(uint8_t axis)
{
	int16_t data_return;
	uint8_t accel_data[2];

	if(axis == X_AXIS){
		__MPU_READ(ACCEL_XOUT_H, 2, accel_data, MPU_ADDR_USED);
		data_return = accel_data[0] << 8 | accel_data[1];

		return MPU_AccelTransformRead(data_return);
	}
	else if(axis == Y_AXIS){
		__MPU_READ(ACCEL_YOUT_H, 2, accel_data, MPU_ADDR_USED);
		data_return = accel_data[0] << 8 | accel_data[1];

		return MPU_AccelTransformRead(data_return);
	}
	else{
		__MPU_READ(ACCEL_ZOUT_H, 2, accel_data, MPU_ADDR_USED);
		data_return = accel_data[0] << 8 | accel_data[1];

		return MPU_AccelTransformRead(data_return);
	}
}

/* @brief:  Function used to change the sensitivity of the accelerometer at any desired instant
 * @param:  New sensitivity that will be used, can be one value of @MPU_ACCEL_SCALE enum
 * @retval: None
 */
void MPU_AccelScaleChange(MPU_ACCEL_SCALE new_scale){

	ACCEL_CONFIG.data_cmd |= new_scale << 3;
	AccelScaleConfig(new_scale);

	__MPU_WRITE(ACCEL_CONFIG, MPU_ADDR_USED);
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
static float MPU_AccelTransformRead(int16_t raw_data_read){

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

	__MPU_WRITE(ACCEL_CONFIG2, MPU_ADDR_USED);
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

	__MPU_WRITE(OFFSET_ACCEL_H, MPU_ADDR_USED);
	__MPU_WRITE(OFFSET_ACCEL_L, MPU_ADDR_USED);
}

/*
 *	@brief: Accelerometer calibration
 *			1) Z pointing up (+9.8)
 *			2) Z pointing down (-9.8)
 *			3) x pointing up (+9.8)
 *			4) x pointing down (-9.8)
 *			5) y pointing up (+9.8)
 *			6) y pointing down (-9.8)
 *	The algorithm will wait if some of this steps above is not guarantee in the correct step
 *	The algorithm used was based on the Application note 3192 (Using LSM303DLH for a tilt compensated electronic compass) from STMicroeletronics.
 *
 *	@param:
 *			numberOfSamples: Number of accelerometer data
 *			uart: To inform the user what step is in the process
 *
 *	@return: See MPU_GetFlagAccelCalibrated
 */
void MPU_AccelCalibrate(uint16_t numberOfSamples, UART_HandleTypeDef *uart)
{
	float axisXData_XUP = 0.0;
	float axisYData_XUP = 0.0;
	float axisZData_XUP = 0.0;

	float axisXData_XDOWN = 0.0;
	float axisYData_XDOWN = 0.0;
	float axisZData_XDOWN = 0.0;

	float axisXData_YUP = 0.0;
	float axisYData_YUP = 0.0;
	float axisZData_YUP = 0.0;

	float axisXData_YDOWN = 0.0;
	float axisYData_YDOWN = 0.0;
	float axisZData_YDOWN = 0.0;

	float axisXData_ZUP = 0.0;
	float axisYData_ZUP = 0.0;
	float axisZData_ZUP = 0.0;

	float axisXData_ZDOWN = 0.0;
	float axisYData_ZDOWN = 0.0;
	float axisZData_ZDOWN = 0.0;

	uint8_t state = 0;

	char buffer[500];

	snprintf(buffer, sizeof(buffer), "Z axis up now\n");
	HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

	state = oneAxisAccelCalibration(Z_AXIS, 1, numberOfSamples, &axisXData_ZUP, &axisYData_ZUP, &axisZData_ZUP, uart, "Z Up: ");
	if(state)
	{

		snprintf(buffer, sizeof(buffer),  "Z axis down now\n");
		HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

		state = oneAxisAccelCalibration(Z_AXIS, 0, numberOfSamples, &axisXData_ZDOWN, &axisYData_ZDOWN, &axisZData_ZDOWN, uart, "Z Down: ");
		if(state)
		{

			snprintf(buffer, sizeof(buffer),  "Y axis up now\n");
			HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

			state = oneAxisAccelCalibration(Y_AXIS, 1, numberOfSamples, &axisXData_YUP, &axisYData_YUP, &axisZData_YUP, uart, "Y Up: ");
			if(state)
			{

				snprintf(buffer, sizeof(buffer),  "Y axis down now\n");
				HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

				state = oneAxisAccelCalibration(Y_AXIS, 0, numberOfSamples, &axisXData_YDOWN, &axisYData_YDOWN, &axisZData_YDOWN, uart, "Y Down: ");
				if(state)
				{

					snprintf(buffer, sizeof(buffer),  "X axis up now\n");
					HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

					state = oneAxisAccelCalibration(X_AXIS, 1, numberOfSamples, &axisXData_XUP, &axisYData_XUP, &axisZData_XUP, uart, "X Up: ");
					if(state)
					{
						snprintf(buffer, sizeof(buffer),  "X axis down now\n");
						HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

						state = oneAxisAccelCalibration(X_AXIS, 0, numberOfSamples, &axisXData_XDOWN, &axisYData_XDOWN, &axisZData_XDOWN, uart, "X Down: ");
						if(state)
						{

							float rawData[6][4] = {
									{axisXData_ZUP/numberOfSamples, axisYData_ZUP/numberOfSamples, axisZData_ZUP/numberOfSamples, 1},
									{axisXData_ZDOWN/numberOfSamples, axisYData_ZDOWN/numberOfSamples, axisZData_ZDOWN/numberOfSamples, 1},
									{axisXData_YUP/numberOfSamples, axisYData_YUP/numberOfSamples, axisZData_YUP/numberOfSamples, 1},
									{axisXData_YDOWN/numberOfSamples, axisYData_YDOWN/numberOfSamples, axisZData_YDOWN/numberOfSamples, 1},
									{axisXData_XUP/numberOfSamples, axisYData_XUP/numberOfSamples, axisZData_XUP/numberOfSamples, 1},
									{axisXData_XDOWN/numberOfSamples, axisYData_XDOWN/numberOfSamples, axisZData_XDOWN/numberOfSamples, 1},
							};

							accelCalibration(rawData);
							flagAccelCalibrated = 1;
						}
					}
				}
			}
		}
	}

}

static uint8_t oneAxisAccelCalibration(AXIS axis, uint8_t up, uint16_t numberOfSamples, float *accumulatedx, float *accumulatedy, float *accumulatedz, UART_HandleTypeDef *uart, char strDebug[])
{
	int i = 0;
	float lastRead;

	char buffer[500];
	if(up)
	{
		for( i = 0; i<500000; i++)			/* The maximum time that the user have to turn the axis to the correct way */
		{
			lastRead = AccelRawReading(axis);

			if(lastRead > 9.0 && lastRead < 11.0)
				break;						/* This means that the axis in pointing up so the calibration can continue */

			snprintf(buffer, sizeof(buffer), "%s Value below the required\n", strDebug);
			HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		}

		if(i == 500000)						/* Calibration will not proceed */
		{
			snprintf(buffer, sizeof(buffer), "%s Maximum time expired \n", strDebug);
			HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
			return 0;
		}

		snprintf(buffer, sizeof(buffer), "Sampling started!\n");
		HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

		HAL_Delay(100);

		i = 0;
		while(i < numberOfSamples)
		{
			lastRead = AccelRawReading(axis);
			if(lastRead > 8.5 && lastRead < 11.0)								/* A threshold to consider as bias, if the sensor tested has a threshold greater than this, the threshold have to change */
			{
				snprintf(buffer, sizeof(buffer), "%s sampling %d \n", strDebug, i);
				HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

				*accumulatedx += AccelRawReading(X_AXIS);
				*accumulatedy += AccelRawReading(Y_AXIS);
				*accumulatedz += AccelRawReading(Z_AXIS);

				i++;
			}
			else
			{
				snprintf(buffer, sizeof(buffer), "%s sensor isnt in the correct position %f\n", strDebug, lastRead);
				HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
			}
		}
	}

	else
	{
		for( i = 0; i<500000; i++)			/* The maximum time that the user have to turn the axis to the correct way */
		{
			lastRead = AccelRawReading(axis);

			if(lastRead < -9.0 && lastRead > -11.0)
				break;/* This means that the axis in pointing down so the calibration can continue */

			snprintf(buffer, sizeof(buffer), "%s Value above the required\n", strDebug);
			HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		}

		if(i == 500000)						/* Calibration will not proceed */
		{
			snprintf(buffer, sizeof(buffer), "%s Maximum time expired \n", strDebug);
			HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
			return 0;
		}
		snprintf(buffer, sizeof(buffer), "%s Sampling started!\n", strDebug);
		HAL_Delay(100);																/* These delays that is used in the process are to take in account for the user correct the position required */

		i = 0;
		while(i < numberOfSamples)
		{
			lastRead = AccelRawReading(axis);
			if(lastRead < -8.5 && lastRead > -11)								/* A threshold to consider as bias, if the sensor tested has a threshold greater than this, the threshold have to change */
			{
				snprintf(buffer, sizeof(buffer), "%s sampling %d \n", strDebug, i);
				HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);


				*accumulatedx += AccelRawReading(X_AXIS);
				*accumulatedy += AccelRawReading(Y_AXIS);
				*accumulatedz += AccelRawReading(Z_AXIS);

				i++;
			}
			else
			{
				snprintf(buffer, sizeof(buffer), "%s sensor isnt in the correct position %f \n", strDebug, lastRead);
				HAL_UART_Transmit(uart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
			}
		}
	}
	return 1;
}


/*	@brief: Return the accelerometer calibration state
 *	@param: None
 *	@retval: Flag that indicates the state of the calibration process
 */
uint8_t MPU_GetFlagAccelCalibrated()
{
	return flagAccelCalibrated;
}


static void accelCalibration(float *rawData)
{

	float Y[6][3] =
	{
			{0,0,G},				/* Z UP   */
			{0,0,-G},				/* Z DOWN */
			{0,G,0},				/* Y UP   */
			{0,-G,0},				/* Y DOWN */
			{G,0,0},				/* X UP	  */
			{-G,0,0}				/* X DOWN */
	};

	float *wt = malloc(4 * 6 * sizeof(float));
	float *multW = malloc(4 * 4 * sizeof(float));
	float *inv = malloc(4 * 4 * sizeof(float));
	float *temp = malloc(4 * 6 * sizeof(float));

	matrixTransp(rawData, wt, 6, 4);				/* wt */
	matrixMult(wt, rawData, multW, 4, 6, 4);		/* wt * w */
	matrixInv(multW, inv, 4, 4);					/* [wt * w]^-1 */
	matrixMult(inv, wt, temp, 4, 4, 6);				/* [wt * w]^-1 * wt */
	matrixMult(temp, Y, accelCalibrationParam, 4, 6, 3);	/* Result is the matrix of calibration parameters */

	free(wt);
	free(inv);
	free(multW);
	free(temp);
}


/*@brief: 	Function to read last gyroscope data
 *@param: 	axis - Specify what axis will be read, can be: X_AXIS, Y_AXIS or Z_AXIS
 *@retval: 	Raw information that is coming from MPU gyroscope ADC
 */
float MPU_GyroRead(uint8_t axis){

	int16_t data_return = 0;
	uint8_t giro_data[2];

	if(axis == X_AXIS){
		__MPU_READ(GYRO_XOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return) - gyroxStaticBias;
	}
	else if(axis == Y_AXIS){
		__MPU_READ(GYRO_YOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return) - gyroyStaticBias;
	}
	else{
		__MPU_READ(GYRO_ZOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return) - gyrozStaticBias;
	}

}

static float GyroRawReading(uint8_t axis)
{

	int16_t data_return = 0;
	uint8_t giro_data[2];

	if(axis == X_AXIS){
		__MPU_READ(GYRO_XOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return);
	}
	else if(axis == Y_AXIS){
		__MPU_READ(GYRO_YOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return);
	}
	else{
		__MPU_READ(GYRO_ZOUT_H, 2, giro_data, MPU_ADDR_USED);
		data_return = giro_data[0] << 8 | giro_data[1];
		return MPU_GyroTransformRead(data_return);
	}

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

	__MPU_WRITE(GYRO_CONFIG, MPU_ADDR_USED);
}

/*
 *@brief:  Change the information read by the gyroscope ADC to °/s
 * 		   resolution = 1/gyro_sensitivity_used
 *@param:  raw_data_read: Information that is coming from gyroscope ADC
 *@retval: Meaningful angular velocity data
 */
static float MPU_GyroTransformRead(int16_t raw_data_read){

	return (float)(raw_data_read * (1.0/gyro_sensitivity_used));
}

/*
 * 	@brief: Remove gyro static bias. Dont move mpu when this function is called. 1000 samples will be collected from both axis
 *	@param: Number of samples to be used in the calibration process
 *	@retval: See MPU_GetFlagGyroCalibrated
 */
void MPU_GyroCalibrate(uint16_t numberOfSamples)
{
	flagGyroCalibrated  = 0;

	float gyrox = 0;
	float gyroy = 0;
	float gyroz = 0;
	for(uint16_t i = 0; i<numberOfSamples; i++)
	{
		gyrox += GyroRawReading(X_AXIS);
		gyroy += GyroRawReading(Y_AXIS);
		gyroz += GyroRawReading(Z_AXIS);
	}
	gyroxStaticBias = gyrox/numberOfSamples;
	gyroyStaticBias = gyroy/numberOfSamples;
	gyrozStaticBias = gyroz/numberOfSamples;

	flagGyroCalibrated = 1;
}

/*
 * 	@brief: State of the calibration process
 *	@param: None
 *	@retval: Return the state of the gyro calibration process
 */
uint8_t MPU_GetFlagGyroCalibrated()
{
	return flagGyroCalibrated;
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
	__MPU_WRITE(CONFIG, MPU_ADDR_USED);

	GYRO_CONFIG.data_cmd |= FCHOICE;
	__MPU_WRITE(GYRO_CONFIG, MPU_ADDR_USED);
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

	__MPU_WRITE(OFFSET_TO_SEND_H, MPU_ADDR_USED);
	__MPU_WRITE(OFFSET_TO_SEND_L, MPU_ADDR_USED);
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
	__MPU_WRITE(USER_CTRL, MPU_ADDR_USED);

	FIFO_EN.data_cmd |= enable_mpu_components;		//controls what data will be put at fifo
	__MPU_WRITE(FIFO_EN, MPU_ADDR_USED);

	CONFIG.data_cmd |= fifo_mode << 6;					//controls if new data override or not the oldest
	__MPU_WRITE(CONFIG, MPU_ADDR_USED);
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

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(MPU_ADDR_USED << 1), (uint8_t*)&FIFO_COUNTH.register_address, sizeof(FIFO_COUNTH.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(MPU_ADDR_USED << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

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

	HAL_I2C_Master_Transmit(&mpu_i2c_comm, (uint16_t)(MPU_ADDR_USED << 1), (uint8_t*)&FIFO_R_W.register_address, sizeof(FIFO_R_W.register_address), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&mpu_i2c_comm, (uint16_t)(MPU_ADDR_USED << 1), (uint8_t*)&data_register, 2, HAL_MAX_DELAY);

	to_return = ((uint16_t)(data_register[0]  << 8)) | data_register[1];
	return to_return;
}


/*
 *@brief: Device information for Magnetometer
 *@param: None
 *@retval: Device information for Magnetometer
 */

uint8_t MPU_MagGetInfo(){

	uint8_t info;

	__MAG_READ(INFO, 1, &info);
	return info;
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

	uint8_t status;

	__MAG_READ(ST1,1,&status);
	return status;
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

	uint8_t status;

	__MAG_READ(ST2, 1, &status);
	return status;
}

/*
 *@brief: Measurement data of magnetic sensor X-axis/Y-axis/Z-axis
  *	Steps accordingly with Ak8963 Datasheet (Rev. 2013/10):
 *		1) Check data ready of not by any of the following method.
 *		   - Polling DRDY bit of ST1 register
 *		   - Monitor DRDY pin
 *		2) Read ST1 register
 *		   - DRDY: Shows Data ready or not.
 *		   - DOR: Shows if any data has been skipped before the current data or not
 *		3) Read measurement data
 *		4) Read ST2 register
 *		   - HOFL: :Shows if magnetic sensor is overflown or not
 *@param: Desired axis
 *@retval: information that is coming from MPU magnetometer em uT
 */
float MPU_MagRead(AXIS axis){
	float to_return = -60000;

	uint8_t st;

	__MAG_READ(ST1, 1, &st);

	if(!flagAccelCalibrated)
	{
		M_OSx = M_OSy = M_OSz = 0;
		M_SCx = M_SCy = M_SCz = 1;
	}

	if(st & 0x01){							/* Check if DRDY bit was set to 1 */

		uint8_t mag_vet[2];

		if(axis == X_AXIS){
			int16_t magx;

		   __MAG_READ(HXL, 2, mag_vet);

			magx = mag_vet[1] << 8 | mag_vet[0];
			to_return = AK8963_SENSITIVITY * magx * magx_Adj;
			to_return = (to_return - M_OSx)/M_SCx;

		}
		else if(axis == Y_AXIS){
			int16_t magy;

			__MAG_READ(HYL, 2, mag_vet);

			magy = mag_vet[1] << 8 | mag_vet[0];
			to_return = AK8963_SENSITIVITY * magy * magy_Adj;
			to_return = (to_return - M_OSy)/M_SCy;
		}else{
			int16_t magz;

			 __MAG_READ(HZL, 2, mag_vet);

			magz = mag_vet[1] << 8 | mag_vet[0];
			to_return = AK8963_SENSITIVITY * magz * magz_Adj;
			to_return = (to_return - M_OSz)/M_SCz;
		}
		__MAG_READ(ST2, 1, &st);
	}

	else if(st & 0x10){
	__MAG_READ(ST2, 1, &st);
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

	uint8_t mag_id;

	__MAG_READ(WIA, 1, &mag_id);
	 return mag_id;
}


/*
 *@brief: Configuration mode of magnetometer
 *@param:
 *		mode - An @MPU_MAG_OPMODE value that describe one operation mode, when each mode is set, AK8963 transits to set mode
 *		output_mode - An @MPU_MAG_OUTPUT_SETTING
 *@retval:None
 *
 */
static void MPU_MagConfigControl(MPU_MAG_OPMODE mode, MPU_MAG_OUTPUT_SETTING output_mde){

	uint8_t read_sensitity;

	USER_CTRL.data_cmd &= ~(1 << 5);					//Disable I2C Master, MPU will directly obtain mag data
	__MPU_WRITE(USER_CTRL, MPU_ADDR_USED);

	INT_PIN_CFG.data_cmd = 1 << 1;					//Enable the host to have control over aux pins
	__MPU_WRITE(INT_PIN_CFG, MPU_ADDR_USED);

	CNTL1.data_cmd = MAG_FUSE_ROOM;
	__MPU_WRITE(CNTL1, AK8963_ADDR);

	__MPU_READ(ASAX, 1, &read_sensitity, AK8963_ADDR);
	magx_Adj = (float)(read_sensitity - 128.0)/256.0 + 1;

	__MPU_READ(ASAY, 1, &read_sensitity, AK8963_ADDR);
	magy_Adj = (float)(read_sensitity - 128.0)/256.0 + 1;

	__MPU_READ(ASAZ, 1, &read_sensitity, AK8963_ADDR);
	magz_Adj = (float)(read_sensitity - 128.0)/256.0 + 1;

	CNTL1.data_cmd = MAG_POWER_DOWN;
	__MPU_WRITE(CNTL1, AK8963_ADDR);
	HAL_Delay(1);

	CNTL1.data_cmd = mode | (output_mde << 4);
	__MPU_WRITE(CNTL1, AK8963_ADDR);

	INT_PIN_CFG.data_cmd &= ~(1 << 1);
	__MPU_WRITE(INT_PIN_CFG, MPU_ADDR_USED);

	I2C_MST_CTRL.data_cmd = 0xD;
	__MPU_WRITE(I2C_MST_CTRL, MPU_ADDR_USED);					/* Set I2C Master Clock to 400 kHz */

	USER_CTRL.data_cmd |= 1 << 5;					//Enable I2C Master, MPU will directly obtain mag data
	__MPU_WRITE(USER_CTRL, MPU_ADDR_USED);
}

/*
 * @brief: This  register  disables  I2C  bus  interface.  I2C  bus  interface  is  enabled  in  default.
 * Once I2C bus interface is disabled, it is impossible to write other value to I2CDIS register. To enable I2C bus interface,
 * reset AK8963 or input start condition 8 times continuously
 * @param: None
 * @retval: None
 */
void MPU_MagI2CDisable(){
	I2CDIS.data_cmd = 0b00011011;						//AS THE RM defines
	__MAG_WRITE(I2CDIS);
}


/*
 *	@brief:	Used to write one specific configuration at magnetometer registers
 *  @param:	One @MPU_REGISTER specific to the magnetometer
 *	@retval: None
 *
 */
void __MAG_WRITE(MPU_REGISTER reg_to_write)
{
	I2C_SLV0_ADDR.data_cmd = 0 << 7;				/* Start a write transaction */
	I2C_SLV0_ADDR.data_cmd |= AK8963_ADDR;			/* Puts the magnetometer I2C address at first 7 bits */
	__MPU_WRITE(I2C_SLV0_ADDR, MPU_ADDR_USED);

	I2C_SLV0_REG.data_cmd = reg_to_write.register_address;
	__MPU_WRITE(I2C_SLV0_REG, MPU_ADDR_USED);

	I2C_SLV0_DO.data_cmd = reg_to_write.data_cmd;
	__MPU_WRITE(I2C_SLV0_DO, MPU_ADDR_USED);

	I2C_SLV0_CTRL.data_cmd = 0x01 << 7;
	I2C_SLV0_CTRL.data_cmd |= 0x01;
	__MPU_WRITE(I2C_SLV0_CTRL, MPU_ADDR_USED);
}

/*
 *	@brief:	Used to read one specific configuration from magnetometer registers
 *  @param:
 *  		reg_to_read: @MPU_REGISTER specific to the magnetometer
 *  		bytes: Number of bytes to be read
 *  		data_vet: Vetor where data will be placed
 *	@retval: Error code TODO
 *
 */
uint8_t __MAG_READ(MPU_REGISTER reg_to_read, uint8_t bytes, uint8_t data_vet[])
{
	I2C_SLV0_ADDR.data_cmd = 1 << 7;				/* Start a read transaction*/
	I2C_SLV0_ADDR.data_cmd |= AK8963_ADDR;			/* Puts the magnetometer I2C address at the first 7 bits */
	__MPU_WRITE(I2C_SLV0_ADDR, MPU_ADDR_USED);			/* Tells for what ext-sensor, mpu will make a read transaction */

	I2C_SLV0_REG.data_cmd = reg_to_read.register_address;			/* What ext-sensor register, mpu will read */
	__MPU_WRITE(I2C_SLV0_REG, MPU_ADDR_USED);

	I2C_SLV0_CTRL.data_cmd = 0x01 << 7;					/* Enabling reading data from this slave */
	I2C_SLV0_CTRL.data_cmd |= bytes;					/* Number of bytes to be read from I2C slave 0 */
	__MPU_WRITE(I2C_SLV0_CTRL, MPU_ADDR_USED);

	HAL_Delay(1);

	__MPU_READ(EXT_SENS_DATA_00, bytes, data_vet, MPU_ADDR_USED);			/* Here i must read EXT_SENS_DATA_0(bytes-1) */

	return 0;												/*TODO: Error code will be made in the future */
}

/*
 *	@brief:	Calibrate magnetometer data. Move MPU making 8 shaped movements
 *  @param:
 *  		numberOfSamples: number of magnetometer samples to be collected
 *  		uart: A UART_HandleTypeDef to debug
 *
 *
 */
void MPU_MagCalibrate(uint16_t numberOfSamples, UART_HandleTypeDef *uart)
{

	float *rawData = malloc(numberOfSamples * 6 * sizeof(float));
	float *w = malloc(numberOfSamples * sizeof(float));

	float magX, magY, magZ;

	char debugBuffer[200];

	sprintf(debugBuffer, "Starting the sampling process\n");
	HAL_UART_Transmit(uart, (uint8_t *)debugBuffer, strlen(debugBuffer), HAL_MAX_DELAY);

	for(uint i = 0; i<numberOfSamples; i++)
	{

		magX = MPU_MagRead(X_AXIS);
		magY = MPU_MagRead(Y_AXIS);
		magZ = MPU_MagRead(Z_AXIS);

		rawData[i * numberOfSamples] = magX;
		rawData[i * numberOfSamples + 1] = magY;
		rawData[i * numberOfSamples + 2] = magZ;
		rawData[i * numberOfSamples + 3] = -(magY * magY);
		rawData[i * numberOfSamples + 4] = -(magZ * magZ);
		rawData[i * numberOfSamples + 5] = 1;

		w[i] = magX * magX;
	}


	/* Last square method */
	float *rawDataT = malloc(6 * numberOfSamples * sizeof(float));
	float *r1 = malloc(6 * 6 * sizeof(float));
	float *inv = malloc(6 * 6 * sizeof(float));
	float *r2 = malloc(6 * numberOfSamples * sizeof(float));

	matrixTransp(rawData, rawDataT, numberOfSamples, 6);		/* H^t*/
	matrixMult(rawDataT, rawData, r1, 6, numberOfSamples, 6);	/* H^t * H */
	matrixInv(r1, inv, 6, 6);									/* (H^t * H)^(-1) */
	matrixMult(inv, rawDataT, r2, 6, 6, numberOfSamples);		/* (H^t * H)^(-1) * H^t */
	matrixMult(r2, w, magCalibrationParam, 6, numberOfSamples, 1);		/* (H^t * H)^(-1) * H^t * w */

	flagMagCalibrated = 1;

	M_OSx = X0 = magCalibrationParam[0]/2;
	M_OSy = Y0 = magCalibrationParam[1]/(2 * magCalibrationParam[3]);
	M_OSz = Z0 = magCalibrationParam[2]/(2 * magCalibrationParam[4]);

	A = magCalibrationParam[5] + pow(X0, 2) + magCalibrationParam[3] * pow(Y0, 2) + magCalibrationParam[4] * pow(Z0, 2);
	B = A / magCalibrationParam[3];
	C = A / magCalibrationParam[4];

	M_SCx = sqrt(A);
	M_SCy = sqrtf(B);
	M_SCz = sqrt(C);

	free(rawData);
	free(w);
	free(rawDataT);
	free(r1);
	free(r2);
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
	__MPU_WRITE(PWR_MGMT_2, MPU_ADDR_USED);
}

/* @brief:  Reset all gyro, accel and digital temp signal path. This bit also clears all the data sensor registers
 * @param:  None
 * @retval: None
 */
void MPU_ResetDataRegisters(){

	USER_CTRL.data_cmd |= 1 << 0;
	__MPU_WRITE(USER_CTRL, MPU_ADDR_USED);
	USER_CTRL.data_cmd &= ~(1 << 0);
}

/*
 * @brief: Reset one or more sensor data path, the sensor register will not be cleared, for this, use @MPU_SignalPathReset
 * @param: sensor_to_reset: One value of @RESET_SENSOR_SIGNAL_PATH
 * @retval: None
 */
void MPU_SignalPathReset(RESET_SENSOR_SIGNAL_PATH sensor_to_reset){

	SIGNAL_PATH_RESET.data_cmd  = sensor_to_reset;
	__MPU_WRITE(SIGNAL_PATH_RESET, MPU_ADDR_USED);
	SIGNAL_PATH_RESET.data_cmd  = 0;
}

/*
 *  @brief:  Reset internal registers and restores default settings
 * @param:  None
 * @retval: None
 */
void MPU_ResetWholeIC(){

	PWR_MGMT_1.data_cmd |= 1 << 7;
	__MPU_WRITE(PWR_MGMT_1, MPU_ADDR_USED);
	PWR_MGMT_1.data_cmd &= ~(1 << 7);
}

static void matrixMult(float *m1, float *m2, float *mr, int m1Line, int m1Column, int m2Column)
{
	//[a,b] * [b,d] = [a,d]
    int i, j, k;
    for (i = 0; i < m1Line; i++) {
        for (j = 0; j < m2Column; j++) {
        	mr[i*m2Column + j] = 0;
            for (k = 0; k < m1Column; k++)
            	mr[i*m2Column + j] += m1[i*m1Column + k] * m2[k*m2Column + j];
        }
    }
}

static void matrixTransp(float *m1, float *transp, int lines, int columns)
{
	for (int i=0;i<lines;i++)
	{
		for (int j=0;j<columns;j++)
		{
			transp[j*lines + i] = m1[i*columns + j];
		}
	}

}

static void matrixInv(float *mfrom, float *inv, int lines, int columns)
{

	float temp;
	float *tempS = malloc(lines * columns * sizeof(float));

	matrixCopy(mfrom, tempS, lines, columns);
	eye(inv, lines, columns);

	for(uint16_t k = 0; k<lines; k++)
	{
		temp = tempS[(k * columns + k)];
		for(uint16_t j = 0; j<lines; j++)
		{
			tempS[(k * columns + j)] /= temp;
			inv[(k * columns + j )] /= temp;
		}
		for(uint16_t i = 0; i<lines; i++)
		{
			temp = tempS[(i * columns + k)];
			for(uint16_t j = 0; j<lines; j++)
			{
				if(i == k)
					break;

				tempS[(i*columns + j)] -= tempS[(k*columns + j)] * temp;
				inv[(i*columns + j)] -= inv[(k*columns + j)] * temp;
			}
		}
	}

	free(tempS);
}

static void matrixCopy(float *mfrom, float *mto, int lines, int columns)
{

	for(int i = 0; i<lines; i++)
	{
		for(int j = 0; j<columns; j++)
		{
			mto[ (i * columns + j ) ] = mfrom[ (i * columns + j) ];
		}
	}
}

static void eye(float *m, int lines, int columns)
{

	for(int i = 0; i<lines; i++)
	{

		for(int j = 0; j<columns; j++)
		{
			m[(i*columns + j)] = 0;

			if(i == j)
				m[(i*columns + j)] = 1;
		}
	}

}


