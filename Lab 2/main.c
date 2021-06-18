/*
Darek Konopka
Lab 2
*/
//***********************************************************************************************************
//  Author: Girma Tewolde
//	Last modified: 4-9-2020 @5:52 P.M.
//  Purpose: Framework for CE-426 Lab 2
//
//  Use the array data defined in the global memory space and the main program in this file for testing
//  your two versions, i.e. assembly and C, of the findHighBit function.
//  Test each of your two implementations with the given array data and make sure your answers match 
//  with the ones provided in the comments for each array.
//  You will also compare the two implementations on criteria such as execution speed and size of 
//  memory used by the corresponding functions.
//***********************************************************************************************************

#include <stm32f10x.h> 

uint32_t array0[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x12000000,0x80000000}; // answer = 255; 
uint32_t array1[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x12000000,0x00000000}; // answer = 220; 
uint32_t array2[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x02200000,0x00000000,0x00000000}; // answer = 185; 
uint32_t array3[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00440000,0x00000000,0x00000000,0x00000000}; // answer = 150; 
uint32_t array4[] = {0x00000001,0x00000020,0x00000400,0x00008000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 111; 
uint32_t array5[] = {0x00000001,0x00000020,0x00000400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 74; 
uint32_t array6[] = {0x00000001,0x00000020,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 37; 
uint32_t array7[] = {0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000}; // answer = 0; 

uint32_t *arrays[] = {array0, array1, array2, array3, array4, array5, array6, array7};
uint32_t narrays = sizeof(arrays)/sizeof(uint32_t*);
uint8_t nelements = sizeof(array0)/sizeof(uint32_t);

uint32_t findHighBitASM(uint32_t*, uint32_t);
uint32_t findHighBitC(uint32_t*, uint32_t);

int main(void){
	int i;
	volatile int position;
	for (i = 0; i<narrays; i++)
	{
		//position = findHighBitASM(arrays[i], nelements);
		position = findHighBitC(arrays[i], nelements);
	}
	
	while(1){position++;	} // endless loop to keep micro from crashing
						  // position++ keeps position in scope for easier debugging
}

uint32_t findHighBitC(uint32_t* array, uint32_t nelements)
{

	// I used 16bit ints because there is no need to take up so much memory
	uint16_t pos1 = 0; // highest bit position
	uint16_t j = 0; // which 32bit number we are looking at 
	uint32_t temp = 0; //temporary array value
	uint16_t pos2 = 0; // which bit we are looking at

	// look though each element of the array
	for(j = 0; j < nelements; j++) { 
	
		temp = array[j]; // create temp array value
		
		// If 0 then we skip to the next element in the array
		if (temp != 0) {
			// Here we do the bitshift until we hit 1 to find the bit position in this element
			for(pos2 = 0; temp > 1; temp >>= 1) {
				pos2++;
			}
			// for each number we skiped, we multiply j*32 since they are 32bits
			pos1 = j*32 + pos2;
		}  
	}
	// return final position
	return pos1;

}



