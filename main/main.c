/**************************************************************************************************/
/** @file     main.c
 *  @brief    x
 *  @details  x
 *
 *  @author   Justin Reina, Firmware Engineer
 *  @created  3/7/25
 *  @last rev 3/7/25
 *
 *
 *  @notes    x
 *
 *  @section    Opens
 *      switch to Core\ demo model of RTOS
 *		switch music from component to direct source
 *
 *  @section    Legal Disclaimer
 *      © 2025 Justin Reina, All rights reserved. All contents of this source file and/or any other
 *      related source files are the explicit property of Justin Reina. Do not distribute.
 *      Do not copy.
 *
 *  @section    Source
 *      SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *      SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/**************************************************************************************************/


//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Project Includes
#include "waveshare_rgb_lcd_port.h"


/**************************************************************************************************/
/** @fcn        void app_main(void)
 *  @brief      x
 *  @details    x
 */
/**************************************************************************************************/
void app_main(void) {
	
	//---------------------------------------- Initialize ----------------------------------------//
    waveshare_esp32_s3_rgb_lcd_init(); 				/* Initialize the Waveshare ESP32-S3 RGB LCD  */

    ESP_LOGI(TAG, "Display LVGL demos");

    //------------------------------------------ Operate -----------------------------------------//
    
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(-1)) {
        // lv_demo_stress();
        // lv_demo_benchmark();
         lv_demo_music();
        //lv_demo_widgets();
        // example_lvgl_demo_ui();
        // Release the mutex
        lvgl_port_unlock();
    }
}

