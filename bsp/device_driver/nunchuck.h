
#ifndef BSP_DEVICE_DRIVER_Nunchuck
#define BSP_DEVICE_DRIVER_Nunchuck

struct nunchuck
{
    uint16_t joy_x;
    uint16_t joy_y;
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
    bool     button_c;
    bool     button_z;
};


typedef struct nunchuck nunchuck_t;

void nunchuck_init();
nunchuck_t nunchuck_read();
void nunchuck_send_read();
void nunchuck_refresh();
void nunchuck_init();
void nunchuck_print(nunchuck_t* data);

#endif /* BSP_DEVICE_DRIVER_Nunchuck */
