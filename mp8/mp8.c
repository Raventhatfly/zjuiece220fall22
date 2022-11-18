#include <stdlib.h>
#include <string.h>

#include "mp8.h"
/*
My Work in MP8
In mp8.c I wrote four functions, basicFlood, colorwithinDistSq, greyFlood
and limitedFlood. In these functions I wrote the code to fullfill some
operations. I wrote a Euclidean distance calculation in colorwithinDistSq,
and use recurence function in the rest three. The recurence function can 
help me find all of the pixels needs to be filled.
Then in mp8recurse.c I wrote the details of the recurse functions.
*/

/*
 * basicFlood -- wrapper for flood filling recursively from a point until 
 *               reaching white or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         startX -- starting x position of the flood
 *         startY -- starting y position of the flood
 *         floodR -- red component of the flood color
 *         floodG -- green component of the flood color
 *         floodB -- blue component of the flood color
 * OUTPUTS: outRed -- pointer to the output red channel (1-D array)
 *          outGreen -- pointer to the output green channel (1-D array)
 *          outBlue -- pointer to the output blue channel (1-D array)
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
basicFlood (int32_t width, int32_t height,
	    const uint8_t* inRed, const uint8_t* inGreen, 
	    const uint8_t* inBlue, 
	    int32_t startX, int32_t startY, 
	    uint8_t floodR, uint8_t floodG, uint8_t floodB,
	    uint8_t* outRed, uint8_t* outGreen, 
	    uint8_t* outBlue)
{
	int32_t index;
	memset(outRed,0,height*width);	//clear the marker array
	//start recurse
	basicRecurse(width,height,inRed,inGreen,inBlue,startX,startY,outRed);
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			index = i * width + j; //calcualte the index
			if(outRed[index]==0){
				// if pixel not being marked, copy the original color
				outRed[index]=inRed[index];
				outBlue[index]=inBlue[index];
				outGreen[index]=inGreen[index];
			}
			else{	
				// if the pixel being marked, fill the plot with given color
				outRed[index]=floodR;
				outBlue[index]=floodB;
				outGreen[index]=floodG;
			}
		}
	}
}


/*
 * colorsWithinDistSq -- returns 1 iff two colors are within Euclidean
 *                       distance squared of one another in RGB space
 * INPUTS: r1 -- red component of color 1
 *         g1 -- green component of color 1
 *         b1 -- blue component of color 1
 *         r2 -- red component of color 2
 *         g2 -- green component of color 2
 *         b2 -- blue component of color 2
 *         distSq -- maximum distance squared for the check
 * RETURN VALUE: 1 if the sum of the squares of the differences in the 
 *               three components is less or equal to distSq; 0 otherwise
 * SIDE EFFECTS: none
 */
int32_t
colorsWithinDistSq (uint8_t r1, uint8_t g1, uint8_t b1,
                    uint8_t r2, uint8_t g2, uint8_t b2, uint32_t distSq)
{
	if((r1-r2)*(r1-r2)+(g1-g2)*(g1-g2)+(b1-b2)*(b1-b2)<=distSq){
		// calculate Euclidean distance square, return 1 if samller than the set value
		return 1;
	}
	return 0;
}


/*
 * greyFlood -- wrapper for flood filling recursively from a point until 
 *              reaching near-white pixels or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         startX -- starting x position of the flood
 *         startY -- starting y position of the flood
 *         floodR -- red component of the flood color
 *         floodG -- green component of the flood color
 *         floodB -- blue component of the flood color
 *         distSq -- maximum distance squared between white and boundary
 *                   pixel color
 * OUTPUTS: outRed -- pointer to the output red channel (1-D array)
 *          outGreen -- pointer to the output green channel (1-D array)
 *          outBlue -- pointer to the output blue channel (1-D array)
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
greyFlood (int32_t width, int32_t height,
	   const uint8_t* inRed, const uint8_t* inGreen, 
	   const uint8_t* inBlue, 
	   int32_t startX, int32_t startY, 
	   uint8_t floodR, uint8_t floodG, uint8_t floodB, uint32_t distSq,
	   uint8_t* outRed, uint8_t* outGreen, 
	   uint8_t* outBlue)
{
	int32_t index;
	memset(outRed,0,height*width);		//clear the marker array
	//start recurse
	greyRecurse(width,height,inRed,inGreen,inBlue,startX,startY,distSq,outRed);
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			index = i * width + j;
			if(outRed[index]==0){
				// if pixel not being marked, copy the original color
				outRed[index]=inRed[index];
				outBlue[index]=inBlue[index];
				outGreen[index]=inGreen[index];
			}
			else{
				// if the pixel being marked, fill the plot with given color
				outRed[index]=floodR;
				outBlue[index]=floodB;
				outGreen[index]=floodG;
			}
		}
	}
}


/*
 * limitedFlood -- wrapper for flood filling recursively from a point until 
 *                 reaching pixels too different (in RGB color) from the 
 *                 color at the flood start point, too far away 
 *                 (> 35 pixels), or beyond the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         startX -- starting x position of the flood
 *         startY -- starting y position of the flood
 *         floodR -- red component of the flood color
 *         floodG -- green component of the flood color
 *         floodB -- blue component of the flood color
 *         distSq -- maximum distance squared between pixel at origin 
 *                   and boundary pixel color
 * OUTPUTS: outRed -- pointer to the output red channel (1-D array)
 *          outGreen -- pointer to the output green channel (1-D array)
 *          outBlue -- pointer to the output blue channel (1-D array)
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
limitedFlood (int32_t width, int32_t height,
	      const uint8_t* inRed, const uint8_t* inGreen, 
	      const uint8_t* inBlue, 
	      int32_t startX, int32_t startY, 
	      uint8_t floodR, uint8_t floodG, uint8_t floodB, uint32_t distSq,
	      uint8_t* outRed, uint8_t* outGreen, 
	      uint8_t* outBlue)
{
	int32_t index;
	memset(outRed,0,height*width);	//clear the marker array
	//start recurse
	limitedRecurse(width,height,inRed,inGreen,inBlue,startX,startY,startX,startY,distSq,outRed); 
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			index = i * width + j;
			if(outRed[index]==0){
				// if pixel not being marked, copy the original color
				outRed[index]=inRed[index];
				outBlue[index]=inBlue[index];
				outGreen[index]=inGreen[index];
			}
			else{
				// if the pixel being marked, fill the plot with given color
				outRed[index]=floodR;
				outBlue[index]=floodB;
				outGreen[index]=floodG;
			}
		}
	}
}


