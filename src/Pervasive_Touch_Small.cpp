//
// Pervasive_Touch_Small.cpp
// Class library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
// Based on highView technology
//
// Created by Rei Vilo, 21 Nov 2024
//
// Copyright (c) Rei Vilo, 2010-2025
// Licence All rights reserved
//
// See Pervasive_Touch_Small.h for references
//
// Release 526: Improved touch management
// Release 530: Added support for new 3.70"-Touch
// Release 531: Added getBoardPins
// Release 533: Improved touch release
// Release 540: Added touch sleep mode, /RESET to resume
// Release 601: Added support for screens with embedded fast update
// Release 602: Improve functions structure
// Release 603: Improved numbering consistency
// Release 603: Simplified partial update
// Release 604: Improved optional external power management
// Release 604: Improved speed for screens with embedded fast update
// Release 604: Added continuity mode for faster GUI
// Release 606: Implemented continuity for GUI
// Release 607: Refactored peripherals
// Release 607: Improved screens names consistency
// Release 608: Added screen report
// Release 608: Added support for screens with wide temperature
// Release 609: Added temperature management
// Release 609: Deprecated partial update
// Release 610: Removed partial update
// Release 610: Added continuity mode
// Release 611: Added check on fast screens
// Release 613: Added x- and y-axis margins
// Release 700: Refactored screen and board functions
// Release 702: Added continuity macro
// Release 800: Read OTP memory
// Release 801: Improved OTP implementation
// Release 802: Added references to application notes
// Release 802: Refactored CoG functions
// Release 803: Added types for string and frame-buffer
// Release 804: Improved power management
// Release 806: Added Wire debugging
// Release 900: Added new driver library
// Release 902: Simplified touch options
// Release 909: Added I2C device availability check
// Release 909: Improved stability for 3.70 touch
//

// Header
#include "Pervasive_Touch_Small.h"

//
// === Touch section
//
// List of touch ICs
#define TOUCH_271_ADDRESS 0x41
#define TOUCH_370_ADDRESS 0x38
// #define TOUCH_343_ADDRESS 0x4A
//
// === End of Touch section
//

//
// === COG section
//
/// @cond NOT_PUBLIC
/// @see
/// * ApplicationNote_Small_Size_wide-Temperature_EPD_v03_20231031_B
/// * ApplicationNote_Small_Size_wide-Temperature_EPD_v01_20231225_A
/// * ApplicationNote_152_Size_wide-Temperature_EPD_v01_20231225_A
/// * ApplicationNote_DualChip_wideTemperature_EPD_v01_20230804
/// * ApplicationNote_SingleChip_wideTemperature_EPD_v01_20230720
//

//
// --- Small screens with Q film
//
void Pervasive_Touch_Small::COG_reset()
{
    // Application note § 2. Power on COG driver
    b_reset(5, 5, 10, 5, 5); // Small

    // // Check after reset
    // switch (u_eScreen_EPD)
    // {
    //     case eScreen_EPD_150_KS_0J:
    //     case eScreen_EPD_152_KS_0J:
    //
    //         if (hV_HAL_GPIO_get(b_pin.panelBusy) == HIGH)
    //         {
    //             hV_HAL_Serial_crlf();
    //             hV_HAL_log(LEVEL_CRITICAL, "Incorrect type for 1.52-Wide");
    //             hV_HAL_exit(0x01);
    //         }
    //         break;
    //
    //     default:
    //
    //         break;
    // }
}

