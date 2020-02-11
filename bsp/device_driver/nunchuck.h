



struct nunchuck_t
{
    uint16_t joy_x;
    uint16_t joy_y;
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
    bool     button_c;
    bool     button_z;
};

void nunchuck_init();
struct nunchuck_t nunchuck_read();
void nunchuck_send_read();
void nunchuck_refresh();
void nunchuck_init();
