//This is the code minus the configuration bits and the library statement 

void main (void){ 

    OSCCON = 0b00111000; //500khz 
    T1CON = 0b00100001;  
    TRISB = 0b00000000; 
    ADCON0 = 0b00011101; //Makes pin RC3 an ADC unit 
    ADCON1 = 0b11110000; //Right Justified  

    T2CLKCON = 0b00000001; 
    T2CON = 0b10000000; 
    RC4PPS = 0b00001110; 
    PWM3CON = 0b10000000;  

    PWM3DC = 200; 
    PWM3DCL = 128; 

    TRISCbits.TRISC4 = 0;//PWM pin, making it an output 
    TRISCbits.TRISC6 = 1; 
    ANSELCbits.ANSC6 = 1; 
    ADCON0bits.ADON = 1; 

    while (1){ 
        ADCON0bits.GO = 1; 
        while(ADCON0bits.GO == 1){ } //Nothing goes inside here 
        int variable = 0.1*ADRES + 51; 
        PWM3DC = variable<<6; 
    } 
} 