void Pervasive_Touch_Small::COG_getDataOTP()
{
    hV_HAL_SPI_end(); // With unicity check
    hV_HAL_SPI3_begin(); // Define 3-wire SPI pins

    // Read OTP
    uint8_t ui8 = 0;
    uint16_t _readBytes = 0;
    u_flagOTP = false;

    // Application note § 3. Read OTP memory
    // Register 0x50 flag
    // Additional settings for fast update, 154 206 213 266 271A 370 and 437 screens (s_flag50)
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_154_KS_0C:
        // case eScreen_EPD_206_KS_0E:
        // case eScreen_EPD_213_KS_0E:
        // case eScreen_EPD_266_KS_0C:
        // case eScreen_EPD_271_KS_0C: // 2.71(A)
        // case eScreen_EPD_370_KS_0C:
        // case eScreen_EPD_437_KS_0C:
        //
        //     s_flag50 = true;
        //     break;

        default:

            s_flag50 = false;
            break;
    }

    // // Screens with no OTP
    // switch (u_eScreen_EPD)
    // {
    //    case eScreen_EPD_150_KS_0J:
    //    case eScreen_EPD_152_KS_0J:
    //    case eScreen_EPD_290_KS_0F:
    //
    //        u_flagOTP = true;
    //        hV_HAL_log(LEVEL_INFO, "OTP check passed - embedded PSR");
    //        return; // No PSR
    //        break;
    //
    //    default:
    //
    //        break;
    // }

    // GPIO
    // COG_reset(); // Although not mentioned, reset to ensure stable state

    // Read OTP
    _readBytes = 2;
    ui8 = 0;

    uint16_t offsetA5 = 0x0000;
    uint16_t offsetPSR = 0x0000;

    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
    hV_HAL_SPI3_write(0xa2);
    hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
    hV_HAL_delayMilliseconds(5);

    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
    ui8 = hV_HAL_SPI3_read(); // Dummy
    hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
    // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

    hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
    ui8 = hV_HAL_SPI3_read(); // First byte to be checked
    hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
    // hV_HAL_log(LEVEL_INFO, "ui8= 0x%02x", ui8);

    // Check bank
    uint8_t bank = ((ui8 == 0xa5) ? 0 : 1);

    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_271_KS_09:
        case eScreen_EPD_271_KS_09_Touch:
        case eScreen_EPD_271_PS_09_Touch:

            offsetPSR = 0x004b;
            offsetA5 = 0x0000;

            if (bank > 0)
            {
                COG_data[0] = 0xcf;
                COG_data[1] = 0x82;
                return;
            }
            break;

        // case eScreen_EPD_271_PS_09:
        // // case eScreen_EPD_271_KS_09_Touch:
        // case eScreen_EPD_271_PS_09_Touch:
        // case eScreen_EPD_287_PS_09:
        //
        //     offsetPSR = 0x004b;
        //     offsetA5 = 0x0000;
        //
        //     break;

        // case eScreen_EPD_154_KS_0C:
        // case eScreen_EPD_266_KS_0C:
        // case eScreen_EPD_271_KS_0C: // 2.71(A)
        // // case eScreen_EPD_350_KS_0C:
        case eScreen_EPD_370_PS_0C_Touch:
        case eScreen_EPD_370_KS_0C_Touch:

            offsetPSR = (bank == 0) ? 0x0fb4 : 0x1fb4;
            offsetA5 = (bank == 0) ? 0x0000 : 0x1000;
            break;

        // case eScreen_EPD_206_KS_0E:
        // case eScreen_EPD_213_KS_0E:
        //
        //     offsetPSR = (bank == 0) ? 0x0b1b : 0x171b;
        //     offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
        //     break;
        //
        // case eScreen_EPD_417_KS_0D:
        //
        //     offsetPSR = (bank == 0) ? 0x0b1f : 0x171f;
        //     offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
        //     break;

        default:

            hV_HAL_Serial_crlf();
            hV_HAL_log(LEVEL_CRITICAL, "OTP check failed - Screen %i-%cS-0%c not supported", SCREEN_SIZE(u_eScreen_EPD), SCREEN_FILM(u_eScreen_EPD), SCREEN_DRIVER(u_eScreen_EPD));
            hV_HAL_exit(0x01);
            break;
    }

    // Check second bank
    if (offsetA5 > 0x0000)
    {
        for (uint16_t index = 1; index < offsetA5; index += 1)
        {
            hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
            ui8 = hV_HAL_SPI3_read();
            hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
        }

        hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
        ui8 = hV_HAL_SPI3_read(); // First byte to be checked
        hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect

        if (ui8 != 0xa5)
        {
            hV_HAL_Serial_crlf();
            hV_HAL_log(LEVEL_CRITICAL, "OTP check failed - Bank %i, first 0x%02x, expected 0x%02x", bank, ui8, 0xa5);
            hV_HAL_exit(0x01);
        }
    }

    switch (u_eScreen_EPD)
    {
        case eScreen_EPD_271_KS_09_Touch:

            hV_HAL_log(LEVEL_INFO, "OTP check passed - Bank %i, first 0x%02x %s", bank, ui8, (bank == 0) ? "as expected" : "not checked");
            break;

        default:

            hV_HAL_log(LEVEL_INFO, "OTP check passed - Bank %i, first 0x%02x as expected", bank, ui8);
            break;
    }

    // Ignore bytes 1..offsetPSR
    for (uint16_t index = offsetA5 + 1; index < offsetPSR; index += 1)
    {
        hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
        ui8 = hV_HAL_SPI3_read();
        hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
    }

    // Populate COG_data
    for (uint16_t index = 0; index < _readBytes; index += 1)
    {
        hV_HAL_GPIO_clear(b_pin.panelCS); // CS low = Select
        ui8 = hV_HAL_SPI3_read(); // Read OTP
        COG_data[index] = ui8;
        hV_HAL_GPIO_set(b_pin.panelCS); // CS high = Unselect
    }

    u_flagOTP = true;

