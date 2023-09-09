## driver version 

02/01/2021  LS  V1.0.0.1 : BMI263 sensor support added, I2C protocol support added and configurable based on BMI26X_I2C_BUS_ID

## suport function 

1 acc data streaming
2 gyro data streaming
3 anymotion work normal
4 acc selftest
5 gyro selftest

## Test result
1 CTS      : pass
2 selftest : pass 
	commands used to run sef test in adb shell :
		nanotool -s accel -x test
		nanotool -s gyro -x test

##  Nano-App configurations in bosch_bmi26x.c file 

### I2C configuration

	Enable this macro to configure the I2C communication with the specified I2C bus-id 
		 BMI26X_I2C_BUS_ID         0
	
	Additional I2C configurations 
		 BMI26X_I2C_SPEED          400000
		 BMI26X_I2C_ADDR           0x68

### SPI configuration

	Disable/Comment out this macro to make the nano-app use the SPI communication
		//  BMI26X_I2C_BUS_ID      0

	Additional SPI configurations 
		 BMI26X_SPI_BUS_ID         1
		 BMI26X_SPI_SPEED_HZ       8000000
		 BMI26X_SPI_MODE           3
	
### Interrupt configurations 
	 BMI26X_INT1_PIN           GPIO_PB(6)
	 BMI26X_INT2_PIN           GPIO_PB(7)


### Axes remapping for BMI26X variants
	Modify the axes remapping macros as per need 
	
	#ifndef BMI26X_TO_ANDROID_COORDINATE
	BMI26X_TO_ANDROID_COORDINATE(x, y, z)   \
		do {                                        \
			float xi = x, yi = y, zi = z;           \
			x = xi; y = yi; z = zi;                 \
		} while (0)
	#endif