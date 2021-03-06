/*
Copyright (c) 2010 Jennifer Holt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//bmp2header_font 
// by Jenn Holt
//converts a bitmap picture containing N character images in the horizontal direction to a c header file of font data for Jenn Holt's sparkfun serial graphic LCD firmware
//the input bitmap should be the height of your characters in the y direction and N*(width of your characters) in the x direction, where N is the number of characters in the font
//Usage:  bmp2header_font [width of character] [width of short character ` ' ; : , .] [font space] [bitmap.bmp] > [file.h]
//height of character is calculated from image, number of characters is int(bitmapwidth/characterwidth], if your image is a non-integral number of character widths wide, the extra will be ignored
//outputs to stdout, use redirection to catch it in a file

#include <stdlib.h>
#include <stdio.h>
#include "qdbmp.h"
#include <string.h>


int main(int argc, char **argv)
{
	BMP* bmp;
	unsigned int y,w,h,Nrows,i,j,k,Nchars, charnum,char_w, char_sw, char_bytes,font_size,font_space,firstbyte;
	unsigned char buff[128]; 	//buffer to hold a row of image data
	unsigned char r,g,b;

  if(argc==5)
  {	

	bmp = BMP_ReadFile(argv[4]);
	BMP_CHECK_ERROR(stderr, -1);     

	/* get the height and width of the image. */
        w = BMP_GetWidth(bmp);
        h = BMP_GetHeight(bmp);
	//get font size data
		char_w = atoi(argv[1]);
		char_sw = atoi(argv[2]);
		font_space = atoi(argv[3]);

	//verify font parameters
	//is size of character valid?
	//is number of bytes/character <=128?

	
	//check to make sure the character is not larger than the display
	if ( (char_w >128) || (h>64) )
	{
		fprintf(stderr,"character is too large. max size is 128x64\n");
		return -1;
	}
	
	//calculate the number of rows necessary(each row is 8 pixels tall)
	Nrows = h/8;			//number of whole rows necessary
	if (h%8!=0)
		Nrows++;		//a partial row counts too

	//calculate the number of characters in the image
	Nchars = (w/char_w);

	//calculate # of bytes per character
	char_bytes = Nrows*char_w;

	//calculate total number of bytes for font

	font_size = char_bytes*Nchars+4;	//char_bytes repeated Nchars times +4 for font width,height,short width,space

	//build font
	
	printf("const unsigned int Font[%d] = {0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x,\n",font_size,char_w,h,char_sw,font_space);	//print the array definition

	//now build image data

	firstbyte=0;					//to get the right number of comma's in the array initialization
	for(charnum=0;charnum<Nchars;charnum++)		//loop for each character in the image	
	{	
		for (i=0;i<Nrows;i++)	//loop for each row
		{
			memset(buff,0x00,128);	//clear all bytes in buffer for each new row
			for (j=0;j<char_w;j++)	//loop for each byte in row
			{
				for(k=0;k<8;k++) //loop for each pixel in byte
				{
					//calculate the y coordinate of this bit(x coordinate is j+charnum*char_w)
					y = i*8+k;	
					BMP_GetPixelRGB(bmp,j+charnum*char_w,y,&r,&g,&b);	//get pixel 
					if ((r+g+b) == 0)	//check if pixel is set
					{
						buff[j] |= ((unsigned char)1 << k);		//set the pixel
					}
			 	}
			}
			//output row of data
			for (y=0;y<char_w;y++)
			{		
				if (firstbyte==0)
					printf("0x%.2x",buff[y]); //write w bytes(1 for each x coordinate)
				else
					printf(", 0x%.2x",buff[y]); //write w bytes(1 for each x coordinate)
				firstbyte=1;
			}
		}
		printf("\n");	//print a newline to make the file pretty	
	}
	printf("};\n");		//finish array definition 
  }
  else
  {
	printf("Usage: bmp2header_font [character width in pixels] [short character width in pixels] [spacing in pixels] [infile.bmp] > header.h\n");
	printf("Character width defines how the input image is divided.\nSpacing is the number of pixels between characters when printed.");
	printf("Converts a 24bbp bitmap image with characters in the x direction into a header file array definition\n");
	printf("with the array name of \"Font\"\n");
	printf("any pixel in the source identically equal to 0 is considered set in the font.\n");
  }
	return 0;
}
