///
/// @file Pervasive_Touch_Small.h
/// @brief Driver for small screens with wide temperature and embedded fast update
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
/// * ApplicationNote_Small_Size_wide-Temperature_EPD_v03_20231031_B
/// * ApplicationNote_Small_Size_wide-Temperature_EPD_v01_20231225_A
///
/// @author Rei Vilo
/// @date 21 May 2025
/// @version 909
///
/// @copyright (c) Rei Vilo, 2010-2025
/// @copyright All rights reserved
/// @copyright For exclusive use with Pervasive Displays screens
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
/// @see https://creativecommons.org/licenses/by-sa/4.0/
///
/// @n Consider the Evaluation or Commercial editions for professionals or organisations and for commercial usage
///
/// * Evaluation edition: for professionals or organisations, evaluation only, no commercial usage
/// @n All rights reserved
///
/// * Commercial edition: for professionals or organisations, commercial usage
/// @n All rights reserved
///
/// * Viewer edition: for professionals or organisations
/// @n All rights reserved
///
/// * Documentation
/// @n All rights reserved
///

// SDK and configuration
#include "PDLS_Common.h"

#if (PDLS_COMMON_RELEASE < 902)
#error Required PDLS_COMMON_RELEASE 902
#endif // PDLS_COMMON_RELEASE

// Driver
#include "Driver_EPD_Virtual.h"

#ifndef DRIVER_TOUCH_SMALL_RELEASE
///
/// @brief Library release number
///
#define DRIVER_TOUCH_SMALL_RELEASE 909

///
/// @name List of supported screens
/// @details Monochrome touch screens with embedded fast update
///
/// @note Normal and fast update modes
/// @deprecated Partial update is removed. Use fast update instead (7.0.0)
/// @see https://www.pervasivedisplays.com/products
/// @{
///
#define eScreen_EPD_271_PS_09_Touch SCREEN(SIZE_271, FILM_P, DRIVER_9) | EXTRA(EXTRA_TOUCH) ///< reference 271-PS-09
#define eScreen_EPD_271_KS_09_Touch SCREEN(SIZE_271, FILM_K, DRIVER_9) | EXTRA(EXTRA_TOUCH) ///< reference 271-KS-09
// #define eScreen_EPD_343_PS_0B_Touch SCREEN(SIZE_343, FILM_P, DRIVER_B) | EXTRA(EXTRA_TOUCH) ///< reference 343-PS-0B
#define eScreen_EPD_370_PS_0C_Touch SCREEN(SIZE_370, FILM_P, DRIVER_C) | EXTRA(EXTRA_TOUCH) ///< reference 370-PS-0C
#define eScreen_EPD_370_KS_0C_Touch SCREEN(SIZE_370, FILM_K, DRIVER_C) | EXTRA(EXTRA_TOUCH) ///< reference 370-PS-0C
/// @}

///
/// @name List of features
/// @{
///
#define WITH_MONOCHROME ///< With B&W only
#define WITH_FAST ///< With fast update capability
#define WITH_TOUCH ///< With touch capability
/// @}

///
/// @brief Driver variant
///
#define DRIVER_EPD_RELEASE DRIVER_TOUCH_SMALL_RELEASE
#define DRIVER_EPD_VARIANT "Touch small"

///
/// @brief Touch small screens class
///
class Pervasive_Touch_Small : public Driver_EPD_Virtual
{
  public:

    ///
    /// @brief Constructor
    /// @param eScreen_EPD_EXT3 size and model of the e-screen
    /// @param board board configuration
    ///
    Pervasive_Touch_Small(eScreen_EPD_t eScreen_EPD, pins_t board);

    /// @name General
    /// @{

    ///
    /// @brief Initialisation
    /// @details Initialise the board and read OTP
    ///
    void begin();

    ///
    /// @brief Driver reference
    ///
    /// @return STRING_CONST_TYPE scope and release number
    /// @note Example `Wide touch v9.0.0`
    ///
    STRING_CONST_TYPE reference();

    ///
    /// @brief Normal update
    ///
    /// @param frame next image
    /// @param sizeFrame size of the frame
    ///
    void updateNormal(FRAMEBUFFER_CONST_TYPE frame1,
                      uint32_t sizeFrame);

    ///
    /// @brief Fast update
    ///
    /// @param frame1 next image
    /// @param frame2 previous image
    /// @param sizeFrame size of the frame
    ///
    void updateFast(FRAMEBUFFER_CONST_TYPE frame1,
                    FRAMEBUFFER_CONST_TYPE frame2, uint32_t sizeFrame);

    /// @}

  protected:

    //
    // === Touch section
    //
    virtual void d_getRawTouch(touch_t & touch);
    virtual bool d_getInterruptTouch();
    //
    // === End of Touch section
    //

  private:

    // Variables and functions specific to the screen
    uint8_t COG_data[112]; // OTP
    bool s_flag50; // Register 0x50

    void COG_reset();
    void COG_getDataOTP();
    void COG_initial(uint8_t updateMode);
    void COG_sendImageDataFast(FRAMEBUFFER_CONST_TYPE frame1, FRAMEBUFFER_CONST_TYPE frame2, uint32_t sizeFrame);
    void COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE frame1, uint32_t sizeFrame);
    void COG_update();
    void COG_stopDCDC();

    //
    // === Touch section
    //
    uint8_t d_touchAddress;
    uint8_t d_touchPrevious;
    uint16_t d_touchX, d_touchY;
    uint8_t d_fsmPowerTouch = FSM_OFF;

    void d_beginTouch();
    //
    // === End of Touch section
    //
};

#endif // DRIVER_TOUCH_SMALL_RELEASE