#if (DEBUG_OTP == 1) // Debug COG_data
    debugOTP(COG_data, _readBytes, COG_WIDE_SMALL, SCREEN_DRIVER(u_eScreen_EPD));
#endif // DEBUG_OTP

	hV_HAL_SPI3_end();
}

void Pervasive_Touch_Small::COG_initial(uint8_t updateMode)
{
    // Application note § 4. Input initial command
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_150_KS_0J:
        // case eScreen_EPD_152_KS_0J:
        //
        // // Soft reset
        //    b_sendCommand8(0x12);
        //    hV_HAL_GPIO_clear(b_pin.panelDC);
        //    b_waitBusy(LOW); // 150 and 152 specific
        //
        //    // Work settings
        //    b_sendCommandData8(0x1a, u_temperature);
        //
        //    if (updateMode == UPDATE_NORMAL)
        //    {
        //        b_sendCommandData8(0x22, 0xd7);
        //    }
        //    else if (updateMode == UPDATE_FAST)
        //    {
        //        b_sendCommandData8(0x3c, 0xc0);
        //        b_sendCommandData8(0x22, 0xdf);
        //    }
        //    break;

        default:

            // Work settings
            uint8_t indexTemperature; // Temperature
            uint8_t index00_work[2]; // PSR

            // FILM_K already checked
            if (updateMode != UPDATE_NORMAL) // Specific settings for fast update
            {
                indexTemperature = u_temperature | 0x40; // temperature | 0x40
                index00_work[0] = COG_data[0] | 0x10; // PSR0 | 0x10
                index00_work[1] = COG_data[1] | 0x02; // PSR1 | 0x02
            }
            else // Common settings
            {
                indexTemperature = u_temperature; // Temperature
                index00_work[0] = COG_data[0]; // PSR0
                index00_work[1] = COG_data[1]; // PSR1
            } // u_codeExtra updateMode

            // New algorithm
            b_sendCommandData8(0x00, 0x0e); // Soft-reset
            b_waitBusy();

            b_sendCommandData8(0xe5, indexTemperature); // Input Temperature
            b_sendCommandData8(0xe0, 0x02); // Activate Temperature

            switch (u_eScreen_EPD)
            {
                // case eScreen_EPD_290_KS_0F: // No PSR
                //
                //     b_sendCommandData8(0x4d, 0x55);
                //     b_sendCommandData8(0xe9, 0x02);
                //     break;

                default:

                    b_sendIndexData(0x00, index00_work, 2); // PSR
                    break;
            }

            // Specific settings for fast update, all screens
            // FILM_K already checked
            if (updateMode != UPDATE_NORMAL)
            {
                b_sendCommandData8(0x50, 0x07); // Vcom and data interval setting
            }
            break;
    }
}

