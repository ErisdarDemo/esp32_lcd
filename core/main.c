/**************************************************************************************************/
/** @file     main.c
 *  @brief    ESP32 Base C Project Template
 *  @details  For new ESP32 development
 *
 *  @author   Justin Reina, Firmware Engineer
 *  @created  4/16/25
 *  @last rev 5/10/25
 *
 *  @note   Private functions & variables are declared static
 *
 *  @section    Opens
 *      Extension to C++
 *
 *  @section    Legal Disclaimer
 *      ©2025 Justin Reina. All rights reserved. All contents of this source file and/or any other
 *      related source files are the explicit property of Justin Reina. Do not distribute.
 *      Do not copy.
 */
/**************************************************************************************************/

//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Standard Library Includes
#include <stdio.h>

//Project Includes
#include "system.h"

//Application Includes
#include "demo.h"


//************************************************************************************************//
//                                        DEFINITIONS & TYPES                                     //
//************************************************************************************************//

//-----------------------------------------  Definitions -----------------------------------------//

//Timing
#define SLEEP_DELAY_MS      (2000)                  /* nice loop delay for runtime demo           */

//Demo
#define MAX_LOOP_CT         (5)                     /* reset after a few loops                    */


//************************************************************************************************//
//                                          PUBLIC FUNCTIONS                                      //
//************************************************************************************************//

/**************************************************************************************************/
/** @fcn        void app_main(void)
 *  @brief      FreeRTOS task for main application
 *  @details    Called by FreeRTOS scheduler when started, running on core for main thread
 *
 *  @section    Purpose
 *      Unlike normal FreeRTOS tasks, or embedded C main functions, the app_main() task is allowed 
 *      to return. If this happens, The task is cleaned up and the system will continue running 
 *      with other RTOS tasks scheduled normally. Therefore, it is possible to implement app_main 
 *      as either a function that creates other application tasks and then returns, or as a main 
 *      application task itself. app_main() has a fixed RTOS priority, one higher than the minimum
 *
 *  @pre    second stage bootloader
 *  @post   no return
 */
/**************************************************************************************************/
void app_main(void) {
    
    //Locals
    int ctr = 0;                                    /* loop counter                               */

    
    //-------------------------------------- Initialization --------------------------------------//

    //Init
    system_initialize();

   
    //--------------------------------------- Application ----------------------------------------//

    for(;;) {

        //------------------------------------- Update -------------------------------------------//
        
        //C Operate 
        demo_routine(ctr);
        
        //Notify
        printf("app_main(): loop %d\n\n", ctr++);

        
        //------------------------------------- Reset --------------------------------------------//

        //Catch
        if(ctr > MAX_LOOP_CT) {
            break;
        }

        //Delay
        delay_ms(SLEEP_DELAY_MS);
    }
}

