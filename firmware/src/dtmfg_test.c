/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "dtmfg_test.h"
#include "config/pic32mz_w1_eth_wifi_freertos/system/command/sys_command.h" 
#include "config/pic32mz_w1_eth_wifi_freertos/system/console/sys_console.h"
#include "config/pic32mz_w1_eth_wifi_freertos/peripheral/gpio/plib_gpio.h"
#include "config/pic32mz_w1_eth_wifi_freertos/peripheral/spi/spi_master/plib_spi1_master.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions


static APP_SPI_SST26_DATA          appSST26Data;
static uint8_t                 writeDataBuffer[APP_SST26_PAGE_PROGRAM_SIZE_BYTES];
static uint8_t                 readDataBuffer[APP_SST26_PAGE_PROGRAM_SIZE_BYTES];


void APP_SST26_Reset(void)
{    
    appSST26Data.isTransferDone = false; 
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_ENABLE_RESET;
    
    APP_SST26_CS_ENABLE(); 
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1);        
    while (appSST26Data.isTransferDone == false);  
    
    appSST26Data.isTransferDone = false;        
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_MEMORY_RESET;
    
    APP_SST26_CS_ENABLE(); 
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1); 
    
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_WriteEnable(void)
{
    appSST26Data.isTransferDone = false;    
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_ENABLE_WRITE;
    
    APP_SST26_CS_ENABLE(); 
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1);    
    
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_WriteDisable(void)
{
    appSST26Data.isTransferDone = false;    
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_DISABLE_WRITE;
    
    APP_SST26_CS_ENABLE();
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1);    
    
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_SectorErase(uint32_t address)
{       
    APP_SST26_WriteEnable();
    
    appSST26Data.isTransferDone = false;    
    
    /* The address bits from A11:A0 are don't care and must be Vih or Vil */
    address = address & 0xFFFFF000;
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_4KB_SECTOR_ERASE;
    appSST26Data.transmitBuffer[1] = (address >> 16);
    appSST26Data.transmitBuffer[2] = (address >> 8);
    appSST26Data.transmitBuffer[3] = address;
    
    APP_SST26_CS_ENABLE();   
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 4);    
    
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_ChipErase(void)
{       
    APP_SST26_WriteEnable();
    
    appSST26Data.isTransferDone = false;            
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_CHIP_ERASE;    
    
    APP_SST26_CS_ENABLE();  
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1);    
    
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_PageProgram(uint32_t address, uint8_t* pPageData)
{        
    uint32_t i;
    
    APP_SST26_WriteEnable();
    
    appSST26Data.isTransferDone = false;                   
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_PAGE_PROGRAM;
    appSST26Data.transmitBuffer[1] = (address >> 16);
    appSST26Data.transmitBuffer[2] = (address >> 8);
    appSST26Data.transmitBuffer[3] = address;
    
    for (i = 0; i < APP_SST26_PAGE_PROGRAM_SIZE_BYTES; i++)
    {
        appSST26Data.transmitBuffer[4 + i] = pPageData[i];
    }
    
    APP_SST26_CS_ENABLE(); 
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, (4 + APP_SST26_PAGE_PROGRAM_SIZE_BYTES));    
        
    while (appSST26Data.isTransferDone == false);  
}

void APP_SST26_MemoryRead(uint32_t address, uint8_t* pReadBuffer, uint32_t nBytes, bool isHighSpeedRead)
{                        
    uint8_t nTxBytes;
    
    appSST26Data.isTransferDone = false;                  
        
    appSST26Data.transmitBuffer[1] = (address >> 16);
    appSST26Data.transmitBuffer[2] = (address >> 8);
    appSST26Data.transmitBuffer[3] = address;        
    
    if (isHighSpeedRead == true)
    {
        appSST26Data.transmitBuffer[0] = APP_SST26_CMD_MEMORY_HIGH_SPEED_READ;
        /* For high speed read, perform a dummy write */
        appSST26Data.transmitBuffer[4] = 0xFF;  
        nTxBytes = 5;
    }
    else
    {
        appSST26Data.transmitBuffer[0] = APP_SST26_CMD_MEMORY_READ;
        nTxBytes = 4;
    }
    
    APP_SST26_CS_ENABLE();  
    appSST26Data.isCSDeAssert = false;    
    SPI1_Write(appSST26Data.transmitBuffer, nTxBytes);    
    
    while (appSST26Data.isTransferDone == false);  
    
    appSST26Data.isTransferDone = false;                 
    appSST26Data.isCSDeAssert = true;
    SPI1_Read(pReadBuffer, nBytes);    
        
    while (appSST26Data.isTransferDone == false);  
}

