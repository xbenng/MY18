#ifndef _THERMOSTAT_H_
#define _THERMOSTAT_H_

#include "i2c.h"

#define THERMO_ADDR 0x90 //slave address is (1001 A2 A1 A0 R/W). 
							//A2, A1, A0 user-configurable, allow for multiple thermostats same bus
							//R/W is 0 for Write, 1 for Read
/////Configuration Register/////

//Conversion Resolution, default 9 bits
#define RES_9_BITS (0x0)
#define RES_10_BITS (0x20)
#define RES_11_BITS (0x40)
#define RES_12_BITS (0x60)

//Fault Tolerance, default 1
#define FAULT_TOL_1 (0x0)
#define FAULT_TOL_2 (0x8)
#define FAULT_TOL_4 (0x10)
#define FAULT_TOL_6 (0x18)

//Thermostat Polarity, default low
#define OS_ACTIVE_LOW (0x0)
#define OS_ACTIVE_HIGH (0x4)

//Operating Mode
#define MODE_COMPARATOR (0x0)
#define MODE_INTERRUPT (0x2)

//SHUTDOWN
#define THERMO_ACTIVE (0x0)
#define THERMO_SHUTDOWN (0x1)

/////POINTER/////
#define POINTER_TEMP (0x0)
#define POINTER_CONFIG (0x1)
#define POINTER_HYST (0x2)
#define POINTER_OS (0x3)


//Address Read and Write
#define READ (0x1)
#define WRITE (0x0)

//Upper and Lower Limits for Tos and Thyst



void Thermostat_Init(void);
float Thermostat_Read(void);
void Thermostat_Shutdown(void);

#endif