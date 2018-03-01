
/*

File receive.c


Purpose: simulate the receiveing end of a message over the network.

The program converts a message encodded as an array of short integers into characters.
The input message uses 1-bit error correction code.
As part of the simulation, the program also corrects any 1-bit errors in the receiving message.


Revision:
a. Initial code - Doron Nussbaum


*/


/************************************************************************/

// INCLUDE FILES

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "bit_manipulation.h"


/************************************************************************/
//MACROS  / DEFINES



#define XSTR(A) STR(A)
#define STR(A) #A
#define MAX_INPUT_LENGTH 8192
#define MAX_MSG_LENGTH 2048
#define P1_MASK 0xaa8   // 0b0101010101000
#define P2_MASK 0xcc8	// 0b0110011001000
#define P4_MASK 0x10e0	// 0b1000011100000
#define P8_MASK 0x1e00  // 0b1111000000000


/************************************************************************/
// FUNCTION PROTOTYPES


void short2Char(short encodedChar, char *c);
void correctCode(short *num);
void readEncodedMessage(short *buf, int len, int *numRead);




/**************************************************************************/

int main(int argc, char *argv[])

{
	int rc = 0;		// return code

	char s[MAX_MSG_LENGTH] = { '\0' };  // message
	short encodedMsg[MAX_MSG_LENGTH];
	int numRead = 0;  // number of characters in the message
	int i;

	// read the message
	printf("Please enter the transmitted message: ");
	readEncodedMessage(encodedMsg, MAX_MSG_LENGTH, &numRead);



	// print the corrected message
	printf("\n\n Transmitted Message:\n");
	for (i = 0; i < numRead; i++) {
		char c;
		short2Char(encodedMsg[i], &c);
		printf("%c", c);
	}
	printf("\n");

	// correct the message
	for (i = 0; i < numRead; i++) {
		correctCode(&encodedMsg[i]);
	}



	// print the corrected message
	printf("\n\n Corrected Transmitted Message:\n");
	for (i = 0; i < numRead; i++) {
		char c;
		short2Char(encodedMsg[i], &c);
		printf("%c", c);
	}
	printf("\n");

	return(0);
}


/***********************************************************************************/
/* reads a message from the user

input
len - maximum length of mesage that should be read

output
receivedMsg - contains the message
numRead - the number of characters in the message

*/


void readEncodedMessage(short *receivedMsg, int len, int *numRead)
{
	int i;
	int rc;
	char s[MAX_INPUT_LENGTH+1];
	char *token = NULL;
	*numRead = 0;

	s[MAX_INPUT_LENGTH] = '\0';
	scanf("%"XSTR(MAX_INPUT_LENGTH)"[^\n]s", s);
	token = strtok(s, " ");

	for (i = 0; token != NULL  &&  i < len;token = strtok(NULL, " "), i++) {
		rc = sscanf(token, "%hd", &receivedMsg[i]);
	}

	*numRead = i; // set the number of read integers

	// empty the input buffer
	for( ; getchar() != '\n'; );


}




/*************************************************************************/
/* collect of a short bits 3,5,6,7,9,10,11,12  to bits 0-7 of a char

input
encodedNum - a short with the bits of c

output
c - the char that is embedded in encodedNum

*/


void short2Char(short encodedNum, char *c)

{
	int bitSet;

	*c = 0;
	for (int i = 0; i < 13; i++) {
		// set the correct bit in char
		bitSet = isShortBitSet(encodedNum, i);
		if (!bitSet){
			continue;
		}
		else{
			if(i == 3){
				setCharBit(0, c);
			}else if(i == 5){
				setCharBit(1, c);
			}else if(i == 6){
				setCharBit(2, c);
			}else if(i == 7){
				setCharBit(3, c);
			}else if(i == 9){
				setCharBit(4, c);
			}else if(i == 10){
				setCharBit(5, c);
			}else if(i == 11){
				setCharBit(6, c);
			}else if(i == 12){
				setCharBit(7, c);
			}
		}
	}

}


/*************************************************************************************/


/* purpose:
performs error corrrections, if needed on the short integer

The function first checks the parity bits for error.
If there is an error then it fixes it.

input
num - the address of the short number
*/


void correctCode(short *num)

{
	int p1;
	int p1_sum;
	int p2;
	int p2_sum;
	int p4;
	int p4_sum;
	int p8;
	int p8_sum;
	int mask;
	int sum;
	int bitNumber = 0; //  bit number with the error bit
	int parity;		// a parity bit either 0 or 1
	int storedParity1; // the parity bit in the encoded char
	int storedParity2; // the parity bit in the encoded char
	int storedParity4; // the parity bit in the encoded char
	int storedParity8; // the parity bit in the encoded char
	// check parity bit p1
	storedParity1 = isShortBitSet(*num,1);
	// get the bits related to P1 using P1_MASK
	p1 = P1_MASK & *num;
	// count the bits
	p1_sum = shortIntcountBits(p1);
	// determine if the parity bit should have been set
	if((p1_sum % 2) == 1){
		parity = 1;
	}else {
		parity = 0;
	}
	// compare the calculated parity with the stored parity
	if(storedParity1 != parity) {
		bitNumber += 1; // if the two parities are different add 2^0 to bitNumber
	}

	// simlilary check parity bit p2
	storedParity2 = isShortBitSet(*num,2);
	// get the bits related to P1 using P1_MASK
	p2 = P2_MASK & *num;
	// count the bits
	p2_sum = shortIntcountBits(p2);
	// determine if the parity bit should have been set
	if((p2_sum % 2) == 1){
		parity = 1;
	}else {
		parity = 0;
	}
	// compare the calculated parity with the stored parity
	if(storedParity2 != parity) {
		bitNumber += 2; // if the two parities are different add 2^0 to bitNumber
	}

	// check parity bit p4
	storedParity4 = isShortBitSet(*num,4);
	// get the bits related to P1 using P1_MASK
	p4 = P4_MASK & *num;
	// count the bits
	p4_sum = shortIntcountBits(p4);
	// determine if the parity bit should have been set
	if((p4_sum % 2) == 1){
		parity = 1;
	}else {
		parity = 0;
	}
	// compare the calculated parity with the stored parity
	if(storedParity4 != parity) {
		bitNumber += 4; // if the two parities are different add 2^0 to bitNumber
	}


	// check parity bit p8
	storedParity8 = isShortBitSet(*num,8);
	// get the bits related to P1 using P1_MASK
	p8 = P8_MASK & *num;
	// count the bits
	p8_sum = shortIntcountBits(p8);
	// determine if the parity bit should have been set
	if((p8_sum % 2) == 1){
		parity = 1;
	}else {
		parity = 0;
	}
	// compare the calculated parity with the stored parity
	if(storedParity8 != parity) {
		bitNumber += 8; // if the two parities are different add 2^0 to bitNumber
	}
	// if necessary flip the bit at position bitNumber
	if(bitNumber != 0){
		flipBitShort(bitNumber,num);
	}

}

