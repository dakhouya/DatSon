/*! \file bleconnection.c  
  * \brief BLE connection management library
  */

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "bleconnection.h"

/*****************************************************************************
* Global variables
*****************************************************************************/
/* All zeros passed as  argument passed to CyBle_GapRemoveDeviceFromWhiteList for 
removing all the bonding data stored */
CYBLE_GAP_BD_ADDR_T clearAllDevices = {{0,0,0,0,0,0},0};

/*****************************************************************************
* Private prototypes
*****************************************************************************/
/*! Function that update the bonded device list in flash
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t _BleConnectionUpdateBondedDeviceList(bleConnection_t* ble);

/*! Function to start advertisment using a private address
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t _BleConnectionStartPrivateAdvertising(bleConnection_t* ble);

/*! Function to start advertisment using a private address
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t _BleConnectionStartPublicAdvertising(bleConnection_t* ble);
/*****************************************************************************
* Public functions
*****************************************************************************/
/*****************************************************************************
* Function Name: BleConnectionInit
******************************************************************************
*
* Summary:
*  Function that initialize the bleConnection structure
*  and the main stack event handler.
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*  stackEventHandler - main stack handler  
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionInit(bleConnection_t* ble, CYBLE_CALLBACK_T stackEventHandler){
    CYBLE_API_RESULT_T apiResult;
    bleConnectionError_t status = ERROR_BLE_CONNECTION_NONE;
    uint8_t bondedDeviceKeysFlag;
    CYBLE_GAP_BD_ADDR_T bondedDeviceAddress;
    CYBLE_GAP_SMP_KEY_DIST_T securityKeys;
    
    /*Ble stack*/
    apiResult = CyBle_Start(stackEventHandler);
    if(apiResult != CYBLE_ERROR_OK) {
        status = ERROR_BLE_CONNECTION_INIT;
    }

    /*Populate current bonded device list locally*/
    apiResult = CyBle_GapGetBondedDevicesList(&(ble->gapBondedDeviceList));
    if(apiResult != CYBLE_ERROR_OK){
        status = ERROR_BLE_CONNECTION_GET_BONDED_DEVICE_LIST;
    }
    
    /* Check if we have bounded device*/
    if(ble->gapBondedDeviceList.count != 0){
        
        /* Get last bonded device */
        memcpy(bondedDeviceAddress.bdAddr, ble->gapBondedDeviceList.bdAddrList[0].bdAddr, 6);
        bondedDeviceAddress.type = ble->gapBondedDeviceList.bdAddrList[0].type;
        
        /* Get BD handle */
        CyBle_GapGetPeerBdHandle(&ble->bondedDeviceBdHandle, &bondedDeviceAddress);
        
        /* Get identification request key */
        bondedDeviceKeysFlag = 0x01;
        CyBle_GapGetPeerDevSecurityKeyInfo(ble->bondedDeviceBdHandle, &bondedDeviceKeysFlag, &securityKeys); 
        memcpy(ble->connectionIrk, securityKeys.irkInfo, CYBLE_GAP_SMP_IRK_SIZE);
    }

    /*Clear auth error*/
    ble->LastAuthentificationError = CYBLE_GAP_AUTH_ERROR_NONE;
    ble->mode = LIMITED_MODE;
    //BleConnectionStartAdvertising(ble);
    return status;
}

