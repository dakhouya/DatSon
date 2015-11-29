/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>
#include "bleconnection.h"
#include "SerialCommunicationService.h"

/*****************************************************************************
* Global variables
*****************************************************************************/
bleConnection_t bleConnection;
serialCommunicaitonService_t serialCommunicationService;

/*****************************************************************************
* Private prototypes
*****************************************************************************/
void StackEventHandler(uint32 event, void* eventParam);

/*****************************************************************************
* Main
*****************************************************************************/
int main()
{
    /*Init stack*/
    BleConnectionInit(&bleConnection, StackEventHandler);
    CyGlobalIntEnable; /* Enable global interrupts. */
    LCD_Start();
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_DisplayOn();
    LCD_Position(0u, 0u);
    LCD_PrintString("Horizontal BG");
    LCD_Position(1u, 0u);
    LCD_PrintString("Demo complete");
    PWM_1_Start();
    PWM_2_Start();
    for(;;)
    {
        /* Place your application code here. */
        CyBle_ProcessEvents();
    }
}

/*****************************************************************************
* Private functions
*****************************************************************************/
/*****************************************************************************
* Function Name: StackEventHandler()
******************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
*****************************************************************************/
void StackEventHandler(uint32 event, void* eventParam){
    CYBLE_GAP_SMP_KEY_DIST_T *keys;
    CYBLE_GAP_BD_ADDR_T addr;
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
            /* Initialize services*/
            SerialCommunicationInit(&serialCommunicationService);
            /* Put the device into discoverable mode so that remote peer can see it. */
            BleConnectionStartAdvertising(&bleConnection);
            break;
        
        /********************* GAP EVENTS **************************/
            
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /*Reset services*/
            SerialCommunicationServiceReset(&serialCommunicationService);
            /* Put the device into discoverable mode so that remote peer can see it. */
            BleConnectionStartAdvertising(&bleConnection);
            break;

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            /*To restart Advertisement when the advertisement stopped*/
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) {
                BleConnectionStartAdvertising(&bleConnection);
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:

            break;
            
            /*Authentification complete with or without error*/
            /*Error is handled internaly in BleConnection lib*/
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            BleConnectionFinalizeAuthentification(&bleConnection, eventParam);
            break;

        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            /*Get keys*/
            keys = (CYBLE_GAP_SMP_KEY_DIST_T *) eventParam;
            /*Will be used to create a private address*/
            BleConnectionSetIRK(&bleConnection, keys->irkInfo);
            break;
            
        /********************* GATT EVENTS *************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            /* Try to connect */
            CyBle_GapGetPeerBdAddr(((CYBLE_CONN_HANDLE_T*)eventParam)->bdHandle, &addr);
            if(!BleConnectionResolvePeerAddress(&bleConnection , addr)){
                CyBle_GapDisconnect(((CYBLE_CONN_HANDLE_T*)eventParam)->bdHandle);
            }
            break; 
            
                /*To handle custom service CCCD(Notify request) & Write request*/
        case CYBLE_EVT_GATTS_WRITE_REQ:
            
            /*Rx Characteristic Handler*/
            SerialCommunicationServiceCharacteristicHandler(&serialCommunicationService , eventParam);
            /*Tx Characteristic Handler*/
            SerialCommunicationServiceNotificationHandler(&serialCommunicationService, eventParam);
            
            /*Write to sender*/
            CyBle_GattsWriteRsp(cyBle_connHandle);
            break;
            
        default:
            break;
    }
}
/* [] END OF FILE */
