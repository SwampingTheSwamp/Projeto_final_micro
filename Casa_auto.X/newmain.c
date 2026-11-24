
#include <xc.h>
#include "nxlcd.h"

#pragma config CPUDIV = OSC1_PLL2
#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF
#pragma config MCLRE = ON

#define _XTAL_FREQ 20000000

void __interrupt() ISR(void)
{
    if(INTCONbits.TMR0IF == 1)
    {
        TMR0H = 0x67;
        TMR0L = 0x69;

        INTCONbits.TMR0IF = 0;
    }
}

void main(void)
{
    TRISE = 0x00;
    TRISD = 0x00;

    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    
    int menu = 0;
    int menu_anterior = -1;

    T0CONbits.T08BIT = 0;// Timer0: 16 bits
    T0CONbits.T0CS = 0;// Clock interno (Fosc/4)
    T0CONbits.PSA = 0;// Prescaler habilitado
    T0CONbits.T0PS = 0b110;// Prescaler 1:128
    // Preload inicial
    TMR0H = 0x67;
    TMR0L = 0x69;

    INTCONbits.TMR0IF = 0;// Limpa flag
    INTCONbits.TMR0IE = 1;// Habilita interrupção Timer0

    // Habilita interrupções globais
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    T0CONbits.TMR0ON = 1;// Liga Timer0
    
    OpenXLCD(FOUR_BIT & LINES_5X7);

    WriteCmdXLCD(0x01);
    __delay_ms(2);

    WriteCmdXLCD(0x0C);

   while(1)
{
    if(PORTBbits.RB0 == 0)
    {
        __delay_ms(30);

        if(PORTBbits.RB0 == 0)
        {
            menu++;

            if(menu > 3)
                menu = 0;

            while(PORTBbits.RB0 == 0);
        }
    }

    if(PORTBbits.RB1 == 0)
    {
        __delay_ms(30);

        if(PORTBbits.RB1 == 0)
        {
            menu--;

            if(menu < 0)
                menu = 3;

            while(PORTBbits.RB1 == 0);
        }
    }

    if(menu != menu_anterior)
    {
        WriteCmdXLCD(0x01);
        __delay_ms(2);

        switch(menu)
        {
            case 0:
                WriteCmdXLCD(0x80);
                putsXLCD("> Iluminacao");
                WriteCmdXLCD(0xC0);
                putsXLCD("  Temperatura");
                break;

            case 1:
                WriteCmdXLCD(0x80);
                putsXLCD("  Iluminacao");
                WriteCmdXLCD(0xC0);
                putsXLCD("> Temperatura");
                break;

            case 2:
                WriteCmdXLCD(0x80);
                putsXLCD("> Ventilacao");
                WriteCmdXLCD(0xC0);
                putsXLCD("  Configuracao");
                break;

            case 3:
                WriteCmdXLCD(0x80);
                putsXLCD("  Ventilacao");
                WriteCmdXLCD(0xC0);
                putsXLCD("> Configuracao");
                break;
        }

        menu_anterior = menu;
    }

   }
}