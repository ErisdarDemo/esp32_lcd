/**************************************************************************************************/
/** @file       system.c
 *  @brief      System operations & utility
 *  @details    x
 *
 * @section     Opens
 *      system_getStatus()
 */
/**************************************************************************************************/

//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Standard Library Includes
#include <stdio.h>

//FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//Project Includes
#include "mcu.h"
#include "rtos.h"
#include "system.h"


//************************************************************************************************//
//                                          PUBLIC FUNCTIONS                                      //
//************************************************************************************************//

/**************************************************************************************************/
/** @fcn        status_code system_initialize(void)
 *  @brief      Initialize system for application
 *  @details    Satisfies all task & feature requirements
 *
 *  @return   (status_code) system initialization status
 *
 *  @pre    app_main()
 *  @post   system is initialized & prepared for application
 */
/**************************************************************************************************/
status_code system_initialize(void) {

    //Locals


    //--------------------------------------- Init Console ---------------------------------------// 

    //Header
    printf("\n\n\n");
    printf("//****************************************************************************//\n");
    printf("//                                                                            //\n");
    printf("//****************************************************************************//\n");
    printf("\n");

    
    //----------------------------------------- Init MCU -----------------------------------------// 
    mcu_initialize();

    //Detail
    mcu_print_info();
            

    //---------------------------------------- Init RTOS -----------------------------------------//
    rtos_init();


    //Notify
    printf("system_initialize(): System is prepared for application.\n\n");

    return STATUS_OK;
}



/**************************************************************************************************/
/** @fcn        void delay_ms(int t_ms)
 *  @brief      delay routine
 *  @details    x
 *
 *  @param [in] (int) t_ms - delay time, milliseconds
 *
 *  @pre    system_initialize()
 *  @post   x
 */
/**************************************************************************************************/
void delay_ms(int t_ms) {

    //Delay
    vTaskDelay(t_ms / portTICK_PERIOD_MS);

    return;
}

