/*****************************************************************************
* Included headers
*****************************************************************************/
#include "SerialCommunicationService.h"

/*******************************************************************************
* Constants
*******************************************************************************/
#define MAXIMUM_PACKET_COUNT_PER_CONNECTION_INTERVAL (4) //4 for android OS, source: forums...
/*Characteristic handle definitions*/
#define SERIAL_COMMUNICATION_SERVICE_RX_HANDLE  (cyBle_customs[CYBLE_SERIAL_PORT_SERVICE_INDEX]).customServiceInfo[CYBLE_SERIAL_PORT_RX_CHAR_INDEX].customServiceCharHandle
#define TX_CHARACTERISTIC_HANDLE (cyBle_customs[CYBLE_SERIAL_PORT_SERVICE_INDEX])  \
                                .customServiceInfo[CYBLE_SERIAL_PORT_TX_CHAR_INDEX]\
                                .customServiceCharDescriptors[CYBLE_SERIAL_PORT_TX_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX])

/*******************************************************************************
* Public Function
*******************************************************************************/
/*******************************************************************************
* Function Name: SerialCommunicationInit
********************************************************************************
*
* Summary:
*  Initialize serial communication service structure
*
* Parameters:
*  serial - Serial communication service structure
*
* Return:
*  None
*
*******************************************************************************/
void SerialCommunicationInit(serialCommunicaitonService_t* serial){

}

/*******************************************************************************
* Function Name: SerialCommunicationServiceCharacteristicHandler
********************************************************************************
*
* Summary:
*  Handle serial communication service characteristics
*
* Parameters:
*  serial - Serial communication service structure
*  eventParam - Event parameter from main event handler
*
* Return:
*  int16_t status
*
*******************************************************************************/
int16_t SerialCommunicationServiceCharacteristicHandler(serialCommunicaitonService_t* serial, void* eventParam){
    int16_t status = 0;
    /*Received handle to characteristic*/
    CYBLE_GATT_DB_ATTR_HANDLE_T handle = ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle;

    /* Send command*/
    if(handle == SERIAL_COMMUNICATION_SERVICE_RX_HANDLE){
        /*Should load data in a buffer*/
    /*CommandManagerSendPassthroughtCommand(command->hexoskinManager, 
                                        ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.val,
                                        ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.len);*/  
    }
    else{
        status = -1;
    }
    return status;
}

/*******************************************************************************
* Function Name: SerialCommunicationServiceTxNotificationCCCD
********************************************************************************
*
* Summary:
*  Function that send a tx notification
*
* Parameters:
*  serial - Serial communication service structure
*  dataLength - length of the data to send. Between ]0, DATA_PACKET_SIZE]
*
* Return:
*  int16_t - status
*
*******************************************************************************/
int16_t SerialCommunicationServiceTxNotificationCCCD(serialCommunicaitonService_t* serial, uint8_t dataLength) {
    CYBLE_GATTS_HANDLE_VALUE_NTF_T		notificationHandle;
    int16_t status = 0;
    
    if((serial->txNotify == SERIAL_COMMUNICAITON_NOTIFICATION_ENABLE) && (CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_FREE)){
 
        /* Update notification handle data*/
    	notificationHandle.attrHandle = CYBLE_SERIAL_PORT_TX_CHAR_HANDLE;
    	notificationHandle.value.val = serial->txPacket;
    	notificationHandle.value.len = dataLength;
        
    	/* Send notifications. */
    	if(CyBle_GattsNotification(cyBle_connHandle, &notificationHandle) != CYBLE_ERROR_OK){
            status = -1;
        }
    }
    return status;
}

/*******************************************************************************
* Function Name: SerialCommunicationServiceNotificationHandler
********************************************************************************
*
* Summary:
*  Function that handle the CCCD flag. Notification enable or disable
*
* Parameters:
*  serial - Serial communication service structure
*  eventParam - pointer to event parameter from main event handler
*
* Return:
*  int16_t - status
*
*******************************************************************************/
int16_t SerialCommunicationServiceNotificationHandler(serialCommunicaitonService_t* serial, void* eventParam){
    int16_t status = 0;
    
    /*Handle notificaiton flag*/
    if(((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle == TX_CHARACTERISTIC_HANDLE{
        if(((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.val[0] == 0x01){
            serial->txNotify = SERIAL_COMMUNICAITON_NOTIFICATION_ENABLE;
        } else {
            serial->txNotify = SERIAL_COMMUNICAITON_NOTIFICATION_DISABLE;
        }
    } 
    return 0;
}

/*****************************************************************************
* Function Name: SerialCommunicationServiceServiceReset
******************************************************************************
*
* Summary:
*  Reset the notification flag. Should be used when there's a ble disconnection.
*
* Parameters:
*  serial - Serial communication service structure
*
* Return:
*  None
*
*****************************************************************************/
void SerialCommunicationServiceReset(serialCommunicaitonService_t* serial){
    uint16_t i;
        
    serial->txNotify = SERIAL_COMMUNICAITON_NOTIFICATION_DISABLE;
    for(i = 0; i < SERIAL_COMMUNICATION_DATA_PACKET_SIZE; i++){
        serial->txPacket[i] = 0;
    }
}

/* [] END OF FILE */
