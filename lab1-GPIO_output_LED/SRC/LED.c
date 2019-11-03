#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
Uint16    *ExRamStart = (Uint16 *)0x100000;
Uint32 t1=0,t2=0,t3=0,t4=0,T1=0,T2=0,t5,t6,t7,t8,T3,T4;
#define   LED1      GpioDataRegs.GPADAT.bit.GPIO27 // 程序运行显示
#define   LED2      GpioDataRegs.GPBDAT.bit.GPIO53 // 过压显示
#define   LED3      GpioDataRegs.GPBDAT.bit.GPIO52 // 过流显示

extern char menu1[]={"设定速度"};

extern char menu3[]={"正二速度"};
extern char menu4[]={"反二速度"};
extern char menu5[]={"正三速度"};
extern char menu6[]={"反三速度"};

#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#define PWM_CLK   20e3                 // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E
#define POST_SHIFT   0  // Shift results after the entire sample table is full
#define INLINE_SHIFT 1  // Shift results as the data is taken from the results regsiter
#define NO_SHIFT     0  // Do not shift the results
char direction_control=0;

interrupt void ISRCap2(void);// ADC start parameters

interrupt void ISRCap1(void);
interrupt void ISRTimer0(void);
//interrupt void adc_isr(void);
void EPwmSetup();

void startmotor(void);

void pidcontrol(int u,int y);
//Uint16  LedTable[8]={0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};
//Uint16  capstastus;  //motor's status
//Uint16  dir=1;  //电机转向
unsigned int dccurent;//直流母线电流
Uint32 Sumdcc=0;
unsigned int dcccount=0,Averagedcc=0;//母线电流平均值初始化
unsigned int dcvoltage;//直流母线电压
Uint32 Sumdcv=0,count=0,cong1=0;
Uint32 SumTime=0, Time=0;
unsigned int pwm=1000;//pwm的占空比初值
float kp=0.136,ki=0.0079,kd=0;
int Speedset=1600;
int ek=0,ek1=0,ek2=0;
int du,j,k;
int strSpead[12];

float duk,SUM=0;
unsigned int Speed=0,a[12];
unsigned int l=0,nn=0,i=0;
unsigned int dcvcount=0,Averagedcv=0;//母线电压平均值初始化
float wc=10,wo=10,b1,b2,b3,kp,kd,t=0.001,z1,z2,z3,e,b0=555500,u1,u2,u0,u3;
unsigned int yy=0;
#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0x1   // S/H width in ADC module periods                  = 2 ADC cycle
#define AVG        1000  // Average sample limit
//////二二制导通变量///////
unsigned int h1=0,h2=0,h3=0,d=0;///d方向
unsigned int v1=0,v2=0,v3=0,v4=0,v5=0,v6=0;
/////三三制导通变量///////////////
unsigned int td=0;//三三制切换的方向
/////二二制-三三制控制变量///////////////
unsigned int two_three_charge=0;//two_three_charge=0二二制运行////////////////////two_three_charge=1三三制运行
char start_flag=0;/////////////电机启动停止标志位

void key_change_direction();
void main(void)
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();//时钟初始化，禁用看门狗
// Xintf总线配置和初始化
  // InitXintf();
   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK; // HSPCLK = SYSCLKOUT/ADC_MODCLK
   EDIS;

// Step 2. Initalize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
  // InitGpio();  // Skipped for this example
  // InitXintf16Gpio();   //zq
   InitEPwmGpio();
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable. ECAP1_INT = &ISRCap1;
   PieVectTable. ECAP2_INT = &ISRCap1;
   PieVectTable. ECAP3_INT = &ISRCap1;
   PieVectTable. ECAP4_INT = &ISRCap2;
   PieVectTable.TINT0 = &ISRTimer0;
  // PieVectTable.ADCINT = &adc_isr;
   //PieVectTable.XINT13 = &cpu_timer1_isr;
   //PieVectTable.TINT2 = &cpu_timer2_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize the Device Peripheral. This function can be
//         found in DSP2833x_CpuTimers.c
   InitCpuTimers();   // For this example, only initialize the Cpu Timers

// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
// 150MHz CPU Freq, 1 second Period (in uSeconds)

    ConfigCpuTimer(&CpuTimer0, 150, 1000);
   //ConfigCpuTimer(&CpuTimer1, 150, 1000000);
   //ConfigCpuTimer(&CpuTimer2, 150, 1000000);
    StartCpuTimer0();
    InitCapl();
    EPwmSetup();
    // InitAdc();
// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// cap1 is connected to CPU-int 4, and CPU int 14, which is connected
// to CPU-Timer 2:
    IER |= M_INT4;
//    IER |= M_INT3;
    IER |= M_INT1;
   //IER |= M_INT14;

// Enable TINT0 in the PIE: Group 4 interrupt1
    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER4.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER4.bit.INTx3 = 1;
    PieCtrlRegs.PIEIER4.bit.INTx4 = 1;
//  PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
// Enable global Interrupts and higher priority real-time debug events:
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM
   //Specific ADC setup for this example:

    LED1=0;
    LED2=0;
    LED3=0;

   // GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;  //按键GPIO设置，GPIO13复用为GPIO功能
   // GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;  //设置为输入
    configio();
    RS=0;
    delay(5);
    RW=0;
    delay(5);
    EN=0;
    LCDInit();
    delay(5);
    Write_order(0x01);
    delay(50);
    Write_order(0x81);
    delay(5);
    display(menu1);

   /* Write_order(0x91);
    delay(5);
    display(menu2);*/

 /*   Write_order(0x89);
    delay(5);
    display(menu3);*/

   /* Write_order(0x99);
    delay(5);
    display(menu4);*/
    key_init();

    while(1)
    {
           key_change_direction();
           show();


    }

}

/*  函 数  名：show;
 *  描       述：刷屏显示函数
 *  入口参数：无
 *  作       者：孙佳伟
 *
 */
void show()
{
    if(two_three_charge==0)
    {
        if(d==0)
        {
            Write_order(0x89);
            delay(5);
            display(menu3);
        }
        else {

            Write_order(0x89);
            delay(5);
            display(menu4);
        }
    }
    if(two_three_charge==1)
       {
           if(d==0)
           {
               Write_order(0x89);
               delay(5);
               display(menu5);
           }
           else {

               Write_order(0x89);
               delay(5);
               display(menu6);
           }
       }



    DisDec(Speedset,0x91,3);//Speedset
    DisDec(Speed,0x99,4);
}



/*  函 数  名：key_Init;
 *  描       述：按键初始化
 *  入口参数：无
 *  作       者：孙佳伟
 */
void key_init()
{
    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;


    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;
    //GpioDataRegs.GPBDAT.bit.GPIO54=0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO50= 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;

    GpioDataRegs.GPBDAT.bit.GPIO50=1;
    GpioDataRegs.GPBDAT.bit.GPIO54=0;
    EDIS;




}

/*  函 数  名：key_change_direction;
 *  描       述：按键切换旋转方向
 *  入口参数：无
 *  作       者：孙佳伟
 */