/*****************************************************************************
* Function Name: BleConnectionStartPairing
******************************************************************************
*
* Summary:
*  Function that start pairing mode
*
* Parameters:
*  ble - pointer to the bleConnection_t structure  
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionStartPairing(bleConnection_t* ble){
    bleConnectionError_t status = ERROR_BLE_CONNECTION_NONE;

    if(ble->mode != PAIRING_MODE) {
        ble->mode = PAIRING_MODE;
    }
    return status;
}

/*****************************************************************************
* Function Name: BleConnectionStopPairing
******************************************************************************
*
* Summary:
*  Function that stop pairing mode
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionStopPairing(bleConnection_t* ble){
    bleConnectionError_t status = ERROR_BLE_CONNECTION_NONE;

    if(ble->mode != LIMITED_MODE) {
        ble->mode = LIMITED_MODE;
    }
    return status;
}

/*****************************************************************************
* Function Name: BleConnectionStartAdvertising
******************************************************************************
*
* Summary:
*  Function to start advertisment
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionStartAdvertising(bleConnection_t* ble){
    bleConnectionError_t status = ERROR_BLE_CONNECTION_NONE;
    CYBLE_API_RESULT_T apiResult;
    uint16_t timeout = 0xFFFF;
    
    /*Start advertisment*/
    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
    do{
        CyBle_ProcessEvents();
    }while(CyBle_GetState() == CYBLE_STACK_STATE_BUSY && timeout--);
    
    /* Error handling */
    if(timeout == 0){
        status = ERROR_BLE_CONNECTION_TIMEOUT;
    }
    if(apiResult != CYBLE_ERROR_OK){
        status = ERROR_BLE_CONNECTION_ADVERTISEMENT_START;
    }
    
    return status;
}



/*****************************************************************************
* Function Name: BleConnectionStopAdvertising
******************************************************************************
*
* Summary:
*  Function to stop advertisment
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
void BleConnectionStopAdvertising(bleConnection_t* ble){
    /*Shutdown ble discovery*/
    CyBle_GappStopAdvertisement();
    //ble->mode = STOP_MODE;
}

/*****************************************************************************
* Function Name: BleConnectionStopAdvertising
******************************************************************************
*
* Summary:
*  Function that will write to flash the bonded device list
*  when authentification is done.
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*  eventParam - pointer to event parameter of the
*  CYBLE_EVT_GAP_AUTH_COMPLETE state in the main event stack handler
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionFinalizeAuthentification(bleConnection_t* ble, void* eventParam){
    bleConnectionError_t status = ERROR_BLE_CONNECTION_NONE;
    CYBLE_GAP_AUTH_FAILED_REASON_T eventResult = *(CYBLE_GAP_AUTH_FAILED_REASON_T*)eventParam;
    /*should check that eventResult == CYBLE_GAP_AUTH_ERROR_NONE*/
    /*but alway get an error from the stack in our pairing flow*/
    status = _BleConnectionUpdateBondedDeviceList(ble);
    return status;
}

/*****************************************************************************
* Function Name: BleConnectionClearBondedDeviceList
******************************************************************************
*
* Summary:
*  Function that will clear the bonded device list located in flash
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionClearBondedDeviceList(bleConnection_t* ble){
    CYBLE_API_RESULT_T apiResult;
    uint16_t timeout = 0xFFFF;
    uint8_t i;
    
    /*Clear all device in the bonded device list*/
    apiResult = CyBle_GapRemoveOldestDeviceFromBondedList();
    if(apiResult != CYBLE_ERROR_OK){
        return ERROR_BLE_CONNECTION_ERASE_BONDED_DEVICE_LIST;
    }
    else{
        /*Update structure*/
        apiResult = CyBle_GapGetBondedDevicesList(&(ble->gapBondedDeviceList)); 
        ble->bondedDeviceBdHandle = 0;
        for(i = 0; i < CYBLE_SMP_IRK_SIZE; i++){
            ble->connectionIrk[i] = 0;
        }
        if(apiResult != CYBLE_ERROR_OK){
            return ERROR_BLE_CONNECTION_GET_BONDED_DEVICE_LIST;
        }
    }
    
    /*Update flash data*/
    while((CyBle_StoreBondingData(1) != CYBLE_ERROR_OK) && (timeout--));
    /*Timeout check*/
    if(timeout == 0x0000){
        return ERROR_BLE_CONNECTION_TIMEOUT;
    }
    else{
        return ERROR_BLE_CONNECTION_NONE;
    }
}

