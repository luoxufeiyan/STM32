
/******************************************************
* 函数名：Touchl_Calibrate
* 描述  ：触摸屏校正函数
* 输入  : 无
* 输出  ：0	---	校正成功
					1	---	校正失败
* 举例  ：无
* 注意  ：无
*********************************************************/ 
int Touch_Calibrate(void)
{
    uint8_t i,k;
    u16 test_x=0, test_y=0;
    u16 gap_x=0, gap_y=0;
    Coordinate * Ptr;   
    
		Lcd_GramScan(2);
    for(i=0; i<4; i++)
    {        
        LCD_Clear(0, 0, 320, 240, BACKGROUND);        
        LCD_DispStr(110, 110, (uint8_t *)"Touch Calibrate......", BLUE);	         
        LCD_DisNum(160, 90, i+1, BLUE);
      
        /* 适当的延时很有必要 */        
		Delay_ms(500);   
        DrawCross(DisplaySample[i].x,DisplaySample[i].y);  //显示校正用的“十”字
        do
        {
            Ptr=Read_2046_2();  //读取TSC2046数据到变量ptr            
        }
        while( Ptr == (void*)0 );     //当ptr为空时表示没有触点被按下
        ScreenSample[i].x= Ptr->x; 	  //把读取的原始数据存放到全局变量ScreenSample结构体
        ScreenSample[i].y= Ptr->y;

    }
		/* 用原始参数计算出 原始参数与坐标的转换系数。 */ 
    Cal_touch_para( &DisplaySample[0],&ScreenSample[0],&touch_para ) ;  	   
    
		/*取一个点计算X值*/
    test_x = ( (touch_para.An * ScreenSample[3].x) + 
               (touch_para.Bn * ScreenSample[3].y) + 
                touch_para.Cn 
             ) / touch_para.Divider ;			 
    
		/*取一个点计算Y值*/
    test_y = ( (touch_para.Dn * ScreenSample[3].x) + 
               (touch_para.En * ScreenSample[3].y) + 
               touch_para.Fn 
             ) / touch_para.Divider ;
    
    /* 实际坐标与计算坐标的差 */
    gap_x = (test_x > DisplaySample[3].x)?(test_x - DisplaySample[3].x):(DisplaySample[3].x - test_x);
    gap_x = (test_y > DisplaySample[3].y)?(test_y - DisplaySample[3].y):(DisplaySample[3].y - test_y);
    

    //LCD_Rectangle(0,0,320,240,CAL_BACKGROUND_COLOR);
    LCD_Clear(0, 0, 320, 240, BACKGROUND);
    
    /* 可以通过修改这两个值的大小来调整精度 */
    if((gap_x>10)||(gap_y>10))
    {
      LCD_DispStr(100, 100, (uint8_t *)"Calibrate fail", BLUE);
      LCD_DispStr(100, 120, (uint8_t *)"try again", BLUE);     
     // Delay_ms(2000);
			DelayUS(0x1ffff);
      return 1;
    }    
    
    /* 校正系数为全局变量 */
//    aa1 = (touch_para.An*1.0)/touch_para.Divider;
//    bb1 = (touch_para.Bn*1.0)/touch_para.Divider;
//    cc1 = (touch_para.Cn*1.0)/touch_para.Divider;
//    
//    aa2 = (touch_para.Dn*1.0)/touch_para.Divider;
//    bb2 = (touch_para.En*1.0)/touch_para.Divider;
//    cc2 = (touch_para.Fn*1.0)/touch_para.Divider; 

    cal_p[0] = (touch_para.An*1.0)/touch_para.Divider;  // aa1
    cal_p[1] = (touch_para.Bn*1.0)/touch_para.Divider;  // bb1
    cal_p[2] = (touch_para.Cn*1.0)/touch_para.Divider;  // cc1
    
    cal_p[3] = (touch_para.Dn*1.0)/touch_para.Divider;  // aa2
    cal_p[4] = (touch_para.En*1.0)/touch_para.Divider;  // bb2
    cal_p[5] = (touch_para.Fn*1.0)/touch_para.Divider;  // cc2     
 
    {
      cal_flag = 0x45;
      SPI_FLASH_SectorErase(0);
      SPI_FLASH_BufferWrite(&cal_flag, 0, 1);     
      SPI_FLASH_BufferWrite((void*)cal_p, 1, sizeof(cal_p));
      for( k=0; k<6; k++ )
        printf("\r\n tx = %LF \r\n",cal_p[k]);
    }
    
    LCD_DispStr(100, 100, (uint8_t *)"Calibrate Succed", BLUE);  
    Delay_ms(1000);
    return 0;    
}

