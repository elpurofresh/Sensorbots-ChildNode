/*
 * common_functions.h
 *
 *  Created on: Jul 17, 2012
 *      Author: andres
 */

#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_

#include <string.h>
#include "common_variables.h"


void initClocks(void);						//initializes MCLK and ACLK
void initUARTBot(void);						//initializes UART0
void initUARTSdCard(void);					//initializes UART1

void sleep(void);							//delays the processor for 1 sec
void sleepHalf(void);						//delays the processor for 1/2 sec
void sleepQuart(void);						//delays the processor for 1/4 sec

void initLEDWait(void);

void UART_SDputChar(char charToSend);
void UART_BotputChar(char charToSend);

int checkSDCardStatus();						//checks to make sure all slaves are working properly
void turnSdCard(int val);

void createNewFile(const char *buf) ;
void UART_SDwrite (const char *buf);
void writeDataSD(char *buf, int varOrFixed);
void writeDataToSDFile(const char *buf, char *data, int varOrFixed);
void sendEscapeCmd(void);

void getMyData(void);
void genPacket(int val);

void LongDelay();

void startTimer(void);
void stopTimer(void);

void startRXBots(void);
void stopRXBots(void);

void startExternalClock(void);
void stopExternalClock(void);

void setupPorts(void);

void itoa(int value, char s[]);
int atoi(char *s);
char* itoaFixLength(int value, int numDigits);
int atoiFixLength(char s[], int startDigit, int endDigits);

int isBotInMyChildrenList(char botToTest);
void putBotMyChildrenList(char botToInsert);

void parsePacket(char packet[]);

void setRxdFromChildren(int botToSet);
int rxdFromAllChildrenTest(void);

void initClocks(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // stop watchdog

	// XT1 Setup
	//PJSEL0 |= BIT4 + BIT5;

	CSCTL0_H = 0xA5;
	CSCTL1 |= DCOFSEL0 + DCOFSEL1;             // Set max. DCO setting: 8MHz, DCOSEL = 0 if 1, then DCO = 24MHz
	//CSCTL2 = SELA_0 + SELS_3 + SELM_3;        // set ACLK = XT1; MCLK = DCO
	//CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;        // set all dividers
	CSCTL2 = SELS_3 + SELM_3;        // set ACLK = XT1; MCLK = DCO
	CSCTL3 = DIVS_0 + DIVM_0;        // set all dividers
	//CSCTL4 |= XT1DRIVE_0;
	//CSCTL4 &= ~XT1OFF;
	// CSCTL0_H = 0x01;                          // Lock Register

	/*do
	{
		CSCTL5 &= ~XT1OFFG;
		// Clear XT1 fault flag
		SFRIFG1 &= ~OFIFG;
	}while (SFRIFG1&OFIFG);                   // Test oscillator fault flag*/
}

void initUARTBot(void)
{
	// Configure UART 0
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 = UCSSEL_2;                      	// Set SMCLK = 8MHz as UCBRCLK
	UCA0BR0 = 52;                           	// (8000000Hz / 9600bps) / 16  = 52.083
	UCA0BR1 = 0;
	// OS16 enabled because we are using SMCLK @ 8MHz High Freq.
	// 52.083-52 = 0.083 *16 = 1.33
	// UCBRSx value 0.3335= 0x42 (See UG)
	UCA0MCTLW = 0x4911 ;						// UCBRFx = 1, UCBRSx = 0x49, UCOS16 = 1 (Refer User Guide)
	UCA0CTL1 &= ~UCSWRST;                     	// Release from reset

	// Configure UART pins
	P2SEL1 |= UART_RXBot + UART_TXBot;
	P2SEL0 &= ~(UART_RXBot + UART_TXBot);

	UCA0IE |= UCRXIE;                         	// Enable RX interrupt
}

void initUARTSdCard(void)
{
	// Configure UART 1
	UCA1CTL1 |= UCSWRST;
	UCA1CTL1 = UCSSEL_2;                    	// Set SMCLK = 8MHz as UCBRCLK
	UCA1BR0 = 52;                 				// (8000000Hz / 9600bps) / 16  = 52.083
	UCA1BR1 = 0;
	// OS16 enabled because we are using SMCLK @ 8MHz High Freq.
	// 52.083-52 = 0.083 *16 = 1.33
	// UCBRSx value 0.3335= 0x42 (See UG)
	UCA1MCTLW = 0x4911 ;						// UCBRFx = 1, UCBRSx = 0x49, UCOS16 = 1 (Refer User Guide)
	UCA1CTL1 &= ~UCSWRST;                     	// Release from reset

	// Configure UART pins
	P2SEL1 |= UART_RXSd + UART_TXSd;
	P2SEL0 &= ~(UART_RXSd + UART_TXSd);

	UCA1IE |= UCRXIE;                         	// Enable RX interrupt
}

