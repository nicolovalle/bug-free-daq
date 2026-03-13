/******************************************************************************
* 
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the 
* software, documentation and results solely at his own risk.
******************************************************************************/

/*******************************************************************************
This is a simple DAQ able to configure a discriminator and manage the readout
of a QTP board (Q=QDC, t=TDC, P=Peak sensing ADC).
Main parameters are read from a text config file (default file name = config.txt)
If the base address is not set (either for QTP or Discr), that board will be 
ignored, thus it is possible to use this program for QTP only or discr only.

Supported QTP Models
 Q = QDC		: V792, V792N, V862, V965, V965A
 T = TDC		: V775, V775N
 P = Peak ADC	: V785, V785N, V1785
Supported Discriminator Models: V812, V814, V895
*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
	#include <sys/timeb.h>
	#include <direct.h>
	#include <windows.h>
	#include <conio.h>
	#define kbhit _kbhit
	#define getch _getch
#else
	#include <unistd.h>
	#include <sys/time.h>
	#define Sleep(x) usleep((x)*1000)
#endif

#include <CAENVMElib.h>
#include <CAENVMEtypes.h>

#include "Console.h"

char path[128];

/****************************************************/

#define MAX_BLT_SIZE		(256*1024)

#define DATATYPE_MASK		0x06000000
#define DATATYPE_HEADER		0x02000000
#define DATATYPE_CHDATA		0x00000000
#define DATATYPE_EOB		0x04000000
#define DATATYPE_FILLER		0x06000000

#define LSB2PHY				100   // LSB (= ADC count) to Physical Quantity (time in ps, charge in fC, amplitude in mV)

#define LOGMEAS_NPTS		1000

#define ENABLE_LOG			0

#ifdef _WIN32
#define FILES_IN_LOCAL_FOLDER	1
#else
#define FILES_IN_LOCAL_FOLDER	1
#endif


/*******************************************************************/

// --------------------------
// Global Variables
// --------------------------
// Base Addresses
uint32_t BaseAddress = 0x06000000;
//
//// handle for the V1718/V2718 
int32_t handle = -1; 
//
int VMEerror = 0;
char ErrorString[100];
FILE *logfile;

int Iped = 100;


/*******************************************************************************/
/*                               READ_REG                                      */
/*******************************************************************************/
uint16_t read_reg(uint16_t reg_addr)
{
	uint16_t data=0;
	CVErrorCodes ret;
	ret = CAENVME_ReadCycle(handle, BaseAddress + reg_addr, &data, cvA32_U_DATA, cvD16);
	if(ret != cvSuccess) {
		sprintf(ErrorString, "Cannot read at address %08X\n", (uint32_t)(BaseAddress + reg_addr));
		VMEerror = 1;
	}
	if (ENABLE_LOG)
		fprintf(logfile, " Reading register at address %08X; data=%04X; ret=%d\n", (uint32_t)(BaseAddress + reg_addr), data, (int)ret);
	return(data);
}



/*******************************************************************************/
/*                                WRITE_REG                                    */
/*******************************************************************************/
void write_reg(uint16_t reg_addr, uint16_t data)
{
	CVErrorCodes ret;
	ret = CAENVME_WriteCycle(handle, BaseAddress + reg_addr, &data, cvA32_U_DATA, cvD16);
	if(ret != cvSuccess) {
		sprintf(ErrorString, "Cannot write at address %08X\n", (uint32_t)(BaseAddress + reg_addr));
		VMEerror = 1;
	}
	if (ENABLE_LOG)
		fprintf(logfile, " Writing register at address %08X; data=%04X; ret=%d\n", (uint32_t)(BaseAddress + reg_addr), data, (int)ret);
}





/******************************************************************************/
/*                                   MAIN                                     */
/******************************************************************************/
int main(int argc, char *argv[])
{

  uint32_t pid = 0;
  int pnt;
  
  printf("\n");
  printf("****************************************************************************\n");
  printf("                    QDC-PADC-TAC-Dicr DAQ        (Scaglioni VERSION)        \n");
  printf("****************************************************************************\n");

  
  if (ENABLE_LOG) {
    char tmp[255];
    sprintf(tmp, "./qtp_log.txt");
    printf("Log file is %s\n",tmp);
    logfile = fopen(tmp,"w");
  }

  CVBoardTypes ctype = cvV2718;
  if (CAENVME_Init2(ctype, &pid, 0, &handle) != cvSuccess){
    printf("Failed to open VME controller...\n");
    Sleep(1000);
  }

  // let's start doing things
  
  write_reg(0x1016, 0); // reset board
  if (VMEerror) {
    printf(ErrorString);
    getch();
    goto QuitProgram;
  }

  int model = (read_reg(0x803E) & 0xFF) + ((read_reg(0x803A) & 0xFF) << 8);
  printf("Model: %d\n",model);

  write_reg(0x1060, Iped);  // Set pedestal
  write_reg(0x1010, 0x60);  // enable BERR to close BLT at end of block

  write_reg(0x1032, 0x0010);  // disable zero suppression
  write_reg(0x1032, 0x0008);  // disable overrange suppression
  write_reg(0x1032, 0x1000);  // enable empty events

  //printf("Ctrl Reg = %04X\n", read_reg(0x1032));  
  printf("Board programmed\n");
  printf("Press any key to start\n");
  getch();
 

 QuitProgram:
  if (handle >= 0) CAENVME_End(handle);
  if (logfile != NULL) fclose(logfile);
  
}