uint8_t APP_SST26_StatusRead(void)
{
    uint8_t status;
    appSST26Data.isTransferDone = false;    
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_STATUS_REG_READ;
    
    APP_SST26_CS_ENABLE();        
    appSST26Data.isCSDeAssert = true;    
    SPI1_WriteRead(appSST26Data.transmitBuffer, 1, appSST26Data.transmitBuffer, (1+1));    
        
    while (appSST26Data.isTransferDone == false); 
    
    status = appSST26Data.transmitBuffer[1];
    
    return status;
}

uint8_t APP_SST26_ConfigRegisterRead(void)
{
    uint8_t config_reg;
    appSST26Data.isTransferDone = false;    
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_CONFIG_REG_READ;
    
    APP_SST26_CS_ENABLE();   
    appSST26Data.isCSDeAssert = true;    
    SPI1_WriteRead(appSST26Data.transmitBuffer, 1, appSST26Data.transmitBuffer, (1+1));    
        
    while (appSST26Data.isTransferDone == false);  
    
    config_reg = appSST26Data.transmitBuffer[1];
    
    return config_reg;
}

void APP_SST26_JEDEC_ID_Read(uint8_t* manufacturerID, uint16_t* deviceID)
{
    appSST26Data.isTransferDone = false;    
    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_JEDEC_ID_READ;
    
    APP_SST26_CS_ENABLE();       
    appSST26Data.isCSDeAssert = true;      
    SPI1_WriteRead(appSST26Data.transmitBuffer, 1, appSST26Data.transmitBuffer, (1+3));    
         
    while (appSST26Data.isTransferDone == false); 
    
    *manufacturerID = appSST26Data.transmitBuffer[1];
    *deviceID = (appSST26Data.transmitBuffer[2] << 8UL) | appSST26Data.transmitBuffer[3];    
    //SYS_CONSOLE_PRINT("ManufactureID %lx Device Id %xldeviceID",*manufacturerID, *deviceID);
}

void APP_SST26_GlobalWriteProtectionUnlock(void)
{
    APP_SST26_WriteEnable();
    
    appSST26Data.isTransferDone = false;    
    appSST26Data.transmitBuffer[0] = APP_SST26_CMD_GLOBAL_BLOCK_PROTECTION_UNLOCK;
    
    APP_SST26_CS_ENABLE();        
    appSST26Data.isCSDeAssert = true;
    SPI1_Write(appSST26Data.transmitBuffer, 1);    
        
    while (appSST26Data.isTransferDone == false);          
}

void APP_SST26_MinPowerOnDelay(void)
{
    uint32_t i;        
    
    /* Cheap delay. 
     * Based on the CPU frequency, ensure the delay is at-least 100 microseconds. 
     */
    for (i = 0; i < 1000000; i++)
    {
        asm("NOP");
    }        
}

/* This function will be called by SPI PLIB when transfer is completed */
void APP_SST26_SPIEventHandler(uintptr_t context )
{
    uint8_t* isCSDeAssert = (uint8_t*)context;
    
    if (*isCSDeAssert == true)
    {
        /* De-assert the chip select */
        APP_SST26_CS_DISABLE();
    }
            
    appSST26Data.isTransferDone = true;
}

void APP_SST26_Initialize (void)
{
    uint32_t i;
    
    APP_SST26_CS_DISABLE();
    LED_Off();
    
    appSST26Data.state = APP_SST26_STATE_INITIALIZE;
    
    /* Fill up the test data */
    for (i = 0; i < APP_SST26_PAGE_PROGRAM_SIZE_BYTES; i++)
    {
        writeDataBuffer[i] = i;
    }            
}


