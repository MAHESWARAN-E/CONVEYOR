/*
 * File:   conveyor-encoder.c
 * Author: USER
 *
 * Created on June 2, 2025, 11:38 AM
 */
/*
 * RA0=MOTOR ON & OFF
 * RA1=SET_CM 
 * RA2=MOTOR
 * PORTB=KEYPAD
 * RC0=ENCODER OUTPUT
 * PORTD=PORTE=LCD
 */
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include<xc.h>
#define RS RE0
#define EN RE1
#define MOTOR RA2    //OUTPUT
#define MOTOR_ON RA0 //INPUT
#define SET_CM  RA1  //INPUT
unsigned long int revolution = 0,centimeter,set_centimeter=0;
int count=0,limit1=1,cm_count=0;
void delay(unsigned int a)
{
    while(a--);
}

void enable()
{
    EN=1;
    delay(100);
    EN=0;
    delay(100);
}

void lcd(char rs, unsigned char data)
{
    RS=rs;
    PORTD=data;
    enable();
}

void string(char *str)
{
    while(*str)
        lcd(1,*str++);
}

void eepromwrite(char address,char data)
{
    EEADR=address;
    EEDAT=data;
    WREN=1;
    EECON2=0x55;
    EECON2=0xAA;
    WR=1;
    WREN=1;
    WREN=0;
    while(EEIF==0);
    EEIF=0;
}

int  eepromread(char address)
{
    EEADR=address;
    EEPGD=0;
    RD=1;
    return EEDAT;
}


void set()
{
    while(limit1)
    {   
    PORTB=0X01;
    if(RB4)
    {
        set_centimeter=set_centimeter*10+1;while(RB4);
    }
    if(RB5)
    {
        set_centimeter=set_centimeter*10+2;while(RB5);
    }
    if(RB6)
    {
        set_centimeter=set_centimeter*10+3;while(RB6);
    }
    PORTB=0X02;
     if(RB4)
    {
        set_centimeter=set_centimeter*10+4;while(RB4);
    }
    if(RB5)
    {
        set_centimeter=set_centimeter*10+5;while(RB5);
    }
    if(RB6)
    {
        set_centimeter=set_centimeter*10+6;while(RB6);
    }
    
    PORTB=0X04;
    if(RB4)
    {
        set_centimeter=set_centimeter*10+7;while(RB4);
    }
    if(RB5)
    {
        set_centimeter=set_centimeter*10+8;while(RB5);
    }
    if(RB6)
    {
        set_centimeter=set_centimeter*10+9;while(RB6);
    }
    
    PORTB=0X08;
    if(RB4)
    {
        set_centimeter=0;while(RB4);
    }
    if(RB5)
    {
        set_centimeter=set_centimeter*10+0;while(RB5);
    }
    if(RB6)
    {
        limit1=0;while(RB6);
    }
    
    lcd(0,0x87);
    lcd(1,((set_centimeter/100)+48));
    lcd(1,(((set_centimeter/10)%10)+48));
    lcd(1,((set_centimeter%10)+48));
    
    }
    limit1=1;
    eepromwrite(0,set_centimeter);
}

void main()
{
    PORTD=PORTC=PORTA=PORTE=PORTB=0X00;
    TRISD=TRISE=0X00;
    TRISC=0X03;
    TRISB=0X70;
    TRISA=0X03;    
    ANSEL=ANSELH=0X00;
    T1CON=0X07;
    TMR1=0;
    
//    OPTION_REG=0X85;
//    TMR0=131;
    
    lcd(0,0x38);
    lcd(0,0x0E);
    lcd(0,0x01);
    int a=1;  //for displaying string one time
    
    set_centimeter=eepromread(0);
    lcd(0,0x80);
    string("SET_CM:");
    lcd(1,((set_centimeter/100)+48));
    lcd(1,(((set_centimeter/10)%10)+48));
    lcd(1,((set_centimeter%10)+48));
    while(1)
    {
        if(MOTOR_ON)
        {
            MOTOR^=1;//motor on and off
            while(MOTOR_ON);
        }
        
        if(SET_CM)
        {
          set_centimeter=0;
          set();//set_centimeter
        }
     
        if(TMR1>=39)
        {
            cm_count++;
            TMR1=0;
        }
        
//        lcd(0,0x80);// displaying pulse
//        if(a)
//            string("Pulse");
//        lcd(1,(revolution/10000)+48);
//        lcd(1,((revolution/1000)%10)+48);
//        lcd(1,((revolution/100)%10)+48);
//        lcd(1,((revolution%100)/10)+48);
//        lcd(1,(revolution%10)+48);
        
        
       // centimeter=cm_count;//displaying centimeter    
     
        if (a==1)
        {
        lcd(0,0xc0);
        string("Cm=");
        a=0;
        }
        lcd(0,0xc3);
        lcd(1,(cm_count/100)+48);
        lcd(1,((cm_count/10)%10)+48);
        lcd(1,(cm_count%10)+48);



        if( cm_count==set_centimeter )
        {
            MOTOR=0;
            TMR1=0;
            cm_count=0;
            lcd(0,0xc8);
            string("Reached");
        }

        }
}
