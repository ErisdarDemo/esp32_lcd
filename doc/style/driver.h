/**************************************************************************************************/
/** @file     driver.h
 *  @brief    Driver interface for public API
 *  @details  interface file header is typically omitted in preference for source file header
 *
 *  @note   Consider replacing driverConfig.comps with dynamic alloc as a ptr
 */
/**************************************************************************************************/
#ifndef DIR_DRIVER_H
#define DIR_DRIVER_H

//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Standard Library Includes

//Library Includes

//BSP Includes

//Project Includes


//************************************************************************************************//
//                                        DEFINITIONS & TYPES                                     //
//************************************************************************************************//

//-----------------------------------------  Definitions -----------------------------------------//

#define DRIVER_NUM_COMPS    (1)


//-------------------------------------------- Macros --------------------------------------------//


//----------------------------------------- Enumerations -----------------------------------------//


//------------------------------------------- Typedefs -------------------------------------------//

//Setup
typedef struct driverConfig {
    int comps[DRIVER_NUM_COMPS];                    /* qualifying descrip for comps e.g.  pins    */
    
    int paramOne;                                   /* qualifying descrip for param               */
    int paramTwo;                                   /* qualifying descrip for param               */
    /* .... */  
} DriverConfig;


//************************************************************************************************//
//                                            VARIABLES                                           //
//************************************************************************************************//


//************************************************************************************************//
//                                       FUNCTION DECLARATIONS                                    //
//************************************************************************************************//

//-------------------------------------------- Public --------------------------------------------//

//Setup
extern void driver_init(DriverConfig *cfg);
extern void driver_enable(void);
extern void driver_disable(void);

//Action
extern void  driver_start(void);
extern void  driver_stop(void);
extern void *driver_status(void);                   /* current status of API performance          */
extern void  driver_reset(void);

//Configure
extern void driver_setConfig(DriverConfig *cfg);
extern DriverConfig *driver_getConfig(void);        /* applied configuration for use              */
extern void *driver_getInfo(void);                  /* current operational state                  */

//Get-Set
extern int  driver_getState(void);
extern void driver_setState(int val);

//Read-Write
extern int  driver_read(void);
extern void driver_write(int val);

//Log
extern void *driver_getLog(void);

//Int
extern void driver_callback(void);                  /* default peripheral interrupt callback      */


//-------------------------------------------- Private -------------------------------------------//


#endif /*  DIR_DRIVER_H */