/*****************************************************************************
* Function Name: BleConnectionSetIRK
******************************************************************************
*
* Summary:
*  Function that will update local irk.
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*  irk - pointer to irk array 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionSetIRK(bleConnection_t* ble, uint8_t* irk){
    if(irk != 0){
        memcpy(&ble->connectionIrk, irk, CYBLE_GAP_SMP_IRK_SIZE);
        //ble->advertisingMode = PRIVATE_MODE;
        return ERROR_BLE_CONNECTION_NONE;
    }
    else{
        return ERROR_BLE_CONNECTION_IRK;
    }
}

/*****************************************************************************
* Function Name: BleConnectionPeer
******************************************************************************
*
* Summary:
*  Function that will update local irk.
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*  peerBdHandle - stack connection handle
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t BleConnectionSetPeerBdHandle(bleConnection_t* ble, CYBLE_CONN_HANDLE_T* peerBdHandle){
    if(peerBdHandle != 0){
        //ble->connectionHandle = peerBdHandle;    
        return ERROR_BLE_CONNECTION_NONE;
    }
    else{
        return ERROR_BLE_PEER_BD_HANDLE;
    }
}

/*****************************************************************************
* Function Name: BleConnectionResolvePeerAddress
******************************************************************************
*
* Summary:
*  Function that will update local irk.
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*  peerBdHandle - stack connection handle
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
uint8_t BleConnectionResolvePeerAddress(bleConnection_t* ble, CYBLE_GAP_BD_ADDR_T privateAddress){
    uint8_t bondedDeviceKeysFlag;
    CYBLE_GAP_BD_ADDR_T bondedDeviceAddress;
    CYBLE_GAP_SMP_KEY_DIST_T securityKeys;
    uint8_t prand[16] = {0}; //prand with 0 padding
    uint8_t hash128Bits[16];
    uint8_t hash24Bits[4] = {0}; //24bits + \0
    uint8_t remoteHash24Bits[4] = {0}; //24bits + \0
    uint8_t i;
    
    if(ble->mode == LIMITED_MODE){
        /* Get prand part of BT address. Last 24bits of the private address*/
        /* Get hash part of BT address. First 24bits of the private address*/
        for(i = 0; i < 3; i++){
            prand[i] = privateAddress.bdAddr[i+3];
            remoteHash24Bits[i] = privateAddress.bdAddr[i];
        }
        
        /* Get 128 bits hash*/
        CyBle_AesEncrypt(prand, ble->connectionIrk, hash128Bits); 
        for(i = 0; i < 3; i++){
            hash24Bits[i] = hash128Bits[i];
        }
        
        /* Compare */
        if(strcmp((const char*)hash24Bits, (const char*)remoteHash24Bits) == 0){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 1;
    }
}

/*****************************************************************************
* Private functions
*****************************************************************************/
/*****************************************************************************
* Function Name: _BleConnectionUpdateBondedDeviceList
******************************************************************************
*
* Summary:
*  Function that will update the bonded device list located in flash
*
* Parameters:
*  ble - pointer to the bleConnection_t structure 
*
* Return:
*  bleConnectionError_t
*
*****************************************************************************/
bleConnectionError_t _BleConnectionUpdateBondedDeviceList(bleConnection_t* ble){
    CYBLE_API_RESULT_T apiResult;
    uint16_t timeout = 0xFFFF;
    
    /*Update device list*/
    apiResult = CyBle_GapGetBondedDevicesList(&(ble->gapBondedDeviceList));
    if(apiResult != CYBLE_ERROR_OK){
        return ERROR_BLE_CONNECTION_GET_BONDED_DEVICE_LIST;
    }
    
    /*Be sure that there's only 1 device in the bonded device list*/
    if(ble->gapBondedDeviceList.count > 1){
        apiResult = CyBle_GapRemoveOldestDeviceFromBondedList();
        if(apiResult != CYBLE_ERROR_OK){
            return ERROR_BLE_CONNECTION_ERASE_BONDED_DEVICE;
        }
        /*Update structure device list*/
        apiResult = CyBle_GapGetBondedDevicesList(&(ble->gapBondedDeviceList)); 
        if(apiResult != CYBLE_ERROR_OK){
            return ERROR_BLE_CONNECTION_GET_BONDED_DEVICE_LIST;
        }
    }

    while((CyBle_StoreBondingData(1) != CYBLE_ERROR_OK) && (timeout--));
    /*Timeout check*/
    if(timeout == 0x0000){
        return ERROR_BLE_CONNECTION_TIMEOUT;
    }
    else{
        return ERROR_BLE_CONNECTION_NONE;
    }
}

/* [] END OF FILE */