/*void initUartBuffer(void)
{
	int i = 0;
	for (i = 0; i < strlen(uartBuffer); ++i) {
		uartBuffer[i] = '@';
	}
}*/

void sleep(void){
	_delay_cycles(8000000);
}

void sleepHalf(void)
{
	_delay_cycles(4000000);
}

void sleepQuart(void)
{
	_delay_cycles(2000000);
}


void UART_SDputChar(char charToSend)  	// UART1 Transmit Subroutine
{
	while (!(UCTXIFG & UCA1IFG));  // USCI_A1 TX buffer ready?
	UCA1TXBUF = charToSend;       // send data

	/*uart_Out[cnterTest1] = UCA1TXBUF;
	cnterTest1++;

	if (cnterTest1 > 60) {
		cnterTest1 = 0;
	}*/
}

void UART_BotputChar(char charToSend)  	// UART0 Transmit Subroutine
{
	while (!(UCTXIFG & UCA0IFG));  // USCI_A0 TX buffer ready?
	UCA0TXBUF = charToSend;       // send data

	uart_Out[cnterTest1] = UCA0TXBUF;
	cnterTest1++;

	if (cnterTest1 > 60) {
		cnterTest1 = 0;
	}
}

void turnSdCard(int val) {

	if (val == ON) {
		P2DIR |= SD_Enable0;// + SD_Enable1;
		P2OUT |= SD_Enable0;// + SD_Enable1;
		sleep();
	}
	else if (val == OFF) {
		P2OUT &= ~SD_Enable0;
	}
}

void createNewFile(const char *buf) {
	UART_SDwrite(buf);
}

void UART_SDwrite (const char *buf) {
	int i;
	for (i = 0; i < strlen(buf); i++) {
		UART_SDputChar (buf[i]);
	}

	UART_SDputChar(CR);
}

void writeDataSD(char *buf, int varOrFixed) {
	int i, len;

	if (varOrFixed == 0) {				// Length of data fixed, i.e. from own sensors
		len = strlen(buf);
	}
	else if (varOrFixed == 1) {			// Length of data variable, i.e. RXed from UART
		len = dataLength;
	}

	UART_SDputChar(START_BYTE);		// StartofLine marker

	for (i = 0; i < len; i++) {
		UART_SDputChar(buf[i]);
	}

	UART_SDputChar(END_BYTE);			// EndOfLine marker
	UART_SDputChar(CR);
}

void writeDataToSDFile(const char *buf, char *data, int varOrFixed) {  // Assumes target file already exists

	UART_SDwrite(buf);      			// append cmd
	writeDataSD(data, varOrFixed);		// write actual data
	sleep();
	sendEscapeCmd();					// escape: go back to cmd prompt '>'
}

void sendEscapeCmd(void) {

	UART_SDputChar(CTRLZ);
	UART_SDputChar(CTRLZ);
	UART_SDputChar(CTRLZ);
	sleepQuart(); // wait for a 1/4sec

}

int checkSDCardStatus() {
	char sdCardOK = 0;
	int i = 0, timeOut = 10;

	turnSdCard(ON);
	while(i < timeOut) {

		//blinkLED1(1);

		UART_SDputChar(CR);
		createNewFile("new myData.txt\0");
		sleepQuart();
		writeDataToSDFile("append myData.txt\0", header, 0);
		sleepQuart();

		if (byteRX_SD == '>') {
			sleep();			// we need to figure out why this is necessary
			sdCardOK = 1;
			//blinkLED2(1);		// display success
			i = timeOut;
		} else {
			//blinkLED8(1);		// indicates that the condition failed to satisfy the if statement
		}

		i++;
	}
	turnSdCard(OFF);

	if (sdCardOK) {		// SD card passed/responded correctly
		return 1;
	} else {
		return 0;		// SD card did not pass/respond correctly
	}
}

void getMyData(void)
{
	int i = 0, j = 0, k = 0;
	int const sizeTemp = 2, sizeData = 4;
	char temp[3] = "00\0";
	char botId[2] = "0\0";
	int data[4];

	/*data[0] = hrs;	data[1] = min;	data[2] = sec; data[3] = OX;
	data[4] = PH;	data[5] = TP;	data[6] = AB;*/
	data[0] = OX;	data[1] = PH;	data[2] = TP;	data[3] = AB;

	itoa(myBotID, botId);
	robotsData[myBotID][k++] = botId[0];

	for (i = 0; i < sizeData; ++i) {
		itoa(data[i], temp);
		for (j = 0; j < sizeTemp; ++j) {
			robotsData[myBotID][k++] = temp[j];
		}
	}
}

