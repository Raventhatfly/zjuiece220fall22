#include <stdlib.h>
#include <string.h>

#include "mp8.h"
/*
My work in np8recurse.c
In this file I wrote three functions, basicRecurse, greyRecurse and 
limitedRecurse. The recursive functions call themselves within the
function, and it is my job to justify the stop limits of all these 
functions, and them apply the recursive method by detecting whether 
the four direction on the current pixel should be manipulated during
the next recursive process. The functions will mark the pixels to be 
colored as 1 and 0 otherwise.

*/

/*
 * basicRecurse -- flood fill recursively from a point until reaching
 *                 white or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
basicRecurse (int32_t width, int32_t height,
	      const uint8_t* inRed, const uint8_t* inGreen, 
	      const uint8_t* inBlue, 
	      int32_t x, int32_t y, 
	      uint8_t* marking)
{	
	int32_t index;
	int32_t temp;
	index = y*width+x;
	// if the pixel is a white pixel, stop recurse
	if(inRed[index]==255&&inBlue[index]==255&&inGreen[index]==255)	return;
	temp=marking[index];
	marking[index]=1;	//mark current pixel
	if(temp==0){
		// judge boundary, if next recurese pixel valid recurse that pixel
		if(x-1>=0)		basicRecurse(width,height,inRed,inGreen,inBlue,x-1,y,marking);
		if(x+1<width)	basicRecurse(width,height,inRed,inGreen,inBlue,x+1,y,marking);
		if(y-1>=0)		basicRecurse(width,height,inRed,inGreen,inBlue,x,y-1,marking);
		if(y+1<height)	basicRecurse(width,height,inRed,inGreen,inBlue,x,y+1,marking);
	}
	
}


/*
 * greyRecurse -- flood fill recursively from a point until reaching
 *                near-white pixels or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         distSq -- maximum distance squared between white and boundary
 *                   pixel color
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
greyRecurse (int32_t width, int32_t height,
	     const uint8_t* inRed, const uint8_t* inGreen, 
	     const uint8_t* inBlue, 
	     int32_t x, int32_t y, uint32_t distSq, 
	     uint8_t* marking)
{
	int32_t index;
	int32_t temp;
	index = y*width+x;	// calculate index
	// if Euclidean distance in RGB space is too large, stop recursion
	if(colorsWithinDistSq(inRed[index],inGreen[index],inBlue[index],
			255,255,255,distSq))	return;
	temp=marking[index];
	marking[index]=1;
	if(temp==0){
		// judge boundary, if next recurese pixel valid recurse that pixel
		if(x-1>=0)		greyRecurse(width,height,inRed,inGreen,inBlue,x-1,y,distSq,marking);
		if(x+1<width)	greyRecurse(width,height,inRed,inGreen,inBlue,x+1,y,distSq,marking);
		if(y-1>=0)		greyRecurse(width,height,inRed,inGreen,inBlue,x,y-1,distSq,marking);
		if(y+1<height)	greyRecurse(width,height,inRed,inGreen,inBlue,x,y+1,distSq,marking);
	}
}


/*
 * limitedRecurse -- flood fill recursively from a point until reaching
 *                   pixels too different (in RGB color) from the color at
 *                   the flood start point, too far away (> 35 pixels), or
 *                   beyond the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         origX -- starting x position of the flood
 *         origY -- starting y position of the flood
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         distSq -- maximum distance squared between pixel at origin 
 *                   and boundary pixel color
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
limitedRecurse (int32_t width, int32_t height,
	        const uint8_t* inRed, const uint8_t* inGreen, 
	        const uint8_t* inBlue, 
	        int32_t origX, int32_t origY, int32_t x, int32_t y, 
		uint32_t distSq, uint8_t* marking)
{
	int32_t index = y * width + x;
	int32_t temp;
	int32_t new_index;
	int32_t orig_index = origY * width + origX; // index of the origon
	// distance larger than 35stop recusion
	if((origX-x)*(origX-x)+(origY-y)*(origY-y)>35*35)	return; 
		
	temp=marking[index];
	marking[index]=1;		//mark the current pixel
	if(temp==0){
		if(x-1>=0){
			// see if the left pixel is valid
			new_index = index - 1;
			if(colorsWithinDistSq(inRed[orig_index],inGreen[orig_index],inBlue[orig_index],
			inRed[new_index],inGreen[new_index],inBlue[new_index],distSq)){
				// start recurse only when within certain Euclidean distance in RGB space.
				limitedRecurse(width,height,inRed,inGreen,inBlue,origX,origY,x-1,y,distSq,marking);
			}
		}
		if(x+1<width){
			// see if the right pixel is valid
			new_index = index + 1;
			if(colorsWithinDistSq(inRed[orig_index],inGreen[orig_index],inBlue[orig_index],
			inRed[new_index],inGreen[new_index],inBlue[new_index],distSq)){
				// start recurse only when within certain Euclidean distance in RGB space.
				limitedRecurse(width,height,inRed,inGreen,inBlue,origX,origY,x+1,y,distSq,marking);
			}
		}
		if(y-1>=0){
			// see if the upper pixel is valid
			new_index = index - width;
			if(colorsWithinDistSq(inRed[orig_index],inGreen[orig_index],inBlue[orig_index],
			inRed[new_index],inGreen[new_index],inBlue[new_index],distSq)){
				// start recurse only when within certain Euclidean distance in RGB space.
				limitedRecurse(width,height,inRed,inGreen,inBlue,origX,origY,x,y-1,distSq,marking);
			}
		}
		if(y+1<height){
			// see if the downward pixel is valid
			new_index = index + width;
			if(colorsWithinDistSq(inRed[orig_index],inGreen[orig_index],inBlue[orig_index],
			inRed[new_index],inGreen[new_index],inBlue[new_index],distSq)){
				// start recurse only when within certain Euclidean distance in RGB space.
				limitedRecurse(width,height,inRed,inGreen,inBlue,origX,origY,x,y+1,distSq,marking);
			}
		}
	}
}


