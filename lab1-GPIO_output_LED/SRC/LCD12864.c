#include"DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#define   UNDEF   0xFF




void LCD_DATA(uchar d){
   Uint32 i=0;
   i = GpioDataRegs.GPADAT.all;
   i |= (d<<6)&0x3FC0;
   i = (~((d<<6)&0x3FC0))|(~i);
   GpioDataRegs.GPADAT.all = ~i;
} 
void LCDInit(void)
{
  delay(500);
  Write_order(0x30);//
  delay(5);
  Write_order(0x0c);//
  delay(5);
  Write_order(0x01);//
  delay(6);
  Write_order(0x02);//
  delay(4);
}

void Write_order(Uint16 order)
{
   RS=0;delay(1);
   RW=0;delay(1);
   EN=1;delay(1);
   LCD_DATA(order);
   delay(10);
   EN=0;delay(1);
}

void Write_data(Uint16 data)
{
   RS=1;delay(1);
   RW=0;delay(1);
   EN=1;delay(1);
   LCD_DATA(data);
   delay(10);
   EN=0;delay(1);
}

void delay(Uint16 t)
{
   Uint16 i;
   while(t--)
    {
      for(i=0;i<125;i++);
    }
}

void configio(void)
{
    EALLOW;
   // GpioCtrlRegs.GPAMUX1.all &= 0xc03f;
   // GpioCtrlRegs.GPADIR.all |= 0x00003FC0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;


    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO20= 0;
    GpioCtrlRegs.GPADIR.bit.GPIO20 = 1;
    EDIS;
}

void display(char *hz)
{
   while(*hz!='\0')
    {
       Write_data(*hz);
       hz++;
       delay(2);
    }
}


//函数功能：显示十进制数据（最大9999 9999 9）
//输入参数：   v：要显示的十进制数据
//           add: 显示起始地址
//           Len：十进制数据长度（最长9位）
//输出参数：无
void DisDec(Uint32 v, Uint16 add, unsigned char Len){
  Uint32 GUI_Pow10[] = {
  1 , 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000
  };
  char c[10], g;
  unsigned char s=0;
  if (Len > 9) {
    Len = 9;
  }
  if(v == 0) Len = 1;
  while(Len--){
    //d = GUI_Pow10[Len];
    g = (char) (v / GUI_Pow10[Len]);
    v -= g * GUI_Pow10[Len];
    c[s++] = g + '0';
  }
  c[s] = '\0';
  g=0;
  while(g < s-1){
    if(c[g] == '0') c[g]=' ';
    else break;
    g++;
  }
    if(add != UNDEF) Write_order(add);
    delay(5);
    display(c);
}

//
//===========================================================================
// No more.
//===========================================================================
