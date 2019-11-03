// TI File $Revision: /main/9 $
// Checkin $Date: August 10, 2007   09:05:08 $
//###########################################################################
//
// FILE:	Example_EpwmSetup.c
//
// TITLE:	Frequency measurement using EQEP peripheral
//
// DESCRIPTION:
//
// This file contains source for the ePWM initialization for the
// freq calculation module
//
//###########################################################################
// Original Author: SD
//
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File


#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#define PWM_CLK   20e3                 // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT
#define POST_SHIFT   0  // Shift results after the entire sample table is full
#define INLINE_SHIFT 1  // Shift results as the data is taken from the results regsiter
#define NO_SHIFT     0  // Do not shift the results
void EPwmSetup()
{
  
    InitEPwm1Gpio();//init EPWM1-3  GPIO  
    InitEPwm2Gpio();
    InitEPwm3Gpio();
	EPwm1Regs.TBSTS.all=0;
	EPwm2Regs.TBSTS.all=0;
	EPwm3Regs.TBSTS.all=0;
	EPwm1Regs.TBPHS.half.TBPHS=0;
	EPwm2Regs.TBPHS.half.TBPHS=0;
	EPwm3Regs.TBPHS.half.TBPHS=0;
	EPwm1Regs.TBCTR=0;
	EPwm2Regs.TBCTR=0;
	EPwm3Regs.TBCTR=0;

	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPCTL.all=0x50; 
	EPwm3Regs.CMPCTL.all=0x50; 



	EPwm1Regs.AQCTLA.all=0;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLB.all=0;
	EPwm1Regs.AQSFRC.all=0;
	EPwm1Regs.AQCSFRC.all=0;
///***************死区模块配置*******************************/
///**************修改:孙佳伟***************************////////
/////////////////修改时间:2019年10月24日************////////
	EPwm1Regs.DBCTL.bit.IN_MODE=DBA_ALL;          //PWM1配置为A通道输入
	EPwm2Regs.DBCTL.bit.IN_MODE=DBA_ALL;          //PWM2配置为A通道输入
	EPwm3Regs.DBCTL.bit.IN_MODE=DBA_ALL;            //PWM3配置为A通道输入

	EPwm1Regs.DBCTL.bit.POLSEL=DB_ACTV_HIC;          //PWM1捕捉上升沿
    EPwm2Regs.DBCTL.bit.POLSEL=DB_ACTV_HIC;         //PWM2捕捉上升沿
    EPwm3Regs.DBCTL.bit.POLSEL=DB_ACTV_HIC;         //PWM3捕捉上升沿

    EPwm1Regs.DBCTL.bit.OUT_MODE=DBB_ENABLE;          //PWM1B通道输出使能
    EPwm2Regs.DBCTL.bit.OUT_MODE=DBB_ENABLE;    //PWM2B通道输出使能
    EPwm3Regs.DBCTL.bit.OUT_MODE=DBB_ENABLE;    //PWM2B通道输出使能

    EPwm1Regs.DBRED=0;////下降沿不延时
	EPwm2Regs.DBRED=0;////下降沿不延时
	EPwm3Regs.DBRED=0;////下降沿不延时
	EPwm1Regs.DBFED=1;    ////上升沿延时 1x时基*10
	EPwm2Regs.DBFED=1; ////上升沿延时 1x时基*10
	EPwm3Regs.DBFED=1; ////上升沿延时 1x时基*10
/////////////////**************////////////////////////////////////////////

	EPwm1Regs.TZSEL.all=0;
	EPwm2Regs.TZSEL.all=0;
	EPwm3Regs.TZSEL.all=0;
	EPwm1Regs.TZCTL.all=0;
	EPwm2Regs.TZCTL.all=0;
	EPwm3Regs.TZCTL.all=0;
	EPwm1Regs.TZEINT.all=0;
	EPwm2Regs.TZEINT.all=0;
	EPwm3Regs.TZEINT.all=0;
	EPwm1Regs.TZFLG.all=0;
	EPwm2Regs.TZFLG.all=0;
	EPwm3Regs.TZFLG.all=0;
	EPwm1Regs.TZCLR.all=0;
	EPwm2Regs.TZCLR.all=0;
	EPwm3Regs.TZCLR.all=0;
	EPwm1Regs.TZFRC.all=0;
	EPwm2Regs.TZFRC.all=0;
	EPwm3Regs.TZFRC.all=0;

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;       // Select SOC from from CPMA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;
	EPwm2Regs.ETSEL.all=0; 
	EPwm3Regs.ETSEL.all=0; 
	EPwm1Regs.ETFLG.all=0;
	EPwm2Regs.ETFLG.all=0;
	EPwm3Regs.ETFLG.all=0;
	EPwm1Regs.ETCLR.all=0;
	EPwm2Regs.ETCLR.all=0;
	EPwm3Regs.ETCLR.all=0;
	EPwm1Regs.ETFRC.all=0;
	EPwm2Regs.ETFRC.all=0;
	EPwm3Regs.ETFRC.all=0;

	EPwm1Regs.PCCTL.all=0;
	EPwm2Regs.PCCTL.all=0;
	EPwm3Regs.PCCTL.all=0;

	EPwm1Regs.TBCTL.all=0x0012;			// Enable Timer
	EPwm2Regs.TBCTL.all=0x0006;
	EPwm3Regs.TBCTL.all=0x0006;
	EPwm1Regs.TBPRD=SP;
	EPwm2Regs.TBPRD=SP;
	EPwm3Regs.TBPRD=SP;




}


