/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>

#define     NORMAL             0
#define     WAKE_UP            1
#define     POWER_SAVING       2
#define     SLEEP              3


uint32_t millis = 0;
uint32_t lastMillis =0;


uint8_t connectionStatus = 1;
uint8_t i;
uint32_t buffer[3];

char serialBuffer[20];

void VHF_Set_Mode(uint8_t parameter);

CY_ISR(Timer_Count_Subroutine) {
    millis++;
    }

int main(void) {
    
    
    uint32_t start_awaiting;
    uint32_t sizeOfVhfBuffer;
    
    char vhfRxBuffer[20];
  
    
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    Timer_Count_Int_StartEx(Timer_Count_Subroutine);
   
    // Start Timer component
    Timer_1_Start();
    // Start VHF Radio UART component
    VHF_Radio_Start();
    // UART debug component
    UART_Start();
    
    // Set VHF Radio to Normal, please refer to the datasheet for more details. 
    VHF_Set_Mode(NORMAL);
    CyDelay(10);
    
    UART_UartPutString("E50 - TTL - 500  VHF 170Mhz 27dBm\n");
    UART_UartPutString("Test program\n");
    VHF_Radio_UartPutString("Test program\n");
  
    for(;;)
    {
        // Get VHF Radio buffer size
        sizeOfVhfBuffer = VHF_Radio_SpiUartGetRxBufferSize();
        // Check if data have been received
        if (sizeOfVhfBuffer != 0) {
            {
                uint8 i;
                // Save data
                for (i = 0; i < sizeOfVhfBuffer; i++){
                    vhfRxBuffer[i] = VHF_Radio_UartGetChar();
                    vhfRxBuffer[i+1] = '\0';
                }
            }
            // Debug
            //UART_UartPutString(vhfRxBuffer);
            connectionStatus = 1;
            OnBoard_Led_Write(0u);
        }
   
        
        // Save awaiting time
        start_awaiting = millis;
        // Those lines below check if connection have been lost
        // Only works when TX and RX send data each other at a periodic rate.
        
     
        while ( VHF_Aux_Pin_Read() && connectionStatus)  {
            // Start the timer 
            if (millis - start_awaiting > 5000) {
                // if AUX  Pin is still high after 2s set connection status to false 
                connectionStatus = 0;
                OnBoard_Led_Write(1u);
                // Debug
                UART_UartPutString("No connection\n");
            }
            if (!VHF_Aux_Pin_Read())
            {
             OnBoard_Led_Write(0u);
             break;
            }
        }
       
        
    }
}

/********************************************************************/
void VHF_Set_Mode(uint8_t parameter) {
    uint8_t mode;
    mode = parameter;
    
    switch(mode) {
        case 0:
            VHF_M1_Pin_Write(1u);
            VHF_M0_Pin_Write(1u);
            break;
        case 1:
            VHF_M1_Pin_Write(0u);
            VHF_M0_Pin_Write(1u);
            break;
        case 2:
            VHF_M1_Pin_Write(1u);
            VHF_M0_Pin_Write(0u);
            break;
        default:
            VHF_M1_Pin_Write(0u);
            VHF_M0_Pin_Write(0u);
            break;
    }
    
}
/********************************************************************/
