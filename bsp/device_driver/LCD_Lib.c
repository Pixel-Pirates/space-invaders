/*
 * LCDLib.c
 */

#include "../bsp.h"
#include "LCD_Lib.h"
//#include "driverlib.h"
#include "AsciiLib.h"

extern void F28x_usDelay(long LoopCount);

//
// DO NOT MODIFY THIS LINE.
//
#define DELAY_US(A)  F28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)


/************************************  Private Functions  *******************************************/

/*
 * Delay x ms
 */
static void Delay(unsigned long interval)
{
    while(interval > 0)
    {
        F28x_usDelay(83000);
        interval--;
    }
}

void InitSpiaGpio()
{
   EALLOW;

    //
    // Enable internal pull-up for the selected pins
    //
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;  // Enable pull-up on GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;   // Enable pull-up on GPIO5 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;  // Enable pull-up on GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pull-up on GPIO3 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;  // Enable pull-up on GPIO18 (SPICLKA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  // Enable pull-up on GPIO19 (SPISTEA)

    //
    // Set qualification for selected pins to asynch only
    //
    // This will select asynch (no qualification) for the selected pins.
    // Comment out other unwanted lines.
    //
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;  // Asynch input GPIO5 (SPISIMOA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;  // Asynch input GPIO3 (SPISOMIA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; // Asynch input GPIO18 (SPICLKA)
//  GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

    GPIO_SetupPinOptions(60, GPIO_INPUT, GPIO_PULLUP);  //CLK
    GPIO_SetupPinOptions(59, GPIO_INPUT, GPIO_PULLUP);  //MISO
    GPIO_SetupPinOptions(58, GPIO_OUTPUT, GPIO_PULLUP); //MOSI

    //
    //Configure SPI-A pins using GPIO regs
    //
    // This specifies which of the possible GPIO pins will be SPI functional
    // pins.
    // Comment out other unwanted lines.
    //
    GPIO_SetupPinMux(60, GPIO_MUX_CPU1, 15);
    GPIO_SetupPinMux(59, GPIO_MUX_CPU1, 15);
    GPIO_SetupPinMux(58, GPIO_MUX_CPU1, 15);
//    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 1; // Configure GPIO16 as SPISIMOA
//  GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;  // Configure GPIO5 as SPISIMOA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 1; // Configure GPIO17 as SPISOMIA
//  GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;  // Configure GPIO3 as SPISOMIA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 1; // Configure GPIO18 as SPICLKA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

    GPIO_SetupPinOptions(61, GPIO_OUTPUT, GPIO_ASYNC);
    SPI_CS_HIGH;

    EDIS;
}

/*******************************************************************************
 * Function Name  : LCD_initSPI
 * Description    : Configures LCD Control lines
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
static void LCD_initSPI()
{
    /* P10.1 - CLK
     * P10.2 - MOSI
     * P10.3 - MISO
     * P10.4 - LCD CS 
     * P10.5 - TP CS 
    */

//    EUSCI_B3->CTLW0 = EUSCI_B_CTLW0_SWRST;
//
//    //3-pin SPI, Master mode, MSB first, select SMCLK, set to rising edge
//    EUSCI_B3->CTLW0 |= EUSCI_B_SPI_3PIN | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MSB | EUSCI_B_CTLW0_SSEL__SMCLK | EUSCI_B_CTLW0_CKPL;
//    EUSCI_B3->BRW = 4; //divide SMCLK/4
//
//    EUSCI_B3->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
//
//    P10->SEL0 |= 0x0F; //select UCB0CLK, UCB0SIMO
//    P10->SEL1 &= 0x0F;
//
//    P10->DIR |= BIT4 | BIT5;
//    P10->OUT |= BIT4 | BIT5;

    // Initialize SPI-A
    InitSpiaGpio();

    EALLOW;
    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 8-bit character
    // disable loop-back
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPICCR.bit.SPICHAR = (8-1);
    SpiaRegs.SPICCR.bit.SPILBK = 0;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = ((200E6 / 4) / 12000E3) - 1;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpiaRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;

    //
    // Initialize SPI FIFO registers
    //
    SpiaRegs.SPIFFTX.all = 0xE040;
    SpiaRegs.SPIFFRX.all = 0x2044;
    SpiaRegs.SPIFFCT.all = 0x0;

}

/*******************************************************************************
 * Function Name  : LCD_reset
 * Description    : Resets LCD
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : Uses P10.0 for reset
 *******************************************************************************/
static void LCD_reset()
{
    GPIO_SetupPinOptions(32, GPIO_OUTPUT, GPIO_ASYNC); //LCD_RESET
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;
    Delay(100);
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
    Delay(100);
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;
}

