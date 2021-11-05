#include <xc.h>
#include <stdio.h>


#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#pragma intrinsic(_delay)
#define _XTAL_FREQ 20000000

const unsigned char ciclos_timer0 = 256-97; // 5 ms
const unsigned char limite_contador = 10; // para llegar al segundo
const unsigned char limite_PWM = 167;
unsigned char x;
unsigned int cont;
unsigned char ascendente1 = 1;
unsigned char ascendente2 = 0;


void init_uart(void)
{  // See chapter 12.3 PIC16F886 manual (table 12-5)

  TXSTAbits.BRGH =1;
  BAUDCTLbits.BRG16 = 1;

  SPBRGH =0x2 ;
  SPBRG = 0x8;  // 9600 baud rate with 20MHz Clock
  
  TXSTAbits.SYNC =0 ; /* Asynchronous */
  TXSTAbits.TX9 = 0; /* TX 8 data bit */
  RCSTAbits.RX9 =0 ; /* RX 8 data bit */

  PIE1bits.TXIE =0 ; /* Disable TX interrupt */
  PIE1bits.RCIE = 0; /* Disable RX interrupt */

  RCSTAbits.SPEN = 1; /* Serial port enable */

  TXSTAbits.TXEN =0 ; /* Reset transmitter */
  TXSTAbits.TXEN = 1; /* Enable transmitter */
  
 }

void putch(char c)
{ 
  while(!TXSTAbits.TRMT);
  TXREG=c;
 } 

void __interrupt() int_handler()
{
    if(INTCONbits.T0IF){
        TMR0 = ciclos_timer0;   
      if(cont == limite_contador){
          cont=0;
          if(ascendente1){
              CCPR1L++;
              if(CCPR1L==limite_PWM)
                  ascendente1=0;
          }
          else{
              CCPR1L--;
              if(CCPR1L==0)
                  ascendente1=1;
          }
          if(ascendente2){
              CCPR2L++;
              
              if(CCPR2L==limite_PWM)
                  ascendente2=0;      
          }
          else{
              CCPR2L--;
              if(CCPR2L==0)
                  ascendente2=1;
          } 
      }
      cont++;
      INTCONbits.T0IF = 0;
    }
    
}

void init_PWM()
{
    TRISCbits.TRISC1 = 1; 
    TRISCbits.TRISC2 = 1;   
    PR2=166;
    CCP1CONbits.CCP1M =0b1100;
    CCP2CONbits.CCP2M = 0b1100;
    CCPR1L=0;
    CCPR2L=limite_PWM;
    CCP1CONbits.DC1B =0;
    //probar con DC2B si no funciona
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    CCP1CONbits.P1M = 0;
    
    PIR1bits.TMR2IF = 0;
    T2CONbits.TOUTPS = 0;
    T2CONbits.T2CKPS = 0;
    T2CONbits.TMR2ON = 1;
    while(!PIR1bits.TMR2IF);
    TRISCbits.TRISC1 = 0; 
    TRISCbits.TRISC2 = 0;
  
    
}

void init_timer0()
{  
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111; //1:256
    cont = 0;
    //Poner contador a 97 ciclos
    TMR0 = ciclos_timer0;
    INTCONbits.T0IE=1;
}


void main(void) {
    OSCCON = 0b00001000;
    TRISB=0;
    
    init_uart();
    init_PWM();
    init_timer0();
  
    INTCONbits.GIE=1;
    while(1);
    return;
}