void genPacket(int val)
{
	unsigned int  i = 0, j = 0;
	int buffCnter = 2;
	char temp1[2] = "0\0";
	char temp2[3] = "00\0";
	int timeSlotArrayIndex = 2;
	int myBotData[6]; //botID(0), parent(1), timeSlot(2), hh(3), mm(4), ss(5)

	myBotData[0] = myBotID;			myBotData[1] = myParent; 		myBotData[2] = myTimeSlotCounter;
	myBotData[3] = hrs;				myBotData[4] = min;				myBotData[5] = sec;

	if (val == SELF_NETWORK_DATA) {

		for (i = 0; i < MAX_SELF_DATA; ++i) {

			if (i < timeSlotArrayIndex) {
				itoa(myBotData[i], temp1);
				buffToNetwork[i] = temp1[0];
			} else {

				itoa(myBotData[i], temp2);
				for (j = 0; j < 2; ++j) {
					buffToNetwork[buffCnter++] = temp2[j];
				}
				temp2[0] = '0'; temp2[1] = '0';
			}
		}

	} else if (val == SELF_N_CHILD_DATA) {

		for (i = 0; i < MAX_SELF_DATA; ++i) {

			if (i < timeSlotArrayIndex) {
				itoa(myBotData[i], temp1);
				buffToNetwork[i] = temp1[0];
			} else {

				itoa(myBotData[i], temp2);
				for (j = 0; j < 2; ++j) {
					buffToNetwork[buffCnter++] = temp2[j];
				}
				temp2[0] = '0'; temp2[1] = '0';
			}
		}

		itoa(myNumChildren, temp1);
		buffToNetwork[buffCnter++] = temp1[0];


		if (myNumChildren == 0) {  //Add only my data
			/*itoa(myBotID, temp1);
			buffToNetwork[buffCnter++] = temp1[0];*/

			for (i = 0; i < MAX_CHILDREN_DATA; ++i) {
				buffToNetwork[buffCnter++] = robotsData[myBotID][i];
			}

		} else {

			for (i = 0; i < myNumChildren; ++i) {
				for (j = 0; j < MAX_CHILDREN_DATA; ++j) {
					buffToNetwork[buffCnter++] = robotsData[i][j];
				}
			}
		}
	}
}

void LongDelay()
{
	__delay_cycles(250000);
}

void startTimer(void)
{
	P1IE |= BIT4;
	startExternalClock();
}

void stopTimer(void)
{
	P1IE &= ~BIT4;
	stopExternalClock();
}

void startRXBots(void)
{
	UCA0IE |= UCRXIE;
}

void stopRXBots(void)
{
	UCA0IE &= ~UCRXIE;
}


void startExternalClock(void)
{
	//P1OUT |= BIT5;

	//P1OUT |= BIT0;	//led0
	//TA0CCR0 = 8192-1; //16364-1; //32768-1;		// Reset the counter register to go up to 32768-1
	//TA0CCTL0 |= CCIE;		// Enable the interrupt

	P1IE  |= BIT4;		// P1.4 interrupt enabled
}

void stopExternalClock(void)
{
	//P1OUT &= ~BIT5;

	//P1OUT &= ~BIT0; //led0
	//TA0CCTL0 &= ~CCIE;		// Disable the interrupt
	//TA0CCR0 = 0;			// Reset the counter register to zero,
	// eliminating any undesired remaining values.

	P1IE  &= ~BIT4;		// P1.4 interrupt enabled
}

void setupPorts(void)
{
	//P1DIR |= BIT5;
	//P1OUT =  0;
	P1DIR |= (BIT0 + BIT1 + BIT2 + BIT5);
	P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT5);
	//P1OUT &= ~BIT5;

	//P1DIR &= ~BIT4;		// P1.4 set to input
	//P1REN |= BIT4;		// P1.4 enable internal pull-down resistor
	//P1REN &= ~BIT4;		// P1.4 disable internal pull-down resistor
	P1OUT &= ~BIT4;		// P1.4 set pull-down resistor on it
	//P1OUT |= BIT4;		// P1.4 set pull-up resistor on it
	//P1IN  &= ~BIT4;		// P1.4 set to Low

	P1IES &= ~BIT4;		// P1.4 Lo/Hi edge interrupt
	P1IE  |= BIT4;		// P1.4 interrupt enabled
	P1IFG &= ~BIT4;		// P1.4 IFG cleared

	/*P3DIR |= 0xFF;
	P3OUT = 0;
	P4DIR |= 0xFF;
	P4OUT = 0;
	PJDIR |= 0xFF;
	PJOUT = 0;*/
}

