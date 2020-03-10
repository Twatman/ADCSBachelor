/* Global Variables */

//Raw values from Gyroscope
float gx;
float gy;
float gz;

//Raw values from Magnetometer
float mx;
float my;
float mz;

float angle_pitch;
float angle_roll;

uint8_t autocalc;

int16_t gBiasRawX;
int16_t gBiasRawY;
int16_t gBiasRawZ;

// mag scale can be 4, 8, 12, or 16
uint8_t magScale;


// Gyroscope scaling, choose: 0 = 245dps, 1 = 500dps, 3 = 2000dps
uint8_t gyroScale;


// Sensor Sensitivity Constants
// Values set according to the typical specifications provided in
// table 3 of the LSM9DS1 datasheet. (pg 12)
#define SENSITIVITY_ACCELEROMETER_2  0.000061
#define SENSITIVITY_ACCELEROMETER_4  0.000122
#define SENSITIVITY_ACCELEROMETER_8  0.000244
#define SENSITIVITY_ACCELEROMETER_16 0.000732
#define SENSITIVITY_GYROSCOPE_245    0.00875
#define SENSITIVITY_GYROSCOPE_500    0.0175
#define SENSITIVITY_GYROSCOPE_2000   0.07
#define SENSITIVITY_MAGNETOMETER_4   0.00014
#define SENSITIVITY_MAGNETOMETER_8   0.00029
#define SENSITIVITY_MAGNETOMETER_12  0.00043
#define SENSITIVITY_MAGNETOMETER_16  0.00058

#define USART_BAUDRATE 9600
#define MYUBRR F_CPU/16/BAUD-1
#define PI 3.141592

#ifndef BAUD                          /* if not defined in Makefile... */
#define BAUD  9600                     /* set a safe default baud rate */
#endif

#define PIN_XG PB1
#define PIN_M PB2

void usart_init(uint16_t ubrr);
char usart_getchar( void );
void usart_putchar( char data );
void usart_pstr (char *s);
unsigned char usart_kbhit(void);
int usart_putchar_printf(char var, FILE *stream);

/*  constrainScales - Constrains the scales of all sensors to make sure they're not out of spec, defaults the value to lowest valid scale.

*/
void constrainScales(void);


/* spiInit - Initializes the SPI.

*/
void spiInit(void);


/* initGyro - Initializes the Gyroscope control registers.

*/
void initGyro(void);


/* interuptGyro - Initializes the gyroscope interrupt registers.

*/
void interuptGyro(void);


/* initMag - Initializes the magnetometer control registers, values can be changed in sensorInits.c

*/
void initMag(void);

/* interruptMap - Sets up the interrupt flags for magnetometer, set values in sensorInits.c

*/
void interuptMag(void);


/* SPIreadByte - reads one byte of data from the desired registry via the hardware SPI.
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to read from
*/
uint8_t SPIreadByte(uint8_t csPin, uint8_t subAddress);


/* SPIreadBytes - reads a desired amount of bytes, incrementing the target registry by one for every read.
	INPUTS:
		-csPin = Chip Select, chose between XL/G or M
		-subADress = The desired start address
		-dest = Destination array for storing data
		-count = desired amount of bytes to read
*/
uint8_t SPIreadBytes(uint8_t csPin, uint8_t subAddress, uint8_t * dest, uint8_t count);


/* SPIwriteByte - writes one byte of data to desired registry via SPI
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to write to
		-data = byte to send
*/
void SPIwriteByte(uint8_t csPin, uint8_t subAddress, uint8_t data);


/* WhoAmICheck - Checks the WHO_AM_I registers of both magnetometer and Accelerometer/Gyro
				 to see if they match the expected response, holds the program until the check passes.
*/
void WhoAmICheck(void);

/* spiTransfer - Transmits one byte of data over SPI, reads one byte back.
	INPUTS:
		-data = Byte to send.
*/
uint8_t spiTransfer(uint8_t data);


/* getFIFOSamples - Reads the FIFO_SRC register on the desired chip select.
	INPUTS:
		-csPin = chip select.
*/
uint8_t getFIFOSamples();


/*	setFIFO - Set desired FIFO mode and threshold.
	INPUTS:
		-fifoMode = 3 bit value to set desired mode:
			0 = Bypass mode. FIFO is turned off.	1 = FIFO mode. Stops collecting data when FIFO is full
			2 = reserved.							3 = Continuous mode until trigger is deasserted, then FIFO mode.
			4 = Bypass mode until trigger is deasserted, then countinous mode.
			5 = Continous mode. If the FIFO is full, the new sample over-writes the older sample.
		-Threshold = ??? FIFO threshold, max value = 31.
*/
void setFIFO(uint8_t fifoMode, uint8_t fifoThs);


/*	enableFIFO - enables FIFO mode
	INPUTS:
		enable - Any value other than 0 enables FIFO.
*/
void enableFIFO(uint8_t enable);


