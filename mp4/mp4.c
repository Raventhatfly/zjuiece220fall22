#include "mp4.h"
#include "stdio.h"

/********************************************************************************/
//  mp4.c contain the function print_row
//  whose function is to print X in the palces where the black pixels must exist
//  and print "." if the black pixels is not destined to exist at the location
//  it takes four parameters r1,r2,r3,r4 as blocks of black pixels in the nanogram
/********************************************************************************/

int32_t print_row (int32_t r1, int32_t r2, int32_t r3, int32_t r4, int32_t width){
    int32_t min_w;
    int32_t input_num = 0;
    int32_t r_temp[4]={r1,r2,r3,r4};
    int32_t r[4];
    int32_t r_new[4];
    int32_t sum = r1+r2+r3+r4;
    // exclude 0s in between the four inputs r1 to r4
    for(int i=0;i<4;i++){
        r[i]=0;
        if(r_temp[i]!=0){       //if rx non zero, put it into the array r
            r[input_num]=r_temp[i];
            input_num+=1;
        }
    }   
    min_w = sum + input_num-1;      //calculate the minimum width to fit
    if(min_w > width)   return 0;   //can not fit the blocks, return exit code 0
    // calculate the order relative to the filled fit solution
    int32_t order = width - min_w;
    // calcualte the number of the remaining destined black pixels
    for(int i=0;i<4;i++){
        if(r[i]>=order) r_new[i] = r[i] -order;
        else    r_new[i] = 0;     // the number of newly pirnted black pixels can't be smaller than 0 
    }
    //ready to print the line
    for(int s=0;s<input_num;s++){
        for(int j=r[s]-r_new[s];j>0;j--)    printf(".");
        for(int j=r_new[s];j>0;j--)         printf("X");
        if(s!=input_num-1)                  printf(".");
    }
    //print the remaining "."
    for(int i=order;i>0;i--)    printf(".");
    // ready to print next line 
    printf("\n");
    return 1;
};
