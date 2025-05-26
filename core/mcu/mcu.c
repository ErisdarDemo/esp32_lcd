/**************************************************************************************************/
/** @file     mcu.c
 *  @brief    Microcontroller operations & utility
 *  @details  x
 */
/**************************************************************************************************/

//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Standard Library includes
#include <stdio.h>
#include <stdint.h>

//Library Includes
#include "esp_chip_info.h"
#include "esp_flash.h"

//FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//Project Includes
#include "system.h"


//************************************************************************************************//
//                                          PUBLIC FUNCTIONS                                      //
//************************************************************************************************//

/**************************************************************************************************/
/** @fcn        status_code mcu_initialize(void)
 *  @brief      Initialize microcontroller & drivers for application
 *  @details    x
 *
 *  @return   (status_code) system initialization status
 *
 *  @pre    system_initialize()
 *  @post   microntroller & driver api are prepared for application use
 *
 *  @section    Opens
 *      Implement routine
 */
/**************************************************************************************************/
status_code mcu_initialize(void) {

    //Locals


    //---------------------------------------- Initialize ----------------------------------------//


    //------------------------------------------ Operate -----------------------------------------//


    //Notify
    printf("mcu_initialize(): MCU is prepared for application.\n\n");

    return STATUS_OK;
}


/**************************************************************************************************/
/** @fcn        void mcu_print_info(void)
 *  @brief      test print for mcu config
 *  @details    display contents from hello_world
 *
 *  @pre    system_initialize()
 */
/**************************************************************************************************/
void mcu_print_info(void) {
    
    //Locals
    esp_err_t stat;                                 /* sdk response code                          */
    esp_chip_info_t chip_info;                      /* Chip information from EEPROM               */
    unsigned major_rev;                             /* Chip ID major revision number              */
    unsigned minor_rev;                             /* Chip ID minor revision number              */
    uint32_t flash_size;                            /* flash bank memory size in bytes            */
    uint32_t flash_print_size;                      /* flash size to print with console           */
    uint32_t min_flash_size;                        /* min heap flash size                        */
    char * flash_type;                              /* display information of flash type          */


    //********************************************************************************************//
    //                                     CHIP INFORMATION                                      //
    //********************************************************************************************//

    //Get info
    esp_chip_info(&chip_info);

    //Get ID
    major_rev = (chip_info.revision / 100);
    minor_rev = (chip_info.revision % 100);


    //CHIP
    printf("    - Chip: %s (%d cores)\n", CONFIG_IDF_TARGET, chip_info.cores);

    //WIFI
    if(chip_info.features & CHIP_FEATURE_WIFI_BGN) {
        printf("    - Wifi: Y\n");
    }

    //BLUETOOTH
    if(chip_info.features & CHIP_FEATURE_BT) {
        printf("    - BT: Y\n");
    }

    //BLE
    if(chip_info.features & CHIP_FEATURE_BLE) {
        printf("    - BLE: Y\n");
    }

    //ZIGBEE
    if(chip_info.features & CHIP_FEATURE_IEEE802154) {
        printf("    - Zigbee: Y\n");
    }

    //ID    
    printf("    - Silicon revision v%d.%d\n", major_rev, minor_rev);
    

    //********************************************************************************************//
    //                                     FLASH INFORMATION                                      //
    //********************************************************************************************//

    //Get Size
    stat = esp_flash_get_size(NULL, &flash_size);

    //Safety
    if(stat != ESP_OK) {
        printf("Get flash size failed");
        return;
    }
    
    //Get Flash Info
    flash_print_size = (flash_size / (uint32_t)(1024 * 1024));

    flash_type       = (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external";
    
    min_flash_size   = esp_get_minimum_free_heap_size();

    //Print Flash Info
    printf("    - %" PRIu32 "MB %s flash\n", flash_print_size, flash_type);

    printf("    - Minimum free heap size: %" PRIu32 " bytes\n\n", min_flash_size);


    return;
}

