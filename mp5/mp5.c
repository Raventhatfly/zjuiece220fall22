/*									tab:8
 *
 * main.c - skeleton source file for ECE220 picture drawing program
 *
 * "Copyright (c) 2018 by Charles H. Zega, and Saransh Sinha."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Charles Zega, Saransh Sinha
 * Version:	    1
 * Creation Date:   12 February 2018
 * Filename:	    mp5.h
 * History:
 *	CZ	1	12 February 2018
 *		First written.
 */
#include "mp5.h"

/*
	You must write all your code only in this file, for all the functions!
*/



/* 
 *  near_horizontal
 *	 
 *	 
 *	
 *	
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
near_horizontal(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t temp;
	int32_t sign;
	int32_t y;
	int32_t within=1;
	if(x_start>x_end){temp = x_start;x_start=x_end;x_end=temp;temp = y_start;y_start=y_end;y_end=temp;}
	sign = (y_end-y_start>0)?1:-1;
	if(y_end==y_start)	sign = 0;
	for(int x=x_start;x<=x_end;x++){
		y = (2*(y_end-y_start)*(x-x_start)+(x_end-x_start)*sign)/(2*(x_end-x_start))+y_start;
		within&=draw_dot(x,y);
	}
	if(within!=0)	return 1;
	return 0;
}


/* 
 *  near_vertical
 *	 
 *	 
 *	
 *	
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
near_vertical(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t temp;
	int32_t sign;
	int32_t x;
	int32_t within=1;
	if(y_start>y_end){temp = x_start;x_start=x_end;x_end=temp;temp = y_start;y_start=y_end;y_end=temp;}
	sign = (x_end-x_start>0)?1:-1;
	if(x_end==x_start)	sign = 0;
	for(int y=y_start;y<=y_end;y++){
		x = (2*(x_end-x_start)*(y-y_start)+(y_end-y_start)*sign)/(2*(y_end-y_start))+x_start;
		within&=draw_dot(x,y);
	}
	if(within!=0)	return 1;
	return 0;
}

/* 
 *  draw_line
 *	 
 *	 
 *	
 *	
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_line(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t temp;
	int32_t within;
	if(x_start>x_end){temp = x_start;x_start=x_end;x_end=temp;temp = y_start;y_start=y_end;y_end=temp;}
	if((x_start==x_end)&&(y_start==y_end))	near_vertical(x_start,y_start,x_end,y_end);
	int32_t dy = y_end-y_start;
	int32_t dx = x_end-x_start;
	if(dy>dx||dy<-dx)	within = near_vertical(x_start,y_start,x_end,y_end);
	else	within = near_horizontal(x_start,y_start,x_end,y_end);
	if(within!=0)	return 1;
	return 0;
}


/* 
 *  draw_rect
 *	 
 *	 
 *	
 *	
 * INPUTS: x,y -- the coordinates of the of the top-left pixel of the rectangle
 *         w,h -- the width and height, respectively, of the rectangle
 * OUTPUTS: draws a pixel to every point of the edges of the rectangle
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_rect(int32_t x, int32_t y, int32_t w, int32_t h){
	if(w<0||h<0)	return 0;
	int32_t within=1;
	within&=draw_line(x,y,x+w,y);
	within&=draw_line(x,y,x,y+h);
	within&=draw_line(x+w,y,x+w,y+h);
	within&=draw_line(x,y+h,x+w,y+h);
	if(within!=0)	return 1;
	return 0;
}


/* 
 *  draw_triangle
 *	 
 *	 
 *	
 *	
 * INPUTS: x_A,y_A -- the coordinates of one of the vertices of the triangle
 *         x_B,y_B -- the coordinates of another of the vertices of the triangle
 *         x_C,y_C -- the coordinates of the final of the vertices of the triangle
 * OUTPUTS: draws a pixel to every point of the edges of the triangle
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_triangle(int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C){
	int32_t within=1;
	within&=draw_line(x_A,y_A,x_B,y_B);
	within&=draw_line(x_A,y_A,x_C,y_C);
	within&=draw_line(x_C,y_C,x_B,y_B);
	if(within!=0)	return 1;
	return 0;
}

/* 
 *  draw_parallelogram
 *	 
 *	 
 *	
 *	
 * INPUTS: x_A,y_A -- the coordinates of one of the vertices of the parallelogram
 *         x_B,y_B -- the coordinates of another of the vertices of the parallelogram
 *         x_C,y_C -- the coordinates of another of the vertices of the parallelogram
 * OUTPUTS: draws a pixel to every point of the edges of the parallelogram
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_parallelogram(int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C){
	int32_t within = 1;
	int32_t	x_D	= x_A+x_C-x_B;
	int32_t y_D	= y_A+y_C-y_B;
	within&=draw_line(x_A,y_A,x_B,y_B);
	within&=draw_line(x_C,y_C,x_B,y_B);
	within&=draw_line(x_A,y_A,x_D,y_D);
	within&=draw_line(x_C,y_C,x_D,y_D);
	if(within!=0)	return 1;
	return 0;
}


/* 
 *  draw_circle
 *	 
 *	 
 *	
 *	
 * INPUTS: x,y -- the center of the circle
 *         inner_r,outer_r -- the inner and outer radius of the circle
 * OUTPUTS: draws a pixel to every point whose distance from the center is
 * 	    greater than or equal to inner_r and less than or equal to outer_r
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_circle(int32_t x, int32_t y, int32_t inner_r, int32_t outer_r){
	int32_t within = 1;
	if(inner_r<0||outer_r<inner_r)	return 0;
	for(int i=x-outer_r;i<=x+outer_r;i++){
		for(int j=y-outer_r;j<=y+outer_r;j++){
			if((i-x)*(i-x)+(j-y)*(j-y)>=inner_r*inner_r&&(i-x)*(i-x)+(j-y)*(j-y)<=outer_r*outer_r){
				within &= draw_dot(i,j);
			}
		}
	}
	if(within!=0)	return 1;
	return 0;
}


/* 
 *  rect_gradient
 *	 
 *	 
 *	
 *	
 * INPUTS: x,y -- the coordinates of the of the top-left pixel of the rectangle
 *         w,h -- the width and height, respectively, of the rectangle
 *         start_color -- the color of the far left side of the rectangle
 *         end_color -- the color of the far right side of the rectangle
 * OUTPUTS: fills every pixel within the bounds of the rectangle with a color
 *	    based on its position within the rectangle and the difference in
 *          color between start_color and end_color
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
rect_gradient(int32_t x, int32_t y, int32_t w, int32_t h, int32_t start_color, int32_t end_color){
	int32_t within = 1;
	if(w<1||h<0)	return 0;
	int32_t	start[3];
	int32_t end[3];
	int32_t sign[3];
	int32_t mask = 0x000000ff;
	for(int i=0;i<3;i++){
		start[i] = start_color & mask;
		end[i] = end_color & mask;
		sign[i]= end[i]>start[i]?1:-1;
		if(end[i]==start[i])	sign[i]=0;
		start_color>>=8;
		end_color>>=8;
	}
	for(int32_t i=x;i<=x+w;i++)
	{	
		int32_t level_r=(2*(i-x)*(end[0]-start[0])+w*sign[0])/(2*w)+start[0];
		int32_t level_g=(2*(i-x)*(end[1]-start[1])+w*sign[1])/(2*w)+start[1];
		int32_t level_b=(2*(i-x)*(end[2]-start[2])+w*sign[2])/(2*w)+start[2];
		set_color((level_b<<16)+(level_g<<8)+(level_r));
		for(int32_t j=y;j<=y+h;j++){
			within &= draw_dot(i,j);
		}
	}
	if(within!=0)	return 1;	
	return 0;

}


/* 
 *  draw_picture
 *	 
 *	 
 *	
 *	
 * INPUTS: none
 * OUTPUTS: alters the image by calling any of the other functions in the file
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */


int32_t
draw_picture(){
	/* Your code goes here! */


	return 0;
}