/************************************  Private Functions  *******************************************/


/************************************  Public Functions  *******************************************/

/*******************************************************************************
 * Function Name  : LCD_DrawRectangle
 * Description    : Draw a rectangle as the specified color
 * Input          : xStart, xEnd, yStart, yEnd, Color
 * Output         : None
 * Return         : None
 * Attention      : Must draw from left to right, top to bottom!
 *******************************************************************************/
void LCD_DrawRectangle(int16_t xStart, int16_t xEnd, int16_t yStart, int16_t yEnd, uint16_t Color)
{
    LCD_WriteReg(HOR_ADDR_START_POS, yStart);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, yEnd-1);  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, xStart);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, xEnd-1); /* Vertical GRAM Start Address */
    LCD_SetCursor(xStart, yStart);

    LCD_WriteIndex(DATA_IN_GRAM);   //Draws rectangle on screen
    SPI_CS_LOW;
    LCD_Write_Data_Start();

    uint32_t size = ((xEnd - xStart) + 1)*((yEnd - yStart) + 1);

    uint32_t i;
    for(i = 0;i < size;i++){
        LCD_Write_Data_Only(Color);
    }

    SPI_CS_HIGH;
}

/******************************************************************************
 * Function Name  : PutChar
 * Description    : Lcd screen displays a character
 * Input          : - Xpos: Horizontal coordinate
 *                  - Ypos: Vertical coordinate
 *                  - ASCI: Displayed character
 *                  - charColor: Character color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
inline void PutChar( uint16_t Xpos, uint16_t Ypos, char* ASCI, uint16_t charColor)
{
    uint16_t i, j;
    unsigned char buffer[16], tmp_char;
    GetASCIICode(buffer,*ASCI);  /* get font data */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* Character color */
            }
        }
    }
}

/******************************************************************************
 * Function Name  : GUI_Text
 * Description    : Displays the string
 * Input          : - Xpos: Horizontal coordinate
 *                  - Ypos: Vertical coordinate
 *                  - str: Displayed string
 *                  - charColor: Character color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Text(uint16_t Xpos, uint16_t Ypos, char *str, uint16_t Color)
{
    char TempChar;

    /* Set area back to span the entire LCD */
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
    do
    {
        TempChar = *str++;
        PutChar( Xpos, Ypos, &TempChar, Color);
        if( Xpos < MAX_SCREEN_X - 8)
        {
            Xpos += 8;
        }
        else if ( Ypos < MAX_SCREEN_X - 16)
        {
            Xpos = 0;
            Ypos += 16;
        }
        else
        {
            Xpos = 0;
            Ypos = 0;
        }
    }
    while ( *str != 0 );
}


/*******************************************************************************
 * Function Name  : LCD_Clear
 * Description    : Fill the screen as the specified color
 * Input          : - Color: Screen Color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    LCD_Send_Data(Color, SCREEN_SIZE);
}

/*******************************************************************************
 * Function Name  : LCD_Send_Data
 * Description    : sends data to GRAM max amount of times
 * Input          : - data: the data to send
 *                : - max: the number of times to send data
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Send_Data(uint16_t data, uint32_t max)
{
    LCD_WriteIndex(GRAM);

    SPI_CS_LOW;
    LCD_Write_Data_Start();
    uint32_t count;
    for(count = 0; count <= max; count++)
    {
        LCD_Write_Data_Only(data);
    }

    SPI_CS_HIGH;
}

/******************************************************************************
 * Function Name  : LCD_SetPoint
 * Description    : Drawn at a specified point coordinates
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate
 * Output         : None
 * Return         : None
 * Attention      : 18N Bytes Written
 *******************************************************************************/
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t color)
{
    LCD_SetCursor(Xpos, Ypos);
    LCD_WriteReg(GRAM, color);
}

/*******************************************************************************
 * Function Name  : LCD_Write_Data_Only
 * Description    : Data writing to the LCD controller
 * Input          : - data: data to be written
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Write_Data_Only(uint16_t data)
{
    SPISendRecvByte((data >>   8));  /* Write D8..D15                */
    SPISendRecvByte((data & 0xFF));  /* Write D0..D7                 */
}

/*******************************************************************************
 * Function Name  : LCD_WriteData
 * Description    : LCD write register data
 * Input          : - data: register data
 * Output         : None
 * Return         : None
 * Attention      : None
 ***************************************************FRAME_RATE_AND_COLOR_CONTROL****************************/
void LCD_WriteData(uint16_t data)
{
    SPI_CS_LOW;

    LCD_Write_Data_Start();
    LCD_Write_Data_Only(data);

    SPI_CS_HIGH;
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Reads the selected LCD Register.
 * Input          : None
 * Output         : None
 * Return         : LCD Register Value.
 * Attention      : None
 *******************************************************************************/
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WriteIndex(LCD_Reg);
    return LCD_ReadData();
}

