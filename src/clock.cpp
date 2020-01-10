#include "clock.h"
MD_MAX72XX* mx72;
uint16_t colcount;
void initMX(MD_MAX72XX* mxin){
  mx72 = mxin;
  colcount=mx72->getColumnCount()-1;

}
void draw_myfont(uint8_t column, const uint8_t *fontdata, uint8_t number) {
	// Draw a digit on the canvas at position specified by column
	uint8_t i,j;
	for(i=0;i<4;i++) {
		for(j=0;j<8;j++) {
			bool val;
			val=(pgm_read_byte(fontdata + number * 4 + i)) & (1<<j);
			mx72->setPoint(j,colcount-(i+column),val);
		}
	}
}

void draw_clock(uint8_t hour,uint8_t minute,uint8_t second)
{
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
	mx72->clear();
	if(hour>=10) draw_myfont(0,font4x8,hour/10);
	else draw_myfont(0,font4x8,10); //blank
	draw_myfont(5,font4x8,hour%10);
  mx72->setPoint(2,21,true);
  mx72->setPoint(5,21,true);
	draw_myfont(12,font4x8,minute/10);
	draw_myfont(17,font4x8,minute%10);
	draw_myfont(23,font4x7,second/10);
	draw_myfont(28,font4x7,second%10);
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);//  refreshAll();	
}

void drawReset()
{
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
	mx72->clear();
  drawString(31,font,"Reset");
/*  draw_myfont(0,font,'R');
  draw_myfont(6,font,'e'); 
  draw_myfont(12,font,'s'); 
  draw_myfont(18,font,'e'); 
  draw_myfont(24,font,'t'); */
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);//  refreshAll();	
}

void drawIP()
{
  //note setPoint column 0 is right side, dwar_myfont column 0 is left side
  Serial.println("Cols ");
  Serial.println(mx72->getColumnCount());
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
	mx72->clear();
 //draws 192.168.4.1
  //draw a 1
   mx72->setPoint(1,31,1);
   mx72->setPoint(2,31,1);
   mx72->setPoint(3,31,1);
   mx72->setPoint(4,31,1);
   mx72->setPoint(5,31,1);
   mx72->setPoint(6,31,1);
   mx72->setPoint(7,31,1);

  draw_myfont(2,font4x7,9); 
  draw_myfont(6,font4x7,2); 
  // draw a decimal place
  mx72->setPoint(7,20,1);

  //draw a 1
   mx72->setPoint(1,18,1);
   mx72->setPoint(2,18,1);
   mx72->setPoint(3,18,1);
   mx72->setPoint(4,18,1);
   mx72->setPoint(5,18,1);
   mx72->setPoint(6,18,1);
   mx72->setPoint(7,18,1);
  draw_myfont(14,font4x7,6); 
  draw_myfont(18,font4x7,8); 
  
  mx72->setPoint(7,8,1);//draw decimal point
  draw_myfont(24,font4x7,4); 

  mx72->setPoint(7,2,1);//draw decimal point
  //draw a '.1'
  mx72->setPoint(1,0,1);
  mx72->setPoint(2,0,1);
  mx72->setPoint(3,0,1);
  mx72->setPoint(4,0,1);
  mx72->setPoint(5,0,1);
  mx72->setPoint(6,0,1);
  mx72->setPoint(7,0,1);
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);//  refreshAll();	
}



void drawChar(uint8_t col, const uint8_t *fontdata, const char ch)
{
  int len = pgm_read_byte(fontdata);
  int w = pgm_read_byte(fontdata + 1 + ch * len);
  for (int i = 0; i < w; i++)
    if(col+i>=0 && col+i<8*NUM_MAX) {
      byte v = pgm_read_byte(fontdata + 1 + ch * len + 1 + i);
			for(int y=0;y<8;y++) mx72->setPoint(y,colcount-(col+i),bitRead(v,y) ? 1 : 0);
    }
}

void drawString(uint8_t col, const uint8_t *fontdata, const char *string)
{
  col=colcount-col; 
  int len = pgm_read_byte(fontdata);
	while(*string)
	{
  	int w = pgm_read_byte(fontdata + 1 + (*string) * len);
		drawChar(col,fontdata,*string);
		col+=w+1;
		string++;
	}
}

void drawStringClr(uint8_t col, const uint8_t *fontdata, const char *string)
{
	mx72->clear();
	drawString(col, fontdata, string);
  mx72->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);//  refreshAll();	
}