void key_change_direction()
{
    if(GpioDataRegs.GPBDAT.bit.GPIO53 == 0){ //如果检测到GPIO为低电平,即按键按下
                DELAY_US(10000);   //延时消抖，防止电平抖动导致的误触发
    if(GpioDataRegs.GPBDAT.bit.GPIO53 == 0){ //如果还是低电平说明按键确实按下了

        DELAY_US(50000);
        if(direction_control==0)direction_control=1;
        else direction_control=0;

        while(GpioDataRegs.GPBDAT.bit.GPIO53 == 0);//等待按键松开
                }
       }
                  /*方向控制，减速换向*/
      if(direction_control!=d){
          GpioDataRegs.GPBDAT.bit.GPIO50 = 0;//打开继电器

          for(i=0;i<10;i++){

             DELAY_US(100000);

         }
         d=direction_control;
         td=direction_control;
         GpioDataRegs.GPBDAT.bit.GPIO50 = 1;//打开继电器
         Speedset=1600;

      }
      /////////////二二制与三三制的切换///////////
      if(GpioDataRegs.GPBDAT.bit.GPIO48 == 0){ //如果检测到GPIO为低电平,即按键按下
             DELAY_US(10000);   //延时消抖，防止电平抖动导致的误触发

         if(GpioDataRegs.GPBDAT.bit.GPIO48 == 0){ //如果还是低电平说明按键确实按下了

             DELAY_US(50000);



             if(two_three_charge==0)two_three_charge=1;
             else two_three_charge=0;

             while(GpioDataRegs.GPBDAT.bit.GPIO48 == 0);//等待按键松开

            }

           }
      /////////////电机的启动与停止///////////
           if(GpioDataRegs.GPBDAT.bit.GPIO52 == 0){ //如果检测到GPIO为低电平,即按键按下
                  DELAY_US(10000);   //延时消抖，防止电平抖动导致的误触发

              if(GpioDataRegs.GPBDAT.bit.GPIO52 == 0) //如果还是低电平说明按键确实按下了
              {
                  DELAY_US(50000);
                  if(start_flag==0)start_flag=1;
                  else {start_flag=0;}

                  while(GpioDataRegs.GPBDAT.bit.GPIO52 == 0);//等待按键松开

                 }

                }
           if(start_flag==0)
            {
               //EALLOW;
             // // EPwm1Regs.AQCSFRC.all=0xa;//强制拉高A,B电平，关闭pwm脉冲
            //   EPwm2Regs.AQCSFRC.all=0xa;//强制拉高A,B电平，关闭pwm脉冲
            //   EPwm3Regs.AQCSFRC.all=0xa;//强制拉高A,B电平，关闭pwm脉冲
               DELAY_US(50000);//延时等待管子关断
               GpioDataRegs.GPBDAT.bit.GPIO50 = 0;//打开继电器
              // EDIS;
             }
           else
           {
               GpioDataRegs.GPBDAT.bit.GPIO50 = 1;//关闭继电器
               DELAY_US(50000);
               startmotor();//启动电机
           }
        ////////////////////////////////////////////////////////////////////////////////////////

}