/*******************************************************************************
 * Function Name  : LCD_WriteIndex
 * Description    : LCD write register address
 * Input          : - index: register address
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_WriteIndex(uint16_t index)
{
    SPI_CS_LOW;

    /* SPI write data */
    SPISendRecvByte(SPI_START | SPI_WR | SPI_INDEX);   /* Write : RS = 0, RW = 0  */
    SPISendRecvByte(0);
    SPISendRecvByte(index);

    SPI_CS_HIGH;
}

/*******************************************************************************
 * Function Name  : SPISendRecvByte
 * Description    : Send one byte then receive one byte of response
 * Input          : uint8_t: byte
 * Output         : None
 * Return         : Recieved value 
 * Attention      : None
 *******************************************************************************/
inline uint16_t SPISendRecvByte(uint16_t byte)
{
//    while(!(EUSCI_B3->IFG & EUSCI_B_IFG_TXIFG0));
//    EUSCI_B3->TXBUF = byte;
//
//    while(!(EUSCI_B3->IFG & EUSCI_B_IFG_RXIFG));
//
//    return EUSCI_B3->RXBUF;

    while(SpiaRegs.SPIFFTX.bit.TXFFST !=0);
    //
    // Transmit data
    //
    SpiaRegs.SPITXBUF = byte << 8;

    //
    // Wait until data is received
    //
    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1);

    return SpiaRegs.SPIRXBUF;
}

/*******************************************************************************
 * Function Name  : LCD_Write_Data_Start
 * Description    : Start of data writing to the LCD controller
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Write_Data_Start(void)
{
    SPISendRecvByte(SPI_START | SPI_WR | SPI_DATA);    /* Write : RS = 1, RW = 0 */
}

/*******************************************************************************
 * Function Name  : LCD_ReadData
 * Description    : LCD read data
 * Input          : None
 * Output         : None
 * Return         : return data
 * Attention      : Diagram (d) in datasheet
 *******************************************************************************/
inline uint16_t LCD_ReadData()
{
    uint16_t value;
    SPI_CS_LOW;

    SPISendRecvByte(SPI_START | SPI_RD | SPI_DATA);   /* Read: RS = 1, RW = 1   */
    SPISendRecvByte(0);                               /* Dummy read 1           */
    value = (SPISendRecvByte(0) << 8);                /* Read D8..D15           */
    value |= SPISendRecvByte(0);                      /* Read D0..D7            */

    SPI_CS_HIGH;
    return value;
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Writes to the selected LCD register.
 * Input          : - LCD_Reg: address of the selected register.
 *                  - LCD_RegValue: value to write to the selected register.
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WriteIndex(LCD_Reg);
    LCD_WriteData(LCD_RegValue);
}

