/*! \file bleconnection.h  
  * \brief BLE connection management library
  */

/****************************************************************************/
#ifndef __BLECONNECTION_H__
#define __BLECONNECTION_H__
/****************************************************************************/
    
/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>
    
/*****************************************************************************
* Type Definition
*****************************************************************************/
/*Error types*/    
typedef enum{
    ERROR_BLE_CONNECTION_NONE,                         /*!< No error >!*/     
    ERROR_BLE_CONNECTION_INIT,                         /*!< Initialization error >!*/
    ERROR_BLE_CONNECTION_ADVERTISEMENT_START,          /*!< Advertisement start error >!*/    
    ERROR_BLE_CONNECTION_PAIRING,                      /*!< Pairing error >!*/
    ERROR_BLE_CONNECTION_IRK,                          /*!< No IRK error >!*/
    ERROR_BLE_PEER_BD_HANDLE,                          /*!< No peer BD handle error >!*/
    ERROR_BLE_CONNECTION_GET_BONDED_DEVICE_LIST,       /*!< Error getting bonded device list >!*/
    ERROR_BLE_CONNECTION_ERASE_BONDED_DEVICE,          /*!< Error while erasing bonded device list from ram>!*/     
    ERROR_BLE_CONNECTION_ERASE_BONDED_DEVICE_LIST,     /*!< Error while erasing bonded device list from flash>!*/   
    ERROR_BLE_CONNECTION_TIMEOUT                       /*!< Timeout error >!*/
}bleConnectionError_t;

/*Discovery modes*/
typedef enum{
    STOP_MODE,      /*!< Device is not discoverable >!*/
    PAIRING_MODE,   /*!< Device is in pairing mode >!*/
    LIMITED_MODE    /*!< Device is in limited mode >!*/
}discoveryMode_t;

/*Ble connection structure*/
typedef struct{
    discoveryMode_t mode;                                       /*!< Current pairing mode >!*/
    CYBLE_GAP_BONDED_DEV_ADDR_LIST_T gapBondedDeviceList;       /*!< Local bonded device list >!*/
    uint8_t connectionIrk[CYBLE_GAP_SMP_IRK_SIZE];              /*!< Local Identification request key >!*/ 
    uint8_t bondedDeviceBdHandle;                               /*!< Local Identification request key >!*/
    CYBLE_GAP_AUTH_FAILED_REASON_T LastAuthentificationError;   /*!< Last authentification error >!*/
    CYBLE_CONN_HANDLE_T* connectionHandle;                      /*!< Current connection handle >!*/
} bleConnection_t;
    
/*****************************************************************************
* Public functions
*****************************************************************************/
/*! Function that initialize the bleConnection structure
 *  and the main stack event handler.
 *  \param  ble pointer to the bleConnection_t structure 
 *  \param  StackEventHandler main stack handler  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionInit(bleConnection_t* ble, CYBLE_CALLBACK_T StackEventHandler);

/*! Function that start pairing mode
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionStartPairing(bleConnection_t* ble);

/*! Function that stop pairing mode
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionStopPairing(bleConnection_t* ble);

/*! Function to start advertisment
 *  \param  ble pointer to the bleConnection_t structure  
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionStartAdvertising(bleConnection_t* ble);

/*! Function to stop advertisment
 *  \param  ble pointer to the bleConnection_t structure  
 */
void BleConnectionStopAdvertising(bleConnection_t* ble);

/*! Function that will write to flash the bonded device list
 *  when authentification is done.
 *  \param  ble pointer to the bleConnection_t structure 
 *  \param  eventParam pointer to event parameter of the
 *  CYBLE_EVT_GAP_AUTH_COMPLETE state in the main event stack handler
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionFinalizeAuthentification(bleConnection_t* ble, void* eventParam);

/*! Function that will clear the bonded device list located in flash
 *  \param  ble pointer to the bleConnection_t structure 
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionClearBondedDeviceList(bleConnection_t* ble);

/*! Function that will update local irk.
 *  \param  ble pointer to the bleConnection_t structure 
 *  \param  irk pointer to irk array 
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionSetIRK(bleConnection_t* ble, uint8_t* irk);

/*! Function that will update local irk.
 *  \param  ble pointer to the bleConnection_t structure 
 *  \param  peerDbHandle stack connection handle
 *  \return bleConnectionError_t status
 */
bleConnectionError_t BleConnectionSetPeerBdHandle(bleConnection_t* ble, CYBLE_CONN_HANDLE_T* peerDbHandle);

/*! Function that will resolve remote peer with bonded device IRK
 *  \param  ble pointer to the bleConnection_t structure 
 *  \return bleConnectionError_t status
 */
uint8_t BleConnectionResolvePeerAddress(bleConnection_t* ble, CYBLE_GAP_BD_ADDR_T privateAddress);
#endif  /* __BLECONNECTION_H__ */

/* [] END OF FILE */