void Pervasive_Touch_Small::COG_sendImageDataFast(FRAMEBUFFER_CONST_TYPE frame1, FRAMEBUFFER_CONST_TYPE frame2, uint32_t sizeFrame) // First frame, blackBuffer
{
    // Application note § 5. Input image to the EPD
    FRAMEBUFFER_CONST_TYPE nextBuffer = frame1;
    FRAMEBUFFER_CONST_TYPE previousBuffer = frame2;

    // Send image data
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_150_KS_0J:
        // case eScreen_EPD_152_KS_0J:
        //
        //     b_sendIndexData(0x24, previousBuffer, sizeFrame); // Next frame, blackBuffer
        //     b_sendIndexData(0x26, nextBuffer, sizeFrame); // Previous frame, 0x00
        //     break;

        default:

            // Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
            if (s_flag50)
            {
                b_sendCommandData8(0x50, 0x27); // Vcom and data interval setting
            }

            b_sendIndexData(0x10, previousBuffer, sizeFrame); // First frame, blackBuffer
            b_sendIndexData(0x13, nextBuffer, sizeFrame); // Second frame, 0x00

            // Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
            if (s_flag50)
            {
                b_sendCommandData8(0x50, 0x07); // Vcom and data interval setting
            }
            break;
    } // u_eScreen_EPD
}

void Pervasive_Touch_Small::COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame) // First frame, blackBuffer
{
    // Application note § 5. Input image to the EPD

    // Send image data
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_150_KS_0J:
        // case eScreen_EPD_152_KS_0J:
        //
        //     b_sendIndexData(0x24, frame, sizeFrame); // Next frame, blackBuffer
        //     b_sendIndexFixed(0x26, 0x00, sizeFrame); // Previous frame, 0x00
        //     break;

        default:

            b_sendIndexData(0x10, frame, sizeFrame); // First frame, blackBuffer
            b_sendIndexFixed(0x13, 0x00, sizeFrame); // Second frame, 0x00
            break;
    } // u_eScreen_EPD
}

void Pervasive_Touch_Small::COG_update()
{
    // Application note § 6. Send updating command
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_150_KS_0J:
        // case eScreen_EPD_152_KS_0J:
        //
        //     b_waitBusy(LOW); // 152 specific
        //     b_sendCommand8(0x20); // Display Refresh
        //     hV_HAL_GPIO_set(b_pin.panelCS); // CS# = 1
        //     b_waitBusy(LOW); // 152 specific
        //     break;

        default:

            b_waitBusy();
            b_sendCommand8(0x04); // Power on
            b_waitBusy();
            b_sendCommand8(0x12); // Display Refresh
            b_waitBusy();
            break;
    }
}

void Pervasive_Touch_Small::COG_stopDCDC()
{
    // Application note § 7. Turn-off DC/DC
    switch (u_eScreen_EPD)
    {
        // case eScreen_EPD_150_KS_0J:
        // case eScreen_EPD_152_KS_0J:
        //
        //     break;

        default:

            b_sendCommand8(0x02); // Turn off DC/DC
            b_waitBusy();
            break;
    }
}
//
// --- End of Small screens with Q film
//
/// @endcond
//
// === End of COG section
//

Pervasive_Touch_Small::Pervasive_Touch_Small(eScreen_EPD_t eScreen_EPD, pins_t board)
{
    d_COG = COG_TOUCH_SMALL;
    u_eScreen_EPD = eScreen_EPD;
    b_pin = board;
}

void Pervasive_Touch_Small::begin()
{
    b_begin(b_pin, FAMILY_SMALL, b_delayCS);
    b_resume(); // GPIO

    COG_reset(); // Reset
    COG_getDataOTP(); // 3-wire SPI read OTP memory

    d_beginTouch();

    // Check I2C device availability
    uint8_t bufferWrite[1] = {0};
    if (hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, nullptr, 0) == RESULT_ERROR)
    {
        hV_HAL_Serial_crlf();
        hV_HAL_log(LEVEL_CRITICAL, "Touch controller (0x%02x) not found", d_touchAddress);
        hV_HAL_exit(0x01);
    }
}

STRING_CONST_TYPE Pervasive_Touch_Small::reference()
{
    return formatString("%s v%i.%i.%i", DRIVER_EPD_VARIANT, DRIVER_EPD_RELEASE / 100, (DRIVER_EPD_RELEASE / 10) % 10, DRIVER_EPD_RELEASE % 10);
}

