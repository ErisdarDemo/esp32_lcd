#ifndef CORE_MCU_H_
#define CORE_MCU_H_

//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Project Includes
#include "system.h"


//************************************************************************************************//
//                                       FUNCTION DECLARATIONS                                    //
//************************************************************************************************//

//Public
extern status_code mcu_initialize(void);

extern void mcu_print_info(void);                   /* and section divs (e.g. 'DECLARATIONS' here)
                                                       are also not recommended for lightweight 
                                                       interface files like this either - 'KISS'  */
#endif /* CORE_MCU_H_ */

