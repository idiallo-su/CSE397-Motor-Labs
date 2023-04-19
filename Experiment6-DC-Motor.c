//This is the code minus the configuration bits and the library statement 

volatile unsigned int current_capture; 
volatile unsigned int period; 
  
void __interrupt() ISR(void) { 
	if(PIR1bits.CCP1IF) { 
    	period = CCPR1 - current_capture; 
        current_capture = CCPR1; 
    	// Test to see if the interrupt is called 
    	PORTBbits.RB5 = 0; 
    	PORTBbits.RB5 = 1; 
    	PIR1bits.CCP1IF = 0; 
	} 
     
     
     
} 
  
  
void main (void){ 
     
	//Changing the FOSC clock frequency 
	OSCCON = 0b00111000; //500khz 
	T1CON = 0b00100001;  
	CCP1CON = 0b10000101;  
	ADCON0 = 0b00011101; //Makes pin RC3 an ADC unit, Left Justified  
	ADCON1 = 0b01110000; 
   
	//Configuring the DAC 
	DAC1CON0 = 0b10000000; //This enables DAC, right justified. 
	DACLD = 0b00000001;//enables the DAC to convert a digital number to analog number 
	//Configuring Op Amp 
	OPA1CON = 0b10010000;//configures unity gain and disables the override function 
	OPA1PCHS = 0b00000010;//connect DAC1 output to the AMP non inverting input. 
             
	//Configuring PWM 
	PWM3CON = 0b10000000;  
	T2CLKCON = 0b00000001;  
	T2CON = 0b10000000;   
	RC4PPS = 0b00001110; //Use RC4 for PWM3  
	//Our wave from waveforms going into RC3 
     
	//TRISCbits.TRISC2 = 0;  
	TRISCbits.TRISC4 = 0;//PWM pin, making it an output 
	//ANSELCbits.ANSC0 = 1; 
     
	//Configuring a GPIO for Input Capture 
	TRISCbits.TRISC6 = 1; 
	ANSELCbits.ANSC6 = 0; 
	CCP1PPS = 0b00010110; //Our square wave from the motor's encoder going into RC6 
     
	//Configuring the interrupt 
	PIE1bits.CCP1IE = 1; 
	PIR1bits.CCP1IF = 0;//Line of code to clear the flag 
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1; 
     
	//Setting ports to outputs for the shift register  
	TRISBbits.TRISB4 = 0;     
	TRISBbits.TRISB5 = 0;  
	TRISBbits.TRISB6 = 0;  
	TRISBbits.TRISB7 = 0; 
     
     
     
	while (1){ 
     
	float frequency = 24000. / period; 
         
	//To make conversion, set GO/DONE bit and wait for it to become cleared 
	//The ADC hardware automatically clears the bit when the ADC conversion finishes 
    	ADCON0bits.GO = 1; 
    	while(ADCON0bits.GO == 1){ } //Nothing goes inside here 
     
	PORTB = ADRESH;     
         
	//PART 3 
	//Put ADRESH into the PWM 
	PWM3DCH = ADRESH; 
	//PART 4 
	//Put ADRESH into the DAC 
	//DAC1REF = ADRES; 
	//DACLD = 0b00000001;//enables the DAC to convert a digital number to analog number 
     
	//PART 5 
	//For 4MHz, Fosc/4, 1:1 prescaler, 5kHz square wave 
	//Scale  the output voltage to the frequency. 5000Hz = 5V, 4000Hz = 4V, etc. 
	DAC1REF = frequency; 
  
	DACLDbits.DAC1LD = 1; //enables the DAC to convert a digital number to analog number 
     
	} 
     
	return; 
} 
