/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _SPI_SST26_H
#define _SPI_SST26_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

/* SST26 Flash Commands */
#define APP_SST26_CMD_ENABLE_RESET                      0x66
#define APP_SST26_CMD_MEMORY_RESET                      0x99
#define APP_SST26_CMD_STATUS_REG_READ                   0x05
#define APP_SST26_CMD_CONFIG_REG_READ                   0x35
#define APP_SST26_CMD_MEMORY_READ                       0x03
#define APP_SST26_CMD_MEMORY_HIGH_SPEED_READ            0x0B
#define APP_SST26_CMD_ENABLE_WRITE                      0x06
#define APP_SST26_CMD_DISABLE_WRITE                     0x04
#define APP_SST26_CMD_4KB_SECTOR_ERASE                  0x20
#define APP_SST26_CMD_BLOCK_ERASE                       0xD8
#define APP_SST26_CMD_CHIP_ERASE                        0xC7
#define APP_SST26_CMD_PAGE_PROGRAM                      0x02
#define APP_SST26_CMD_JEDEC_ID_READ                     0x9F
#define APP_SST26_CMD_GLOBAL_BLOCK_PROTECTION_UNLOCK    0x98

#define APP_SST26_STATUS_BIT_RES_0                      (0x01 << 0)
#define APP_SST26_STATUS_BIT_WEL                        (0x01 << 1)
#define APP_SST26_STATUS_BIT_WSE                        (0x01 << 2)
#define APP_SST26_STATUS_BIT_WSP                        (0x01 << 3)
#define APP_SST26_STATUS_BIT_WPLD                       (0x01 << 4)
#define APP_SST26_STATUS_BIT_SEC                        (0x01 << 5)
#define APP_SST26_STATUS_BIT_RES_6                      (0x01 << 6)
#define APP_SST26_STATUS_BIT_BUSY                       (0x01 << 7)

#define APP_SST26_PAGE_PROGRAM_SIZE_BYTES               256
#define APP_SST26_CS_ENABLE()                           (LATACLR = (1U<<1))
#define APP_SST26_CS_DISABLE()                          (LATASET = (1U<<1))
#define APP_SST26_MEM_ADDR                              0x10000
#define LED_On()                                        (LATKSET = (1U<<1))
#define LED_Off()                                       (LATKCLR = (1U<<1))

    
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
	APP_SST26_STATE_INITIALIZE,
    APP_SST26_STATE_WAIT_MIN_POWER_UP_TIME,
    APP_SST26_STATE_RESET,
    APP_SST26_STATE_GLOBAL_BLK_PROTECTION_UNLOCK,
    APP_SST26_STATE_JEDEC_ID_READ,            
    APP_SST26_STATE_SECTOR_ERASE,
    APP_SST26_STATE_READ_STATUS,    
    APP_SST26_STATE_PAGE_PROGRAM,
    APP_SST26_STATE_MEMORY_READ,
    APP_SST26_STATE_VERIFY,
    APP_SST26_STATE_XFER_SUCCESSFUL,
    APP_SST26_STATE_XFER_ERROR,    
    APP_SST26_STATE_IDLE,    
} APP_SST26_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    APP_SST26_STATES    state;
    APP_SST26_STATES    nextState;
    uint8_t             transmitBuffer[APP_SST26_PAGE_PROGRAM_SIZE_BYTES + 5];    
    uint8_t             manufacturerID;
    uint16_t            deviceID;
    uint8_t             isCSDeAssert;
    volatile bool       isTransferDone;
}APP_SPI_SST26_DATA;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_SPI_SST26_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_SPI_SST26_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_SPI_SST26_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_SPI_SST26_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _SPI_SST26_H */

/*******************************************************************************
 End of File
 */