static void     readGPIO(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void     writeGPIO(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void     SPIFlashTest(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
// built-in command table supported by Bridge application
static const SYS_CMD_DESCRIPTOR    builtinDTMFGCmdTbl[]=
{
    {"readGPIO",   readGPIO,   " :Reading GPIO input (RPB2,RPB7,RPB8,RPB9,RPA4,RPA5,RPA11,RPA13) pins status"},
    {"writeGPIO",  writeGPIO,  " :Write GPIO output pins(RPB1)"},
    {"SPIFlashTest",   SPIFlashTest,    " :Testing SPI Flash features"},
};


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
volatile bool static isSSTTesting = false;
volatile uint32_t static numOfSST26Test = 0;
volatile bool SPISST26TestResults = true;
static void readGPIO(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    (void)argv;
    (void)argc;
    (void)pCmdIO;
    SYS_CONSOLE_PRINT("GPIO status on RPB2 = %d, RPB7 = %d, RPB8 = %d, RPB9 = %d, RPA4 = %d, RPA5 = %d, RPA11 = %d, RPA13 = %d\r\n", (GPIO_PortRead(GPIO_PORT_B) >> 2) & 0x01, (GPIO_PortRead(GPIO_PORT_B) >> 7) & 0x01, (GPIO_PortRead(GPIO_PORT_B) >> 8) & 0x01,
            (GPIO_PortRead(GPIO_PORT_B) >> 9) & 0x01, (GPIO_PortRead(GPIO_PORT_A) >> 4) & 0x01, (GPIO_PortRead(GPIO_PORT_A) >> 5) & 0x01, (GPIO_PortRead(GPIO_PORT_A) >> 11) & 0x01, (GPIO_PortRead(GPIO_PORT_A) >> 13)& 0x01);
}
static void writeGPIO(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // Set these pins as output pin
    // Parse and set pins as High/Low
    // Python evaluate these values
    if((strcmp(argv[1], "H") == 0 ))
        GPIO_PortSet(GPIO_PORT_B, 0x02); //RPB1Set as High
    else
        GPIO_PortClear(GPIO_PORT_B, 0x02); 
}
static void SPIFlashTest(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    LED_Off();
    //SYS_CONSOLE_PRINT(" SPI flash under test test");
    if((strcmp(argv[1], "start") == 0 ) && (isSSTTesting != true)) 
    {
        isSSTTesting = true;
        SPISST26TestResults = true;
    }
    else if((strcmp(argv[1], "start") == 0 ) && (isSSTTesting != false)) 
    {
        SYS_CONSOLE_PRINT("Flash is Under Test\r\n");
        return;
    }
    else if((strcmp(argv[1], "stop") == 0 ) && (isSSTTesting != false)) 
    {
        isSSTTesting = false;
        SYS_CONSOLE_PRINT("\nSPI-Flash Test Iteration %d\r\n", numOfSST26Test);
        if( SPISST26TestResults )
                SYS_CONSOLE_PRINT("SPIFlashTest Pass\r\n");
        else
            SYS_CONSOLE_PRINT("\nSPIFlashTest Fail\r\n");
        numOfSST26Test = 0;
    }
    else if((strcmp(argv[1], "stop") == 0 ) && (isSSTTesting != true)) 
    {
        SYS_CONSOLE_PRINT("Flash Test is not Initiated\r\n");
        return;
    }    
    else
    {
        SYS_CONSOLE_PRINT("Wrong SPI-Flash Test Command\r\n");
        return;
    }
    appSST26Data.state = APP_SST26_STATE_RESET;
}

void SPIFlashTask(void)
{
    uint8_t status;
    while(isSSTTesting)
    {
        /* Check the application's current state. */
        switch (appSST26Data.state)
        {
            case APP_SST26_STATE_RESET:
                APP_SST26_Reset();
                appSST26Data.state = APP_SST26_STATE_WAIT_MIN_POWER_UP_TIME;
                break;
                
            case APP_SST26_STATE_WAIT_MIN_POWER_UP_TIME:
                APP_SST26_MinPowerOnDelay();
                appSST26Data.state = APP_SST26_STATE_GLOBAL_BLK_PROTECTION_UNLOCK;                                 
                break;    
                
            case APP_SST26_STATE_GLOBAL_BLK_PROTECTION_UNLOCK:
                APP_SST26_GlobalWriteProtectionUnlock();                
                appSST26Data.state = APP_SST26_STATE_JEDEC_ID_READ;
                break;
                
            case APP_SST26_STATE_JEDEC_ID_READ:
                APP_SST26_JEDEC_ID_Read(&appSST26Data.manufacturerID, &appSST26Data.deviceID);
                appSST26Data.state = APP_SST26_STATE_SECTOR_ERASE;
                break;                                
                
            case APP_SST26_STATE_SECTOR_ERASE:
                APP_SST26_SectorErase(APP_SST26_MEM_ADDR);                
                appSST26Data.state = APP_SST26_STATE_READ_STATUS;
                appSST26Data.nextState = APP_SST26_STATE_PAGE_PROGRAM;
                break;
                
            case APP_SST26_STATE_READ_STATUS:
                status = APP_SST26_StatusRead();
                if ((status & APP_SST26_STATUS_BIT_BUSY) == 0)                                             
                {
                    appSST26Data.state = appSST26Data.nextState;
                }
                break;
                
            case APP_SST26_STATE_PAGE_PROGRAM:
                APP_SST26_PageProgram(APP_SST26_MEM_ADDR, &writeDataBuffer[0]);
                appSST26Data.state = APP_SST26_STATE_READ_STATUS;
                appSST26Data.nextState = APP_SST26_STATE_MEMORY_READ;
                break;
                
            case APP_SST26_STATE_MEMORY_READ:
                APP_SST26_MemoryRead(APP_SST26_MEM_ADDR, readDataBuffer, APP_SST26_PAGE_PROGRAM_SIZE_BYTES, false);
                appSST26Data.state = APP_SST26_STATE_VERIFY;                
                break;
                
            case APP_SST26_STATE_VERIFY:
                if (memcmp(writeDataBuffer, readDataBuffer, APP_SST26_PAGE_PROGRAM_SIZE_BYTES) == 0)
                {
                    appSST26Data.state = APP_SST26_STATE_XFER_SUCCESSFUL;
                }
                else
                {
                    appSST26Data.state = APP_SST26_STATE_XFER_ERROR;
                }
                break;

            case APP_SST26_STATE_XFER_SUCCESSFUL:
                LED_On();
                numOfSST26Test++;
                //SYS_CONSOLE_PRINT("\nSPI flash:Success", numOfSST26Test++);
                //isSSTTesting = false;
                appSST26Data.state = APP_SST26_STATE_RESET;
                break;

            case APP_SST26_STATE_XFER_ERROR:
                LED_Off();
                //SYS_CONSOLE_PRINT("\nSPI flash:Failed");
                SPISST26TestResults = false;
                appSST26Data.state = APP_SST26_STATE_RESET;
                break;
                
            default:
                break;
        }
    }
}
APP_SPI_SST26_DATA appSpiSst26Data;

void configureGPIO()
{
    /* GPIO pins configured as Input pin as per DTMFG*/
    //RPB7,RPB8,RPB9,RPA11,RPA13
    ANSELACLR = 0x2830;
    ANSELBCLR = 0x384;
    GPIO_PortInputEnable(GPIO_PORT_B, 0x04);// RPB2 as Input pin
    GPIO_PortInputEnable(GPIO_PORT_B, 0x80); // RPB7 as Input pin
    GPIO_PortInputEnable(GPIO_PORT_B, 0x100); // RPB8 as Input pin
    GPIO_PortInputEnable(GPIO_PORT_B, 0x200); // RPB9  as Input pin
    GPIO_PortInputEnable(GPIO_PORT_A, 0x800); //RPA11 as input pin
    GPIO_PortInputEnable(GPIO_PORT_A, 0x2000); // RAP13 as input pin
    GPIO_PortInputEnable(GPIO_PORT_A, 0x30); //RAPA4 and RPA5 as input pin
    /* GPIO pins configured as Output pin as per DTMFG*/
    //RPB1
    ANSELBCLR = 0x01;
    GPIO_PortOutputEnable(GPIO_PORT_B, 0x02); //RPB1 as output pin
}
void APP_SPI_SST26_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appSpiSst26Data.state = APP_SST26_STATE_INITIALIZE;
    LED_Off();
    configureGPIO(); // AS per the test fixture/DTMFG requirement. 
    if (!SYS_CMD_ADDGRP(builtinDTMFGCmdTbl, sizeof (builtinDTMFGCmdTbl) / sizeof (*builtinDTMFGCmdTbl), "DTMFGTest", ": Test to cover GPIO,Flash functions")) {
        SYS_CONSOLE_PRINT("DTMFG test Command Interface Failed\r\n");
    }
}

void APP_SPI_SST26_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appSpiSst26Data.state )
    {
        /* Application's initial state. */
        case APP_SST26_STATE_INITIALIZE:
        {
            APP_SST26_Initialize();
                /* Register a callback with the SPI PLIB and pass a pointer to the isCSDeAssert variable as the context */
            SPI1_CallbackRegister(APP_SST26_SPIEventHandler, (uintptr_t) &appSST26Data.isCSDeAssert);

            appSpiSst26Data.state = APP_SST26_STATE_IDLE;
            
            break;
        }

        case APP_SST26_STATE_IDLE:
        {
            SPIFlashTask();
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