interrupt void ISRTimer0(void)
{


     // count++;
      Speed=count*36;

      count=0;
    //  StopCpuTimer0();
      //    CpuTimer1Regs.TIM.all=0;

//StartCpuTimer0();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;


}
interrupt void ISRCap1(void)
{

    GpioCtrlRegs.GPAMUX2.bit.GPIO24=0;//设定cap1~3为gpio
    GpioCtrlRegs.GPAMUX2.bit.GPIO25=0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO26=0;
    GpioCtrlRegs.GPADIR.bit.GPIO24=0;//设定cap1~3为输入
    GpioCtrlRegs.GPADIR.bit.GPIO25=0;
    GpioCtrlRegs.GPADIR.bit.GPIO26=0;

    ////////二二制切换代码///////////////
    if(two_three_charge==0)
        {
            if(GpioDataRegs.GPADAT.bit.GPIO24==1){h1=1;}else{h1=0;}
            if(GpioDataRegs.GPADAT.bit.GPIO25==1){h2=1;}else{h2=0;}
            if(GpioDataRegs.GPADAT.bit.GPIO26==1){h3=1;}else{h3=0;}
            //*******************************************************************************************************/////////////
            v1 = (!v1)*( (!h1)*(!h2)*h3*d + h1*(!h2)*h3*d + (!h1)*h2*(!h3)*(!d) + h1*h2*(!h3)*(!d))
                  +v1*(!( h1*(!h2)*(!h3) + h1*h2*(!h3)*d + (!h1)*h2*(!h3)*d  +(!h1)*(!h2)*h3*(!d) + h1*(!h2)*h3*(!d) + (!h1)*h2*h3) );
            //**********************************************************************************************************//////////////
            v2 = (!v2)*( (!h1)*(!h2)*h3*d + (!h1)*(h2)*h3*d + h1*(!h2)*(!h3)*(!d) + h1*h2*(!h3)*(!d))
                  +v2*(!( (!h1)*h2*(!h3) + h1*(!h2)*h3 + (!h1)*(!h2)*h3*(!d) + (!h1)*(h2)*h3*(!d) + h1*(!h2)*(!h3)*d + h1*h2*(!h3)*d));
            ////**************************************************************************************************************////////
            v3 = (!v3)*( (!h1)*h2*h3*d + (!h1)*h2*(!h3)*d  +h1*(!h2)*(!h3)*(!d) + h1*(!h2)*h3*(!d) )
                  +v3*(!( (!h1)*(!h2)*h3 + h1*h2*(!h3) + h1*(!h2)*(!h3)*d + h1*(!h2)*h3*d + (!h1)*h2*h3*(!d) + (!h1)*h2*(!h3)*(!d) ) );
            ////////****************************************************************************************************/////////////
            v4 = (!v4)*( (!h1)*h2*(!h3)*d + h1*h2*(!h3)*d + (!h1)*(!h2)*h3*(!d) + h1*(!h2)*h3*(!d) )
                  +v4*(!( (!h1)*h2*h3 + h1*(!h2)*(!h3) + h1*(!h2)*h3*d + (!h1)*(!h2)*h3*d + (!h1)*h2*(!h3)*(!d) + h1*h2*(!h3)*(!d) ) );
            //////////***************************************************************************************************************/////////
            v5 = (!v5)*( h1*h2*(!h3)*d + h1*(!h2)*(!h3)*d + (!h1)*(!h2)*h3*(!d) + (!h1)*h2*h3*(!d))
                  +v5*(!( (!h1)*h2*(!h3) + h1*(!h2)*h3 + h1*h2*(!h3)*(!d) + h1*(!h2)*(!h3)*(!d) + (!h1)*(!h2)*h3*d + (!h1)*h2*h3*d));
            //////////***********************************************************************************************************////
            v6 = (!v6)*( h1*(!h2)*h3*d + h1*(!h2)*(!h3)*d + (!h1)*h2*(!h3)*(!d) + (!h1)*h2*h3*(!d) )
                  +v6*(!( (!h1)*(!h2)*h3 + h1*(!h3)*h2 + h1*(!h2)*h3*(!d) + h1*(!h2)*(!h3)*(!d) + (!h1)*h2*(!h3)*d + (!h1)*h2*h3*d) );

            if(v1==1&&v2==0&&v3==0&&v4==0&&v5==0&&v6==1)
            {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0xA;}

            else if(v1==0&&v2==0&&v3==0&&v4==0&&v5==1&&v6==1)
            {EPwm1Regs.AQCSFRC.all=0xA;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCTLA.all=0x90;}

            else if(v1==0&&v2==0&&v3==0&&v4==1&&v5==1&&v6==0)
            {EPwm1Regs.AQCTLB.all=0x90;EPwm1Regs.AQCSFRC.all=0x2;EPwm2Regs.AQCSFRC.all=0x0a;EPwm3Regs.AQCTLA.all=0x90;EPwm3Regs.AQCSFRC.all=0x8;}

            else if(v1==0&&v2==0&&v3==1&&v4==1&&v5==0&&v6==0)
            {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0xA; }

            else if(v1==0&&v2==1&&v3==1&&v4==0&&v5==0&&v6==0)
            {EPwm1Regs.AQCSFRC.all=0xA;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCTLB.all=0x90;}

            else if(v1==1&&v2==1&&v3==0&&v4==0&&v5==0&&v6==0)
            {EPwm1Regs.AQCTLA.all=0x90;EPwm1Regs.AQCSFRC.all=0x8;EPwm2Regs.AQCSFRC.all=0x0a;EPwm3Regs.AQCTLB.all=0x90;EPwm3Regs.AQCSFRC.all=0x2;}


            pidcontrol(Speedset,Speed);
        }
    ///////////////////三三制绕组切换//////////////////

    if(two_three_charge==1)
    {
        if(GpioDataRegs.GPADAT.bit.GPIO24==1){h1=1;}else{h1=0;}
        if(GpioDataRegs.GPADAT.bit.GPIO25==1){h2=1;}else{h2=0;}
        if(GpioDataRegs.GPADAT.bit.GPIO26==1){h3=1;}else{h3=0;}
        //*****************************************************************************************************************************////
        v1=(!v1)* ( h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) )
            +v1*( !( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) ) );
        ///*********************************************************************************************************************************////
        v2=(!v2)* ( (!h1)*h2*h3*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) )
            +v2*( !( (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + (!h1)*h2*h3*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) ) );
        //********************************************************************************************************************************//////
        v3=(!v3)* ( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + (!h1)*(!h2)*h3*td + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) )
           +v3*( !( h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + (!h1)*(!h2)*h3*(!td) ) );
        ///**************************************************************************************************************************************////
        v4=(!v4)* ( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) )
            +v4*( !( h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) ) );
        /////***************************************************************************************************************************************////
        v5=(!v5)* ( (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + (!h1)*h2*h3*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) )
            +v5*( !( (!h1)*h2*h3*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) ) );
        //*******************************************************************************************************************************************///
        v6=(!v6)* ( h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + (!h1)*(!h2)*h3*(!td) )
            +v6*( !( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + (!h1)*(!h2)*h3*td + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) ) );
        //*****************************************************************************************************************************************///
        if(v1==1&&v2==1&&v3==1&&v4==0&&v5==0&&v6==0)
        {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}

        else if(v1==0&&v2==1&&v3==1&&v4==1&&v5==0&&v6==0)
        {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}

        else if(v1==0&&v2==0&&v3==1&&v4==1&&v5==1&&v6==0)
        {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90;}

        else if(v1==0&&v2==0&&v3==0&&v4==1&&v5==1&&v6==1)
        {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90; }

        else if(v1==1&&v2==0&&v3==0&&v4==0&&v5==1&&v6==1)
        {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90; }

        else if(v1==1&&v2==1&&v3==0&&v4==0&&v5==0&&v6==1)
        {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}

        pidcontrol(Speedset,Speed);
    }

    //////////////////////////////


    /*************************/
    GpioCtrlRegs.GPAMUX2.bit.GPIO24=1;//设定cap1~3为cap
    GpioCtrlRegs.GPAMUX2.bit.GPIO25=1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO26=1;
   // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap1Regs.ECCLR.all=0xFFFF;//clare all flag
    ECap2Regs.ECCLR.all=0xFFFF;
    ECap3Regs.ECCLR.all=0xFFFF;

}