/*******************************************************************************
 * Function Name  : LCD_SetCursor
 * Description    : Sets the cursor position.
 * Input          : - Xpos: specifies the X position.
 *                  - Ypos: specifies the Y position.
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos )
{
    LCD_WriteReg(GRAM_VERTICAL_ADDRESS_SET, Xpos);
    LCD_WriteReg(GRAM_HORIZONTAL_ADDRESS_SET, Ypos);
}

/*******************************************************************************
 * Function Name  : LCD_Init
 * Description    : Configures LCD Control lines, sets whole screen black
 * Input          : bool usingTP: determines whether or not to enable TP interrupt 
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Init(bool usingTP)
{
    LCD_initSPI();

//    if (usingTP)
//    {
//        P4->DIR &= ~BIT0; //set as input
//        P4->IFG &= ~BIT0; //clear interrupt flag
//        P4->IE  |= BIT0; //enable interrupt
//        P4->IES |= BIT0; //falling edge trigger
//        P4->REN |= BIT0; // add pull-up resistor
//        P4->OUT |= BIT0; //output a 1
//    }

    LCD_reset();

    LCD_WriteReg(0xE5, 0x78F0); /* set SRAM internal timing */
    LCD_WriteReg(DRIVER_OUTPUT_CONTROL, 0x0100); /* set Driver Output Control */
    LCD_WriteReg(DRIVING_WAVE_CONTROL, 0x0700); /* set 1 line inversion */
    LCD_WriteReg(ENTRY_MODE, 0x1038); /* set GRAM write direction and BGR=1 */
    LCD_WriteReg(RESIZING_CONTROL, 0x0000); /* Resize register */
    LCD_WriteReg(DISPLAY_CONTROL_2, 0x0207); /* set the back porch and front porch */
    LCD_WriteReg(DISPLAY_CONTROL_3, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(DISPLAY_CONTROL_4, 0x0000); /* FMARK function */
    LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_1, 0x0000); /* RGB interface setting */
    LCD_WriteReg(FRAME_MARKER_POSITION, 0x0000); /* Frame marker Position */
    LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_2, 0x0000); /* RGB interface polarity */

    /* Power On sequence */
    LCD_WriteReg(POWER_CONTROL_1, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(POWER_CONTROL_2, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(POWER_CONTROL_3, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(POWER_CONTROL_4, 0x0000); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(DISPLAY_CONTROL_1, 0x0001);
    Delay(200);

    /* Dis-charge capacitor power voltage */
    LCD_WriteReg(POWER_CONTROL_1, 0x1090); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(POWER_CONTROL_2, 0x0227); /* Set DC1[2:0], DC0[2:0], VC[2:0] */
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(POWER_CONTROL_3, 0x001F);
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(POWER_CONTROL_4, 0x1500); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(POWER_CONTROL_7, 0x0027); /* 04 VCM[5:0] for VCOMH */
    LCD_WriteReg(FRAME_RATE_AND_COLOR_CONTROL, 0x000D); /* Set Frame Rate */
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(GRAM_HORIZONTAL_ADDRESS_SET, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(GRAM_VERTICAL_ADDRESS_SET, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve */
    LCD_WriteReg(GAMMA_CONTROL_1,    0x0000);
    LCD_WriteReg(GAMMA_CONTROL_2,    0x0707);
    LCD_WriteReg(GAMMA_CONTROL_3,    0x0307);
    LCD_WriteReg(GAMMA_CONTROL_4,    0x0200);
    LCD_WriteReg(GAMMA_CONTROL_5,    0x0008);
    LCD_WriteReg(GAMMA_CONTROL_6,    0x0004);
    LCD_WriteReg(GAMMA_CONTROL_7,    0x0000);
    LCD_WriteReg(GAMMA_CONTROL_8,    0x0707);
    LCD_WriteReg(GAMMA_CONTROL_9,    0x0002);
    LCD_WriteReg(GAMMA_CONTROL_10,   0x1D04);

    /* Set GRAM area */
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X60, 0x2700); /* Gate Scan Line */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X61, 0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X6A, 0x0000); /* set scrolling line */

    /* Partial Display Control */
    LCD_WriteReg(PART_IMAGE_1_DISPLAY_POS, 0x0000);
    LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x81, 0x0000);
    LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x82, 0x0000);
    LCD_WriteReg(PART_IMAGE_2_DISPLAY_POS, 0x0000);
    LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x84, 0x0000);
    LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x85, 0x0000);

    /* Panel Control */
    LCD_WriteReg(PANEL_ITERFACE_CONTROL_1, 0x0010);
    LCD_WriteReg(PANEL_ITERFACE_CONTROL_2, 0x0600);
    LCD_WriteReg(DISPLAY_CONTROL_1, 0x0133); /* 262K color and display ON */
    Delay(50); /* delay 50 ms */

    LCD_Clear(LCD_BLACK);
}

/*******************************************************************************
 * Function Name  : TP_ReadXY
 * Description    : Obtain X and Y touch coordinates
 * Input          : None
 * Output         : None
 * Return         : Pointer to "Point" structure
 * Attention      : None
 *******************************************************************************/
//Point TP_ReadXY()
//{
//    Point dataPoint;
//    dataPoint.x = TP_ReadY();
//    dataPoint.y = TP_ReadX();
//
//    return dataPoint;
//}
//
//uint16_t TP_Read(uint8_t data)
//{
//    SPI_CS_TP_LOW;
//    SPISendRecvByte(data);
//    uint16_t value = (SPISendRecvByte(0) << 8);
//    value |= SPISendRecvByte(0);
//    SPI_CS_TP_HIGH;
//    return value >> 3;
//}

/*******************************************************************************
 * Function Name  : TP_ReadX
 * Description    : Obtain X touch coordinate
 * Input          : None
 * Output         : None
 * Return         : X Coordinate
 * Attention      : None
 *******************************************************************************/
//uint16_t TP_ReadX()
//{
//    return (TP_Read(CHX)*MAX_SCREEN_X)/4096;
//}
/*******************************************************************************
 * Function Name  : TP_ReadY
 * Description    : Obtain Y touch coordinate
 * Input          : None
 * Output         : None
 * Return         : Y Coordinate
 * Attention      : None
 *******************************************************************************/
//uint16_t TP_ReadY()
//{
//    return (TP_Read(CHY)*MAX_SCREEN_Y)/4096;
//}
/************************************  Public Functions  *******************************************/

