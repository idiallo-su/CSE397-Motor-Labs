/*  
* File:   newmain.c 
* Author: phu103 
* 
* Created on October 6, 2021, 5:18 PM 
*/ 
  
// CONFIG1 
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin) 
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled) 
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled) 
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR) 
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled) 
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled) 
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin) 
#pragma config IESO = ON        // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled) 
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled) 

// CONFIG2 
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off) 
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software) 
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR) 
#pragma config PLLEN = ON       // Phase Lock Loop enable (4x PLL is always enabled) 
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset) 
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.) 
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled) 
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled) 

// #pragma config statements should precede project file includes. 
// Use project enums instead of #define for ON and OFF. 


#include <xc.h> 
/* 
*  
*/ 

int direction = 0; //0 is forwards, 1 is backwards 
int pause = 0; //0 is on, 1 is off 
int button1 = 0; 
int button2 = 0; 
int tracker = 0; 

void __interrupt() ISR(void) 
{ 
    if (PIR1bits.CCP1IF == 1) 
    { 
        tracker++; 
        //Going 1 -> 2 -> 3 -> 4  
        if((LATC & 0b00011110) == 0b00000010) //If RC1 
        { 
            LATC = 0b00000100; //Make RC2 
        } 
        else if((LATC & 0b00011110) == 0b00000100) //If RC2 
        { 
            LATC = 0b00001000; //Make RC3 
        } 
        else if((LATC & 0b00011110) == 0b00001000) //If RC3 
        { 
            LATC = 0b00010000; //Make RC4 
        } 
        else if((LATC & 0b00011110) == 0b00010000) //If RC4 
        { 
            LATC = 0b00000010; //Make RC1 
        }        

        PIR1bits.CCP1IF = 0;        
    } 
} 
  

void main (void){ 

    //Make these 4 ports output ports 
    TRISCbits.TRISC1 = 0; 
    TRISCbits.TRISC2 = 0; 
    TRISCbits.TRISC3 = 0; 
    TRISCbits.TRISC4 = 0; 
    //Add switch as an input port 
    TRISAbits.TRISA2 = 1; 
    ANSELAbits.ANSA2 = 0; //Sets as a digital pin 
    //Add a stop switch and 2 switches for cardinal position 
    TRISBbits.TRISB4 = 1;    
    TRISBbits.TRISB5 = 1; 
    TRISBbits.TRISB6 = 1; 
    ANSELBbits.ANSB4 = 0;
    ANSELBbits.ANSB5 = 0; 
    ANSELBbits.ANSB6 = 0; 

    //KEEP THIS LINE!!!!! 
    LATC = 0b00000010; //Starting output for our Cx output ports 
    //KEEP THIS LINE!!!!! 
     
    //Changing the FOSC clock frequency 
    OSCCON = 0b00111000; //500khz 
    T1CON = 0b00100001; 
    CCP1CON = 0b10100001; 
    CCPR1 = 77.5; //77.5 Lengthen or shorten our period [Default speed 00, or 2.5ms pulse.) 
    PIE1bits.CCP1IE = 1; //Enabling the CCP1 interrupt 
    PIR1bits.CCP1IF = 0; //Clearing the CCP1 interrupt flag  
    //The two bits  required to make the CCP1 interrupt work 
    INTCONbits.PEIE = 1; //enables the peripheral interrupt 
    INTCONbits.GIE = 1; //Globally enable interrupts 
    tracker = 0; 
    int ninetyDeg = 512; 
    int howFar = 0; 
    int distance = 0; 

    while(1) 
    {   
        button1 = PORTBbits.RB6;  
        button2 = PORTBbits.RB5; 
        pause = PORTBbits.RB4; 

        if(tracker == (howFar)) 
        { 
            PIE1bits.CCP1IE = 0; 
            howFar = 0; 
            tracker = 0; 
          
        } else { 
            PIE1bits.CCP1IE = 1; 
        } 
         
        //Lets us restart the interrupt after we move 90 degrees 
        if((pause==1) && (tracker == howFar)) 
        { 
            tracker = 0;       //This tracks how far we move until turn off interrupt so we need to set it to 0 
            howFar = distance; //We need to update how far we got when we start moving again 
        } 
         

        //Setting how far we should go 
        if ((button1 == 0) && (button2 == 0)){  
            distance = ninetyDeg;   
        }  
        else if ((button1 == 0) && (button2 == 1)){  
            distance = ninetyDeg*2;  
        }  
        else if ((button1 == 1) && (button2 == 0)){  
            distance = ninetyDeg*3;  
        }  
        else if ((button1 == 1) && (button2 == 1)){  
            distance = ninetyDeg*4;  
        }  

   } 

    return; 
} 