/*
 * 画板初始化，用于取色用
 */
void Palette_Init(void)
{
  /* 整屏清为白色 */
  LCD_Clear(0, 0, 320, 240, WHITE);
  
  /* 画两条直线 */
  LCD_Clear(39, 0, 1, 30, BLACK);
  LCD_Clear(0, 29, 40, 1, BLACK);
  LCD_DispStr(7, 10, (uint8_t *)"CLR", BLUE);
  
  LCD_Clear(0, 30, 40, 30, GREEN);
  LCD_Clear(0, 60, 40, 30, BLUE);
  LCD_Clear(0, 90, 40, 30, BRED);
  LCD_Clear(0, 120, 40, 30, GRED);
  LCD_Clear(0, 150, 40, 30, GBLUE);
  LCD_Clear(0, 180, 40, 30, BLACK);
  LCD_Clear(0, 210, 40, 30, RED);  
  
 // Delay_ms(500);
	DelayUS(0x1ffff);
}

/******************************************************
* 函数名：Get_touch_point
* 描述  ：通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：如果获取的触点信息有误，将返回DISABLE
*********************************************************/    
//long double linear=0 ;
//long double aa1=0,bb1=0,cc1=0,aa2=0,bb2=0,cc2=0;
FunctionalState Get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para )
{
  FunctionalState retTHRESHOLD =ENABLE ;

  if(screenPtr==0)
  {
    /*如果获取的触点信息有误，则返回DISABLE*/
    retTHRESHOLD = DISABLE;			
  }
  else
  {    
    //if( para->Divider != 0 )  /* 每次都要校正时 */
    if( para->Divider != 1 )    /* 校正系数写到FLASH时 */
    {        
      //displayPtr->x = ( (aa1 * screenPtr->x) + (bb1 * screenPtr->y) + cc1);        
      //displayPtr->y = ((aa2 * screenPtr->x) + (bb2 * screenPtr->y) + cc2 );
      displayPtr->x = ( (cal_p[0] * screenPtr->x) + (cal_p[1] * screenPtr->y) + cal_p[2]);        
      displayPtr->y = ((cal_p[3] * screenPtr->x) + (cal_p[4] * screenPtr->y) + cal_p[5] );
    }
    else
    {
      retTHRESHOLD = DISABLE;
    }
  }
  return(retTHRESHOLD);
} 

/******************************************************
* 函数名：Palette_draw_point
* 描述  ：在LCD指定位置画一个大点(包含四个小点)
* 输入  : Xpos		--X方向位置
*         Ypos		--Y方向位置
* 输出  ：无
* 举例  ：Palette_draw_point(100,100);
* 注意  ：该函数是 "触摸画板应用实例" 专用函数
*********************************************************/    
void Palette_draw_point(uint16_t x, uint16_t y)
{
  /* 画笔默认为黑色 */
  static u16 Pen_color=0; 
  u16 y_pos = y;

  /* 在画板内取色 */
  if( x<40 )
  {
    if( y>30 )
    Pen_color = (y_pos<60)?GREEN:\
                (y_pos<90)?BLUE:\
                (y_pos<120)?BRED:\
                (y_pos<150)?GRED:\
                (y_pos<180)?GBLUE:\
                (y_pos<210)?BLACK:\
                (y_pos<240)?RED:BLUE;
    else
    {/* 清屏 */
      #if 0
      LCD_Clear(40, 0, 280, 240, WHITE);
      #elif 1
      LCD_Clear(40, 0, 280, 240, BLACK);
      #endif
      return;
    }
  }
  else
  {
    LCD_SetPoint(x , y , Pen_color);
    LCD_SetPoint(x+1 , y , Pen_color);
    LCD_SetPoint(x , y+1 , Pen_color);
    LCD_SetPoint(x+1 , y+1 , Pen_color);
  }	
}