void Pervasive_Touch_Small::updateNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame)
{
    b_resume(); // GPIO
    COG_reset(); // Reset

    if (u_flagOTP == false)
    {
        COG_getDataOTP(); // 3-wire SPI read OTP memory
        COG_reset(); // Reset
    }

    // Start SPI
    hV_HAL_SPI_begin(16000000); // Fast 16 MHz, with unicity check

    COG_initial(UPDATE_NORMAL); // Initialise
    COG_sendImageDataNormal(frame, sizeFrame);

    COG_update(); // Update
    COG_stopDCDC(); // Power off
}

void Pervasive_Touch_Small::updateFast(FRAMEBUFFER_CONST_TYPE frame1,
                                       FRAMEBUFFER_CONST_TYPE frame2, uint32_t sizeFrame)
{
    b_resume(); // GPIO
    COG_reset(); // Reset

    if (u_flagOTP == false)
    {
        COG_getDataOTP(); // 3-wire SPI read OTP memory
        COG_reset(); // Reset
    }

    // Start SPI
    hV_HAL_SPI_begin(16000000); // Fast 16 MHz, with unicity check

    COG_initial(UPDATE_FAST); // Initialise
    COG_sendImageDataFast(frame1, frame2, sizeFrame);

    COG_update(); // Update
    COG_stopDCDC(); // Power off
}

//
// === Touch section
//
void Pervasive_Touch_Small::d_beginTouch()
{
    // v_touchTrim = 0x10; // standard threshold
    // v_touchEvent = true;

    // Target   FSM_ON
    // Source   FSM_OFF -> FSM_SLEEP
    //          FSM_SLEEP
    if (d_fsmPowerTouch == FSM_OFF)
    {
        hV_HAL_Wire_begin();

#if (DEBUG_POWER > 0)

        hV_HAL_log(LEVEL_DEBUG, "%24s %4s I2C.GPIO %02x -> %02x", "d_beginTouch", "I2C", d_fsmPowerTouch, (d_fsmPowerTouch | FSM_SLEEP));

#endif // DEBUG_POWER

        d_fsmPowerTouch |= FSM_BUS_MASK;
    }

    // if (b_pin.touchInt != NOT_CONNECTED) already tested
    hV_HAL_GPIO_define(b_pin.touchInt, INPUT_PULLUP);

    // if (b_pin.touchReset != NOT_CONNECTED) already tested
    hV_HAL_GPIO_define(b_pin.touchReset, OUTPUT);
    d_fsmPowerTouch |= FSM_GPIO_MASK;

    if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_271)
    {
        hV_HAL_GPIO_set(b_pin.touchReset);
        hV_HAL_delayMilliseconds(100);
        hV_HAL_GPIO_clear(b_pin.touchReset);
        hV_HAL_delayMilliseconds(100);
        hV_HAL_GPIO_set(b_pin.touchReset);
        hV_HAL_delayMilliseconds(100);

        d_touchAddress = TOUCH_271_ADDRESS; // 0x41

        // // v_touchXmax and v_touchYmax read from controller
        // uint8_t bufferWrite[1] = {0};
        // uint8_t bufferRead[10] = {0};
        //
        // bufferWrite[0] = 0x20;
        // hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, bufferRead, 10);
        // v_touchXmin = 0;
        // v_touchXmax = bufferRead[0] + (bufferRead[1] << 8); // Xmax
        // v_touchYmin = 0;
        // v_touchYmax = bufferRead[2] + (bufferRead[3] << 8); // Ymax

        // // v_touchXmax and v_touchYmax hard-coded
        // v_touchXmin = 0;
        // v_touchXmax = 176; // Xmax
        // v_touchYmin = 0;
        // v_touchYmax = 264; // Ymax
    }
    // else if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_343)
    // {
    //     hV_HAL_GPIO_set(b_pin.touchReset);
    //     hV_HAL_delayMilliseconds(10);
    //     hV_HAL_GPIO_clear(b_pin.touchReset);
    //     hV_HAL_delayMilliseconds(10);
    //     hV_HAL_GPIO_set(b_pin.touchReset);
    //     hV_HAL_delayMilliseconds(1000);
    //
    //     d_touchAddress = TOUCH_343_ADDRESS; // 0x4A
    //
    //     // uint8_t bufferWrite[1] = {0};
    //     // uint8_t bufferRead[4] = {0};
    //     //
    //     // bufferWrite[0] = 0xa6;
    //     // hV_HAL_Wire_transfer(_touchAddress, bufferWrite, 1, bufferRead, 4);
    //
    //     // // v_touchXmax and v_touchYmax hard-coded
    //     // v_touchXmin = 0;
    //     // v_touchXmax = 455; // Xmax, hardware hard-coded
    //     // v_touchYmin = 0;
    //     // v_touchYmax = 391; // Ymax, hardware hard-coded
    // }
    else if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_370)
    {
        hV_HAL_GPIO_set(b_pin.touchReset);
        hV_HAL_delayMilliseconds(10);
        hV_HAL_GPIO_clear(b_pin.touchReset);
        hV_HAL_delayMilliseconds(10);
        hV_HAL_GPIO_set(b_pin.touchReset);
        hV_HAL_delayMilliseconds(1000);

        d_touchAddress = TOUCH_370_ADDRESS; // 0x38

        // uint8_t bufferWrite[1] = {0};
        // uint8_t bufferRead[4] = {0};
        //
        // bufferWrite[0] = 0xa6;
        // hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, bufferRead, 4);

        // // v_touchXmax and v_touchYmax hard-coded
        // v_touchXmin = 0;
        // v_touchXmax = 239; // Xmax, hardware hard-coded
        // v_touchYmin = 0;
        // v_touchYmax = 415; // Ymax, hardware hard-coded
    }
    d_touchPrevious = TOUCH_EVENT_NONE;

    // Target   FSM_ON
    // Source   FSM_SLEEP -> FSM_ON