/*
 * Extremely simplified version of itoa.
 * Size of string is NOT fixed and unknown, it does include
 * the last char in a string '\0' (the null char). It doesn't
 * checks for a sign, it assumes values are always positive.
 * Finally, the function already puts the values of the integer
 * in the correct order, no need to reverse them.
 */
void itoa(int value, char s[])
{
	int i = strlen(s); // MAX_NUM_DIGITS = 2

	do {
		s[--i] = value % 10 + '0';

	} while ( (value /= 10) > 0);
}

char* itoaFixLength(int value, int numDigits)
{
	int i = numDigits + 1; //"+1 = '\0'

	do {
		itoaFixedArray[--i] = value % 10 + '0';

	} while ( (value /= 10) > 0);

	return itoaFixedArray;

}

/*
 * Extremely simplified version of atoi.
 * Size of string is NOT fixed and unknown, it does check for
 * the last char in a string '\0' (the null char) but not for
 * a sign, it assumes values are always positive.
 */
int atoi(char *s)
{
	int n = 0;

	for (n = 0; *s >= '0' && *s <= '9'; s++) {
		n = 10 * n + (*s - '0');
	}

	return n;
}

int atoiFixLength(char s[], int startDigit, int endDigits)
{
	int i = 0, n = 0;
	for (i = startDigit; s[i] >= '0' && s[i] <= '9' && i < endDigits; ++i) {
		n = 10 * n + (s[i] - '0');
	}

	return n;
}

int isBotInMyChildrenList(char botToTest)
{
	int sizeOfChildrenList = 6; //this should be improved
	int i = 0;

	for (i = 0; i < sizeOfChildrenList; ++i) {
		if (botToTest == myChildrenList[i]) {
			return TRUE;
		}
	}
	return FALSE;
}

void putBotMyChildrenList(char botToInsert)
{
	int i = 0; //to include the last char '\0'

	while(myChildrenList[i] != 'y'){ //NLL
		if (myChildrenList[i] == 'x') {
			myChildrenList[i] = botToInsert;
			myNumChildren++;
			break;
		}
		i++;
	}
}

void parsePacket(char packet[])
{
	//network data
	//botID(0), parent(1), timeSlot(2-3), hh(4-5), mm(6-7), ss(8-9)

	//children data
	//numChild(10), botID(11), hh(12-13), mm(14-15), ss(16-17), OX(18-19), PH(20-21), AB(22-23)
	int i = 0, j = 0;
	packetBotId = atoiFixLength(packet, 0, 1);
	packetParent = atoiFixLength(packet, 1, 2);
	packetTimeSlot = atoiFixLength(packet, 2, 4);
	//packetSubTree = atoiFixLength(packet, 4, 5);

	stopExternalClock();
	hrs = atoiFixLength(packet, 4, 6);
	min = atoiFixLength(packet, 6, 8);
	sec = atoiFixLength(packet, 8, 10);
	startExternalClock();

	int maxNetworkSize = 11;

	for (i = 0; i < maxNetworkSize; ++i) {
		networkData[packetBotId][i] = packet[i];
	}

	int numRobChildren = atoiFixLength(packet, 10, 11);
	int indexStart = 11; //index value that represents the start of child data
	//maximum number of char/bytes per child data = MAX_CHILDREN_DATA

	if (numRobChildren == 0) {
		for (i = 0; i < MAX_CHILDREN_DATA; ++i) {
			robotsData[packetBotId][i] = packet[indexStart+i];
		}

	} else {
		for (i = 0; i < numRobChildren; ++i) {
			for (j = 0; j < MAX_CHILDREN_DATA; ++j) {
				robotsData[packet[indexStart+(i*MAX_CHILDREN_DATA)]][j] = packet[(i*MAX_CHILDREN_DATA)+(j+indexStart)];
			}
		}
	}
}

void setRxdFromChildren(int botToSet)
{
	//int tmpBotToSet = atoi(botToSet);
	if (rxdFromChildren[botToSet] == FALSE) {
		rxdFromChildren[botToSet] = TRUE;
	}
}

int rxdFromAllChildrenTest(void)
{
	int i = 0, counter = 0;

	for (i = 0; i < myNumChildren; ++i) {
		if (rxdFromChildren[i] == TRUE) {
			counter++;
		}
	}

	if (counter == myNumChildren) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*void setDoneFinishChildSubTreeList(int botToSet)
{
	if (myChildrenSubTree[botToSet] == FALSE) {
		myChildrenSubTree[botToSet] = TRUE;
	}
}

int isChildrenSubTreeDone()
{
	int i = myNumChildren;
	for (i = myNumChildren; i > 0; i--) {
		if (myChildrenSubTree[i] == 'x') {
			return FALSE;
		}
	}

	return TRUE;
}*/


#endif /* COMMON_FUNCTIONS_H_ */
