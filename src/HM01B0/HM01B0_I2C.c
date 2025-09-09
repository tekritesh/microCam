#include "HM01B0_I2C.h"

void init_cam(void)
{


    uint8_t id_high = 0, id_low = 0;
    uint8_t reg_addr_high[2] = {0x00, 0x00};   // MODEL_ID_H address
    uint8_t reg_addr_low[2]  = {0x00, 0x01};   // MODEL_ID_L address
    /*
    // Read MODEL_ID_H
    int ret1 = i2c_write_read_dt(&dev_i2c, reg_addr_high, 2, &id_high, 1);
    // Read MODEL_ID_L
    int ret2 = i2c_write_read_dt(&dev_i2c, reg_addr_low, 2, &id_low, 1);
    if (ret1 != 0 || ret2 != 0) {
        printk("Failed to read chip ID! 0x%02x%02x\n", id_high, id_low);
        return;
    }
    printk("HM01B0 ID: 0x%02x%02x\n", id_high, id_low);
    */
    
    hm_i2c_write( REG_MODE_SELECT, 0x00);//go to stand by mode
    //hm_i2c_write( REG_ANA_REGISTER_17, 0x00);//register to change the clk source(osc:1 mclk:0), if no mclk it goes to osc by default
    hm_i2c_write( REG_TEST_PATTERN_MODE, 0x00);//Enable the test pattern, set it to walking 1
    //hm_i2c_write(0x0101 , 0x03);
    
    hm_i2c_write( REG_BIN_MODE, 0x03);//VERTICAL BIN MODE
    hm_i2c_write( REG_QVGA_WIN_EN, 0x01);//Set line length LSB to QQVGA => enabled: makes the image 160(row)*240(col)
//    disable: image 160*320 //In test pattern mode, enabling this does not have any effect

    /*looking at lattice cfg setting*/
    //hm_i2c_write(0x0103,0x00);

    //100*100 optimization
    hm_i2c_write( REG_BIN_RDOUT_X, 0x03);//Horizontal Binning enable
    hm_i2c_write( REG_BIN_RDOUT_Y, 0x03);//vertical Binning enable => this register should be always 0x03 because we never go more than 160 for the height
        //frame timing control
    // Frame timing control: defaults from datasheet 9.4
    hm_i2c_write(0x0342, 0x00);   // LINE_LENGTH_PCK_H
    hm_i2c_write(0x0343, 0xD7);   // LINE_LENGTH_PCK_L (0x0172)
    hm_i2c_write(0x0340, 0x00);   // FRAME_LENGTH_LINES_H
    hm_i2c_write(0x0341, 0x80);   // FRAME_LENGTH_LINES_L (0x0232)

    /*looking at lattice cfg setting*/
    //hm_i2c_write(0x0103,0x00);
    hm_i2c_write(0x0104 ,0x01);
    
    hm_i2c_write(0x3044,0x0A);
    hm_i2c_write(0x3045,0x00);
    hm_i2c_write(0x3047,0x0A);
    hm_i2c_write(0x3050,0xC0);
    hm_i2c_write(0x3051,0x42);
//    hm_i2c_write(0x3052,0x50);
    hm_i2c_write(0x3053,0x00);
    hm_i2c_write(0x3054,0x03);
    hm_i2c_write(0x3055,0xF7);
    hm_i2c_write(0x3056,0xF8);
    hm_i2c_write(0x3057,0x29);
    hm_i2c_write(0x3058,0x1F);
//    hm_i2c_write(0x3059,0x1E);//bit control
    hm_i2c_write(0x3064,0x00);
    hm_i2c_write(0x3065,0x04);

    //black level control
    hm_i2c_write(0x1000,0x43);
    hm_i2c_write(0x1001,0x40);
    hm_i2c_write(0x1002,0x32);
    hm_i2c_write(0x1003,0x08);//default from lattice 0x08
    hm_i2c_write(0x1006,0x01);
    hm_i2c_write(0x1007,0x08);//default from lattice 0x08

    hm_i2c_write(0x0350,0x7F);
    

    //Sensor reserved
    hm_i2c_write(0x1008,0x00);
    hm_i2c_write(0x1009,0xA0);
    hm_i2c_write(0x100A,0x60);
    hm_i2c_write(0x100B,0x90);//default from lattice 0x90
    hm_i2c_write(0x100C,0x40);//default from lattice 0x40;

    //Vsync, hsync and pixel shift register
//    hm_i2c_write(0x1012,0x07);//changed by Ali
    hm_i2c_write(0x1012,0x00);//lattice value

    //Statistic control and read only
    hm_i2c_write(0x2000,0x07);
    hm_i2c_write(0x2003,0x00);
    hm_i2c_write(0x2004,0x1C);
    hm_i2c_write(0x2007,0x00);
    hm_i2c_write(0x2008,0x58);
    hm_i2c_write(0x200B,0x00);
    hm_i2c_write(0x200C,0x7A);
    hm_i2c_write(0x200F,0x00);
    hm_i2c_write(0x2010,0xB8);
    hm_i2c_write(0x2013,0x00);
    hm_i2c_write(0x2014,0x58);
    hm_i2c_write(0x2017,0x00);
    hm_i2c_write(0x2018,0x9B);

    // --- AE: darker target, no gain, shorter max exposure ---
    hm_i2c_write(0x2100, 0x00);   // AE off while updating

    hm_i2c_write(0x2101, 0x22);   // AE_TARGET_MEAN (lower than 0x22 → darker, improves text)
    hm_i2c_write(0x2105, 0x00);   // MAX_INTG = 0x0080 (shorter ceiling)
    hm_i2c_write(0x2106, 0x60);

    hm_i2c_write(0x2108, 0x00);   // MAX_AGAIN_FULL  = 1×
    hm_i2c_write(0x2109, 0x00);   // MAX_AGAIN_BIN2  = 1×
    hm_i2c_write(0x210A, 0x00);   // MIN_AGAIN       = 1×
    hm_i2c_write(0x210B, 0x40);   // MAX_DGAIN       = 1×
    hm_i2c_write(0x210C, 0x40);   // MIN_DGAIN       = 1×
    hm_i2c_write(0x210D, 0x30);   // DAMPING (keep stable)
    hm_i2c_write(0x210E, 0x03);   // Anti‑flicker 60 Hz (as before)

    hm_i2c_write(0x0202, 0x00);   // Seed a darker starting exposure
    hm_i2c_write(0x0203, 0x80);   // INTEGRATION = 0x0080
    hm_i2c_write(0x0205, 0x00);   // Analog gain 1×
    hm_i2c_write(0x020E, 0x01);   // Digital 1×
    hm_i2c_write(0x020F, 0x00);

    hm_i2c_write(0x2100, 0x01);   // AE on

    // Keep motion detection control as before
    hm_i2c_write(0x2150, 0x03);

    //Sensor exposure gain
    // Sensor exposure/gain (manual baseline): start sane before AEG takes over
    // Coarse integration in lines (datasheet 9.3). Start ~50% of default frame length (0x0232).
    hm_i2c_write(0x0202, 0x01);   // INTEGRATION_H
    hm_i2c_write(0x0203, 0x20);   // INTEGRATION_L -> 0x0120 = 288 lines
    // Modest analog gain, 2x; digital gain 1x
    hm_i2c_write(0x0205, 0x00);   // ANALOG GAIN (1x)
    hm_i2c_write(0x020E, 0x01);   // DIGITAL_GAIN_H
    hm_i2c_write(0x020F, 0x00);   // DIGITAL_GAIN_L
    //hm_i2c_write(0x2100,0x01);
    //frame timing control
//    hm_i2c_write(0x0340,0x02);//changed by Ali
//    hm_i2c_write(0x0341,0x32);//changed by Ali    
////    hm_i2c_write(0x0340,0x0C);
////    hm_i2c_write(0x0341,0x5C);
//
//    hm_i2c_write(0x0342,0x01);
//    hm_i2c_write(0x0343,0x78);//changed by Ali
//    hm_i2c_write(0x0343,0x78);

//    hm_i2c_write(0x3010,0x01); //done in lower lines
//    hm_i2c_write(0x0383,0x00); //done in lower lines
//    hm_i2c_write(0x0387,0x00); //done in lower lines
//    hm_i2c_write(0x0390,0x00); //done in lower lines
//    hm_i2c_write(0x3059,0x42); //done in lower lines
//    hm_i2c_write(0x3060,0x51); //done in lower lines
        //hm_i2c_write( REG_OSC_CLK_DIV, 0x30);//This is effective when we use external clk, Use the camera in the gated clock mode to make the clock zero when there is no data

        hm_i2c_write( REG_BIT_CONTROL, 0x20);//8-bit grayscale (1 byte per pixel)
        //hm_i2c_write(0x3062, 0xFF);
        //hm_i2c_write(0x0202, 0x00);
        //hm_i2c_write(0x0203, 0x08);
        //hm_i2c_write(0x2101, 0x06);
        //hm_i2c_write(0x2105, 0x01);
        hm_i2c_write(0x3060, 0x30);
        hm_i2c_write(0x1012, 0x03);
        //hm_i2c_write(0x3023, 0x05);
        //hm_i2c_write(0x0101 , 0x02);

        //hm_i2c_write( REG_PMU_PROGRAMMABLE_FRAMECNT, 0x01);//set the number of frames to be sent out, it sends N frames
        //hm_clk_enable(true);
}

void hm_i2c_write(uint16_t reg, uint8_t val)
{
uint8_t buf[] = { reg >> 8, reg & 0xff, val };
i2c_write_dt(&dev_i2c, buf, sizeof buf);
}