#if (DEBUG_POWER > 0)

    hV_HAL_log(LEVEL_DEBUG, "%24s %4s I2C.GPIO %02x -> %02x", "d_beginTouch", "GPIO", d_fsmPowerTouch, FSM_ON);

#endif // DEBUG_POWER

    d_fsmPowerTouch = FSM_ON;
}

void Pervasive_Touch_Small::d_getRawTouch(touch_t & touch)
{
    bool flagValid = false;
    uint8_t flagInterrupt = 1 - hV_HAL_GPIO_get(b_pin.touchInt);
    hV_HAL_delayMilliseconds(10);

    if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_271)
    {
        uint8_t bufferWrite[1] = {0};
        uint8_t bufferRead[5] = {0};

        bufferWrite[0] = 0x10; // check
        hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, bufferRead, 1);

        uint8_t number = bufferRead[0];
        touch.z = 0;
        touch.t = TOUCH_EVENT_NONE;

        // Only one finger read, flagInterrupt not used
        // if ((number > 0) and (number < 3) and (flagInterrupt > 0))
        if ((number > 0) and (number < 3))
        {
            bufferWrite[0] = 0x11; // report
            hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, bufferRead, 5);

            uint8_t status = bufferRead[0];
            touch.x = (bufferRead[1] << 8) + bufferRead[2];
            touch.y = (bufferRead[3] << 8) + bufferRead[4];

            if (status & 0x80) // touch
            {
                touch.t = (d_touchPrevious != TOUCH_EVENT_NONE) ? TOUCH_EVENT_MOVE : TOUCH_EVENT_PRESS;

                // Keep position for next release
                d_touchPrevious = TOUCH_EVENT_PRESS;
                d_touchX = touch.x;
                d_touchY = touch.y;
            }
            else
            {
                touch.t = TOUCH_EVENT_RELEASE;
            }
            touch.z = 0x16;
        }
        else // no touch
        {
            if (d_touchPrevious == TOUCH_EVENT_NONE)
            {
                touch.t = TOUCH_EVENT_NONE;
                touch.z = 0;
            }
            else // Take previous position for release
            {
                d_touchPrevious = TOUCH_EVENT_NONE;
                touch.t = TOUCH_EVENT_RELEASE;
                touch.x = d_touchX;
                touch.y = d_touchY;
                touch.z = 0x16;
            }
        }
    }
    // else if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_343)
    // {
    //     touch.z = 0;
    //     touch.t = TOUCH_EVENT_NONE;
    //
    //     // Only one finger read
    //     if (flagInterrupt > 0)
    //     {
    //         uint8_t bufferWrite[1];
    //         uint8_t bufferRead[3 + 6 * 1];
    //
    //         bufferWrite[0] = 0x00;
    //         hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 0, bufferRead, 3 + 6 * 1); // report
    //
    //         // char * stringEvent[] = {"Down", "Up", "Contact", "Reserved"};
    //         // uint8_t event = bufferRead[3 + 6 * 0 + 0] >> 6;
    //         // uint8_t id = bufferRead[3 * 0 + 6 * 0 + 2] >> 4; // 0= Down, 1= Up, 2= Contact, 3= Reserved
    //         // bool flagValid = (id < 0x0f);
    //         flagValid = (bufferRead[3 * 0 + 6 * 0 + 0] != 0xff) and ((bufferRead[3 * 0 + 6 * 0 + 1] & 0x0f) > 0x00);
    //
    //         if (flagValid)
    //         {
    //             touch.x = ((bufferRead[0 + 6 * 0 + 3] & 0x0f) << 8) + bufferRead[0 + 6 * 0 + 2];
    //             touch.y = ((bufferRead[0 + 6 * 0 + 5] & 0x0f) << 8) + bufferRead[0 + 6 * 0 + 4];
    //
    //             touch.t = (d_touchPrevious != TOUCH_EVENT_NONE) ? TOUCH_EVENT_MOVE : TOUCH_EVENT_PRESS;
    //
    //             // Keep position for next release
    //             d_touchPrevious = TOUCH_EVENT_PRESS;
    //             d_touchX = touch.x;
    //             d_touchY = touch.y;
    //             touch.z = 0x16;
    //         }
    //         else
    //         {
    //             touch.t = TOUCH_EVENT_RELEASE;
    //         }
    //     }
    else if (SCREEN_SIZE(u_eScreen_EPD) == SIZE_370)
    {
        touch.z = 0;
        touch.t = TOUCH_EVENT_NONE;

        // Only one finger read
        if (flagInterrupt > 0) // touch
        {
            uint8_t bufferWrite[1];
            uint8_t bufferRead[3 + 6];

            bufferWrite[0] = 0x00;
            hV_HAL_Wire_transfer(d_touchAddress, bufferWrite, 1, bufferRead, 3 + 6); // report

            // char * stringEvent[] = {"Down", "Up", "Contact", "Reserved"};
            // uint8_t event = bufferRead[3 + 6 * 0 + 0] >> 6;
            uint8_t id = bufferRead[3 + 6 * 0 + 2] >> 4; // 0= Down, 1= Up, 2= Contact, 3= Reserved
            bool flagValid = (id < 0x0f);

            if (flagValid)
            {
                touch.x = ((bufferRead[3 + 6 * 0 + 0] & 0x0f) << 8) + bufferRead[3 + 6 * 0 + 1];
                touch.y = ((bufferRead[3 + 6 * 0 + 2] & 0x0f) << 8) + bufferRead[3 + 6 * 0 + 3];

                touch.t = (d_touchPrevious != TOUCH_EVENT_NONE) ? TOUCH_EVENT_MOVE : TOUCH_EVENT_PRESS;

                // Keep position for next release
                d_touchPrevious = TOUCH_EVENT_PRESS;
                d_touchX = touch.x;
                d_touchY = touch.y;
                touch.z = 0x16;
            }
            else
            {
                touch.t = TOUCH_EVENT_RELEASE;
            }
        }
        else // no touch
        {
            if (d_touchPrevious == TOUCH_EVENT_NONE)
            {
                touch.t = TOUCH_EVENT_NONE;
                touch.z = 0;
            }
            else // Take previous position for release
            {
                d_touchPrevious = TOUCH_EVENT_NONE;
                touch.t = TOUCH_EVENT_RELEASE;
                touch.x = d_touchX;
                touch.y = d_touchY;
                touch.z = 0x16;
            }
        }
    } // u_eScreen_EPD
}

bool Pervasive_Touch_Small::d_getInterruptTouch()
{
    // if (b_pin.touchInt != NOT_CONNECTED) already tested
    // Translate for true = interrupt
    // 271, 343 and 370: LOW = false for interrupt
    return (hV_HAL_GPIO_get(b_pin.touchInt) == LOW);
}
//
// === End of Touch section
//

