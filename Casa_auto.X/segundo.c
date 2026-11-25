#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "nxlcd.h"

#pragma config CPUDIV = OSC1_PLL2
#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF
#pragma config MCLRE = ON

#define _XTAL_FREQ 20000000

// Botoes (ativos em nivel baixo)
#define BTN_RB0  0x01
#define BTN_RB1  0x02
#define BTN_RB2  0x04

// Saida da luz
#define LUZ  LATDbits.LATD0

// Quantos segundos vale cada unidade configurada no timer:
// 3600 = horas | 60 = minutos | 1 = segundos (bom para testar)
#define SEG_POR_UNIDADE  3600UL
#define CONT_MAX         99

// ---------- Variaveis globais ----------
volatile uint32_t restante = 0;    // segundos restantes do timer
volatile uint8_t  timer_ativo = 0;
volatile uint8_t  timer_fim = 0;   // ISR avisa o main que acabou

char lcd_buf[17];
char num_buf[17];

// ---------- Interrupcao: 1 segundo ----------
void __interrupt() ISR(void)
{
    if(INTCONbits.TMR0IF == 1)
    {
        TMR0H = 0x67;
        TMR0L = 0x69;
        INTCONbits.TMR0IF = 0;

        if(timer_ativo)
        {
            if(restante > 0)
                restante--;

            if(restante == 0)
            {
                timer_ativo = 0;
                timer_fim = 1;
            }
        }
    }
}

// ---------- Funcoes auxiliares ----------

// Escreve texto na linha (0 ou 1), completando com espacos (sem precisar limpar o LCD)
void lcd_linha(uint8_t linha, const char *txt)
{
    uint8_t i = 0;

    while(i < 16 && txt[i] != '\0')
    {
        lcd_buf[i] = txt[i];
        i++;
    }
    while(i < 16)
    {
        lcd_buf[i] = ' ';
        i++;
    }
    lcd_buf[16] = '\0';

    WriteCmdXLCD(linha == 0 ? 0x80 : 0xC0);
    putsXLCD(lcd_buf);
}

// Retorna 1 uma unica vez por clique (debounce + espera soltar)
uint8_t pressionado(uint8_t mascara)
{
    if((PORTB & mascara) == 0)
    {
        __delay_ms(30);
        if((PORTB & mascara) == 0)
        {
            while((PORTB & mascara) == 0);
            __delay_ms(30);
            return 1;
        }
    }
    return 0;
}

// Trata o fim do timer (desliga a luz)
// Retorna 1 se o timer acabou agora
uint8_t verifica_timer(void)
{
    if(timer_fim)
    {
        timer_fim = 0;
        LUZ = 0;
        return 1;
    }
    return 0;
}

void desenha_menu(int menu)
{
    switch(menu)
    {
        case 0:
            lcd_linha(0, "> Iluminacao");
            lcd_linha(1, "  Temperatura");
            break;
        case 1:
            lcd_linha(0, "  Iluminacao");
            lcd_linha(1, "> Temperatura");
            break;
        case 2:
            lcd_linha(0, "> Ventilacao");
            lcd_linha(1, "  Configuracao");
            break;
        case 3:
            lcd_linha(0, "  Ventilacao");
            lcd_linha(1, "> Configuracao");
            break;
    }
}

// ---------- Telas ----------

void tela_luz(void)
{
    lcd_linha(1, "0:On 1:Off 2:Sai");

    while(1)
    {
        verifica_timer();
        lcd_linha(0, LUZ ? "Luz: ligada" : "Luz: desligada");

        if(pressionado(BTN_RB0))
            LUZ = 1;

        if(pressionado(BTN_RB1))
            LUZ = 0;

        if(pressionado(BTN_RB2))
            return;
    }
}

void tela_temperatura(void)
{
    lcd_linha(0, "Temp. da casa");
    lcd_linha(1, "30 C");

    while(!pressionado(BTN_RB2))
    {
        // TODO: ler o sensor (ADC) e atualizar o valor aqui
    }
}

void tela_ventilacao(void)
{
    lcd_linha(0, "Vel. ventoinha");
    lcd_linha(1, "30 rpm");

    while(!pressionado(BTN_RB2))
    {
        // TODO: ler a rotacao real e atualizar o valor aqui
    }
}

void tela_timer(void)
{
    uint8_t cont = 0;

    lcd_linha(0, "Configure seu");
    lcd_linha(1, "Timer");
    __delay_ms(500);

    lcd_linha(0, "Setar horas");
    lcd_linha(1, "0");

    while(1)
    {
        sprintf(num_buf, "%u", cont);
        lcd_linha(1, num_buf);

        if(pressionado(BTN_RB0))        // diminui
        {
            if(cont > 0)
                cont--;
        }

        if(pressionado(BTN_RB1))        // aumenta
        {
            if(cont < CONT_MAX)
                cont++;
        }

        if(pressionado(BTN_RB2))        // confirma e sai
        {
            di();
            restante = (uint32_t)cont * SEG_POR_UNIDADE;
            timer_fim = 0;
            timer_ativo = (cont > 0) ? 1 : 0;
            TMR0H = 0x67;
            TMR0L = 0x69;
            ei();

            if(cont > 0)
                lcd_linha(0, "Timer ligado");
            else
                lcd_linha(0, "Timer cancelado");
            lcd_linha(1, "");
            __delay_ms(1000);
            return;
        }
    }
}

// ---------- Programa principal ----------
void main(void)
{
    int menu = 0;

    TRISE = 0x00;
    TRISD = 0x00;
    TRISC = 0b00000100;
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;

    INTCON2bits.RBPU = 0;   // pull-ups internos do PORTB (inofensivo se houver externos)

    LUZ = 0;

    // Timer0: 16 bits, Fosc/4, prescaler 1:128 -> estouro a cada ~1 s
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0b110;
    TMR0H = 0x67;
    TMR0L = 0x69;

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    T0CONbits.TMR0ON = 1;

    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01);
    __delay_ms(2);
    WriteCmdXLCD(0x0C);

    lcd_linha(0, " Menu da casa");
    lcd_linha(1, "   Bem vindo");
    __delay_ms(1000);

    desenha_menu(menu);

    while(1)
    {
        if(verifica_timer())
        {
            lcd_linha(0, "Timer encerrado");
            lcd_linha(1, "Luz desligada");
            __delay_ms(1500);
            desenha_menu(menu);
        }

        if(pressionado(BTN_RB0))
        {
            menu++;
            if(menu > 3)
                menu = 0;
            desenha_menu(menu);
        }

        if(pressionado(BTN_RB1))
        {
            menu--;
            if(menu < 0)
                menu = 3;
            desenha_menu(menu);
        }

        if(pressionado(BTN_RB2))
        {
            switch(menu)
            {
                case 0: tela_luz();         break;
                case 1: tela_temperatura(); break;
                case 2: tela_ventilacao();  break;
                case 3: tela_timer();       break;
            }
            desenha_menu(menu);   // ao voltar, redesenha o menu
        }
    }
}