void startmotor(void)
{
    EALLOW;
    EPwm1Regs.CMPA.half.CMPA =pwm;
    EPwm2Regs.CMPA.half.CMPA =pwm;
    EPwm3Regs.CMPA.half.CMPA =pwm;
    EPwm1Regs.CMPB=pwm;
    EPwm2Regs.CMPB=pwm;
    EPwm3Regs.CMPB=pwm;

    GpioCtrlRegs.GPAMUX2.bit.GPIO24=0;//设定cap1~3为gpio
    GpioCtrlRegs.GPAMUX2.bit.GPIO25=0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO26=0;
    GpioCtrlRegs.GPADIR.bit.GPIO24=0;//设定cap1~3为输入
    GpioCtrlRegs.GPADIR.bit.GPIO25=0;
    GpioCtrlRegs.GPADIR.bit.GPIO26=0;

    ////////二二制切换代码///////////////
       if(two_three_charge==0)
           {
               if(GpioDataRegs.GPADAT.bit.GPIO24==1){h1=1;}else{h1=0;}
               if(GpioDataRegs.GPADAT.bit.GPIO25==1){h2=1;}else{h2=0;}
               if(GpioDataRegs.GPADAT.bit.GPIO26==1){h3=1;}else{h3=0;}
               //*******************************************************************************************************/////////////
               v1 = (!v1)*( (!h1)*(!h2)*h3*d + h1*(!h2)*h3*d + (!h1)*h2*(!h3)*(!d) + h1*h2*(!h3)*(!d))
                     +v1*(!( h1*(!h2)*(!h3) + h1*h2*(!h3)*d + (!h1)*h2*(!h3)*d  +(!h1)*(!h2)*h3*(!d) + h1*(!h2)*h3*(!d) + (!h1)*h2*h3) );
               //**********************************************************************************************************//////////////
               v2 = (!v2)*( (!h1)*(!h2)*h3*d + (!h1)*(h2)*h3*d + h1*(!h2)*(!h3)*(!d) + h1*h2*(!h3)*(!d))
                     +v2*(!( (!h1)*h2*(!h3) + h1*(!h2)*h3 + (!h1)*(!h2)*h3*(!d) + (!h1)*(h2)*h3*(!d) + h1*(!h2)*(!h3)*d + h1*h2*(!h3)*d));
               ////**************************************************************************************************************////////
               v3 = (!v3)*( (!h1)*h2*h3*d + (!h1)*h2*(!h3)*d  +h1*(!h2)*(!h3)*(!d) + h1*(!h2)*h3*(!d) )
                     +v3*(!( (!h1)*(!h2)*h3 + h1*h2*(!h3) + h1*(!h2)*(!h3)*d + h1*(!h2)*h3*d + (!h1)*h2*h3*(!d) + (!h1)*h2*(!h3)*(!d) ) );
               ////////****************************************************************************************************/////////////
               v4 = (!v4)*( (!h1)*h2*(!h3)*d + h1*h2*(!h3)*d + (!h1)*(!h2)*h3*(!d) + h1*(!h2)*h3*(!d) )
                     +v4*(!( (!h1)*h2*h3 + h1*(!h2)*(!h3) + h1*(!h2)*h3*d + (!h1)*(!h2)*h3*d + (!h1)*h2*(!h3)*(!d) + h1*h2*(!h3)*(!d) ) );
               //////////***************************************************************************************************************/////////
               v5 = (!v5)*( h1*h2*(!h3)*d + h1*(!h2)*(!h3)*d + (!h1)*(!h2)*h3*(!d) + (!h1)*h2*h3*(!d))
                     +v5*(!( (!h1)*h2*(!h3) + h1*(!h2)*h3 + h1*h2*(!h3)*(!d) + h1*(!h2)*(!h3)*(!d) + (!h1)*(!h2)*h3*d + (!h1)*h2*h3*d));
               //////////***********************************************************************************************************////
               v6 = (!v6)*( h1*(!h2)*h3*d + h1*(!h2)*(!h3)*d + (!h1)*h2*(!h3)*(!d) + (!h1)*h2*h3*(!d) )
                     +v6*(!( (!h1)*(!h2)*h3 + h1*(!h3)*h2 + h1*(!h2)*h3*(!d) + h1*(!h2)*(!h3)*(!d) + (!h1)*h2*(!h3)*d + (!h1)*h2*h3*d) );

               if(v1==1&&v2==0&&v3==0&&v4==0&&v5==0&&v6==1)
               {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0xA;}

               else if(v1==0&&v2==0&&v3==0&&v4==0&&v5==1&&v6==1)
               {EPwm1Regs.AQCSFRC.all=0xA;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCTLA.all=0x90;}

               else if(v1==0&&v2==0&&v3==0&&v4==1&&v5==1&&v6==0)
               {EPwm1Regs.AQCTLB.all=0x90;EPwm1Regs.AQCSFRC.all=0x2;EPwm2Regs.AQCSFRC.all=0x0a;EPwm3Regs.AQCTLA.all=0x90;EPwm3Regs.AQCSFRC.all=0x8;}

               else if(v1==0&&v2==0&&v3==1&&v4==1&&v5==0&&v6==0)
               {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0xA; }

               else if(v1==0&&v2==1&&v3==1&&v4==0&&v5==0&&v6==0)
               {EPwm1Regs.AQCSFRC.all=0xA;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCTLB.all=0x90;}

               else if(v1==1&&v2==1&&v3==0&&v4==0&&v5==0&&v6==0)
               {EPwm1Regs.AQCTLA.all=0x90;EPwm1Regs.AQCSFRC.all=0x8;EPwm2Regs.AQCSFRC.all=0x0a;EPwm3Regs.AQCTLB.all=0x90;EPwm3Regs.AQCSFRC.all=0x2;}

           }
       ///////////////////三三制绕组切换//////////////////

       if(two_three_charge==1)
       {
           if(GpioDataRegs.GPADAT.bit.GPIO24==1){h1=1;}else{h1=0;}
           if(GpioDataRegs.GPADAT.bit.GPIO25==1){h2=1;}else{h2=0;}
           if(GpioDataRegs.GPADAT.bit.GPIO26==1){h3=1;}else{h3=0;}
           //*****************************************************************************************************************************////
           v1=(!v1)* ( h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) )
               +v1*( !( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) ) );
           ///*********************************************************************************************************************************////
           v2=(!v2)* ( (!h1)*h2*h3*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) )
               +v2*( !( (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + (!h1)*h2*h3*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) ) );
           //********************************************************************************************************************************//////
           v3=(!v3)* ( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + (!h1)*(!h2)*h3*td + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) )
              +v3*( !( h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + (!h1)*(!h2)*h3*(!td) ) );
           ///**************************************************************************************************************************************////
           v4=(!v4)* ( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) )
               +v4*( !( h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) ) );
           /////***************************************************************************************************************************************////
           v5=(!v5)* ( (!h1)*h2*(!h3)*td + h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + (!h1)*h2*h3*(!td) + h1*(!h2)*h3*(!td) + (!h1)*(!h2)*h3*(!td) )
               +v5*( !( (!h1)*h2*h3*td + h1*(!h2)*h3*td + (!h1)*(!h2)*h3*td + (!h1)*h2*(!h3)*(!td) + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) ) );
           //*******************************************************************************************************************************************///
           v6=(!v6)* ( h1*h2*(!h3)*td + h1*(!h2)*(!h3)*td + h1*(!h2)*h3*td + (!h1)*h2*h3*(!td) + (!h1)*h2*(!h3)*(!td) + (!h1)*(!h2)*h3*(!td) )
               +v6*( !( (!h1)*h2*h3*td + (!h1)*h2*(!h3)*td + (!h1)*(!h2)*h3*td + h1*h2*(!h3)*(!td) + h1*(!h2)*(!h3)*(!td) + h1*(!h2)*h3*(!td) ) );
           //*****************************************************************************************************************************************///
           if(v1==1&&v2==1&&v3==1&&v4==0&&v5==0&&v6==0)
           {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}

           else if(v1==0&&v2==1&&v3==1&&v4==1&&v5==0&&v6==0)
           {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}

           else if(v1==0&&v2==0&&v3==1&&v4==1&&v5==1&&v6==0)
           {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLA.all=0x90;EPwm2Regs.AQCSFRC.all=0x8;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90;}

           else if(v1==0&&v2==0&&v3==0&&v4==1&&v5==1&&v6==1)
           {EPwm1Regs.AQCSFRC.all=0x2;EPwm1Regs.AQCTLB.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90; }

           else if(v1==1&&v2==0&&v3==0&&v4==0&&v5==1&&v6==1)
           {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x08;EPwm3Regs.AQCTLA.all=0x90; }

           else if(v1==1&&v2==1&&v3==0&&v4==0&&v5==0&&v6==1)
           {EPwm1Regs.AQCSFRC.all=0x8;EPwm1Regs.AQCTLA.all=0x90;EPwm2Regs.AQCTLB.all=0x90;EPwm2Regs.AQCSFRC.all=0x2;EPwm3Regs.AQCSFRC.all=0x02;EPwm3Regs.AQCTLB.all=0x90;}
       }

   /*************************/
    GpioCtrlRegs.GPAMUX2.bit.GPIO24=1;//设定cap1~3为cap
    GpioCtrlRegs.GPAMUX2.bit.GPIO25=1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO26=1;


    EDIS;

}
interrupt void ISRCap2(void)
{
    GpioCtrlRegs.GPAMUX2.bit.GPIO27=0;
    GpioCtrlRegs.GPADIR.bit.GPIO27=0;//设定cap1~3为输入
    count++;
    GpioCtrlRegs.GPAMUX2.bit.GPIO27=1;
    // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

    ECap4Regs.ECCLR.all=0xFFFF;

}
void pidcontrol(int u,int y)
{
        ek=u-y;
        duk=kp*(ek-ek1)+ki*ek+kd*(ek+ek2-ek1*2);
        du=(int)duk;
        if(duk>1)  duk=1;
        if(duk<-1) duk=-1;
        pwm-=du;
        if(pwm<150)
        {
          pwm=150;
        }
        if(pwm>2400)
        {
          pwm=2400;
        }
        EPwm1Regs.CMPA.half.CMPA =pwm;
        EPwm2Regs.CMPA.half.CMPA =pwm;
        EPwm3Regs.CMPA.half.CMPA =pwm;
        EPwm1Regs.CMPB=pwm;
        EPwm2Regs.CMPB=pwm;
        EPwm3Regs.CMPB=pwm;

        ek2=ek1;
        ek1=ek;
}
//===========================================================================
// No more.petri网
//===========================================================================
