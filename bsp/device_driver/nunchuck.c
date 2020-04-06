
#include "../bsp.h"     // Device Headerfile and Examples Include File
#include "../../libs/printNum.h"

//
// Local variables
//
struct I2CMSG I2cInit={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR, //Writing Address
                           2,
                           0xF0,                   // Msg Byte 1
                           0x55};

struct I2CMSG I2cReq={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR, //Writing Address
                           1,
                           0x00};

struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          6};


//
// Public functions
//

void nunchuck_init()
{
    while (I2CA_WriteData(&I2cInit) != I2C_SUCCESS);
    while (I2CA_WriteData(&I2cReq) != I2C_SUCCESS);
}

void nunchuck_refresh()
{
    while (I2CA_WriteData(&I2cReq) != I2C_SUCCESS);
}

void nunchuck_send_read()
{
    while (I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS);
}

nunchuck_t nunchuck_read()
{
    uint16_t data[6];
    int i;
    for(i=0; i < 6; i++)
        data[i] = (I2caRegs.I2CDRR.all ^ 0x17) + 0x17;

    nunchuck_t response;
    response.joy_x = data[0];
    response.joy_y = data[1];
    response.accel_x = (data[2] << 2) | ((data[5] >> 2) & 0x3);
    response.accel_y = (data[3] << 2) | ((data[5] >> 4) & 0x3);
    response.accel_z = (data[4] << 2) | ((data[5] >> 6) & 0x3);
    int zPressed = data[5] & 0x01;
    int cPressed = (data[5] >> 1) & 0x01;
    response.button_c = cPressed || !zPressed;
    response.button_z = zPressed;
    return response;
}

void nunchuck_print(nunchuck_t* data)
{
    scia_msg("\n\rJoy X: ");
    printNum(data->joy_x);
    scia_msg("\n\rJoy Y: ");
    printNum(data->joy_y);

    scia_msg("\n\rZ-Button: ");
    printNum(data->button_z);
    scia_msg("\n\rC-Button: ");
    printNum(data->button_c);

    scia_msg("\n\rx-accel: ");
    printNum(data->accel_x);
    scia_msg("\n\ry-accel: ");
    printNum(data->accel_y);
    scia_msg("\n\rz-accel: ");
    printNum(data->accel_z);
    scia_msg("\n");
}