/*	sleepGyro - tucks the gyroscope into bed.
	INPUTS:
		enable - ANy value other than 0 enables sleep.
*/
void sleepGyro(uint8_t enable);

/* getGyroIntSrc() -- Get contents of magnetometer interrupt source register

*/
uint8_t getMagIntSrc();


/* configMagThs - Sets the magnetometer interrupt threshold
	INPUTS:
		-threshold = 16-bit value for the desired threshold.
*/
void configMagThs(uint16_t threshold);


/* configMagInt - configures the magnetometer interrupt register
	INPUTS:
		- generator = Interrupt axis/high-low events
			Any OR'd combination of ZIEN, YIEN, XIEN    --- What the fuck does this mean?
		- activeLow = Interrupt active configuration
			Can be either 0 for Active LOW or 1 for active HIGH
		- latch: latch gyroscope interrupt request.
*/
void configMagInt(uint8_t generator, uint8_t activeLow, uint8_t latch);


/* getGyroIntSrc - Reads the gyro interrupt source register

*/
uint8_t getGyroIntSrc();


/*	configGyroThs() -- Configure the threshold of a gyroscope axis
	Input:
		threshold = Interrupt threshold. Possible values: 0-0x7FF.
		Value is equivalent to raw gyroscope value.
		axis = Axis to be configured. Either 1 for X, 2 for Y, or 3 for Z
		duration = Duration value must be above or below threshold to trigger interrupt
		wait = Wait function on duration counter
			1: Wait for duration samples before exiting interrupt
			0: Wait function off
*/
void configGyroThs(int16_t threshold, uint8_t axis, uint8_t duration, uint8_t wait);

/*	configGyroInt() -- Configure Gyroscope Interrupt Generator
	Input:
		- generator = Interrupt axis/high-low events
			Any OR'd combination of ZHIE_G, ZLIE_G, YHIE_G, YLIE_G, XHIE_G, XLIE_G  - ??? wats dis.
		- aoi = AND/OR combination of interrupt events
			1: AND combination
			0: OR combination
		- latch: latch gyroscope interrupt request.
*/
void configGyroInt(uint8_t generator, uint8_t aoi, uint8_t latch);


/* getGyroIntSrc() - Get status of inactivity interrupt

*/
uint8_t getInactivity(void);


/* configInactivity - 
	Input:
		- duration = Inactivity duration - actual value depends on gyro ODR
		- threshold = Activity Threshold
		- sleepOn = Gyroscope operating mode during inactivity.
		  1: gyroscope in sleep mode
		  0: gyroscope in power-down
*/
void configInactivity(uint8_t duration, uint8_t threshold, uint8_t sleepOn);


/* configInt - Configure INT1 or INT2 (Gyro and Accel Interrupts only)
	Input:
		- interrupt = Select INT1_CTRL or INT2_CTRL
		  Possible values: INT1_CTRL or INT2_CTRL
		- generator = OR'd combination of interrupt generators.
		  Possible values: INT_DRDY_XL, INT_DRDY_G, INT1_BOOT (INT1 only), INT2_DRDY_TEMP (INT2 only)
		  INT_FTH, INT_OVR, INT_FSS5, INT_IG_XL (INT1 only), INT1_IG_G (INT1 only), INT2_INACT (INT2 only)   --???? work on this.
		- activeLow = Interrupt active configuration
			1: Active HIGH
			0: Active lOW
		- pushPull =  Push-pull or open drain interrupt configuration    -- ???? work on this.
		  Can be either INT_PUSH_PULL or INT_OPEN_DRAIN
*/
void configInt(uint8_t interrupt_select, uint8_t generator, uint8_t activeLow, uint8_t pushPull);


/* readGyro - reads the desired gyroscope axis.
	INPUTS:
		axis_address = desired low byte address to start on, chose between:
			OUT_X_L_G
			OUT_Y_L_G
			OUT_Z_L_G
*/
void readGyro(void);



/* readMag - Reads the XYZ values from the magnetometer, scales the raw value according to the set sensitivity and stores the values in mx, my, and mz.
	*This function depends on the variable magScale being set correctly, it must be either 4, 8, 12 or 16.
*/
void readMag(void);

/* availableGyro = 1 when data available. 
				 = 0 when data not available
*/
uint8_t availableGyro(void);

/* availableMag	= 1 when data available.
				= 0 when data not available
   INPUTS:	0 for X-axis
			1 for Y-axis
			2 for Z-axis
			3 for X, Y, and Z- axis
*/
uint8_t availableMag(uint8_t axis);


/* calibrateGyro - Calibrates the gyroscope on the IMU module. Stores the bias in respective variables.

*/
void calibrateGyro(void);


/* calibrateMag - Calibrates the magnetometer by finding the offset and storing it in the offset registers present on the IMU.

*/
void calibrateMag(void);

/*	OffsetMag - set the offset of the magnetometer, this function is called in calibrateMag
	INPUTS:		inputs are selected in another function, should not be touched.
*/
void offsetMag(uint8_t axis, int16_t offset);