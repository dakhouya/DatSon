/*! \file SerialCommunicationService.h  
  * \brief serial communication interface for ble
  */

/****************************************************************************/
#ifndef __SERIALCOMMUNICATIONSERVICE_H__
#define __SERIALCOMMUNICATIONSERVICE_H__
/****************************************************************************/

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>
    
/*****************************************************************************
* Constant
*****************************************************************************/
#define SERIAL_COMMUNICATION_DATA_PACKET_SIZE (20)   /*!< Data packet size >!*/
#define SERIAL_COMMUNICAITON_NOTIFICATION_ENABLE  (1)
#define SERIAL_COMMUNICAITON_NOTIFICATION_DISABLE (0)
    
/*****************************************************************************
* Type definition 
*****************************************************************************/  
    
typedef struct serialCommunicaitonService
{     
    uint8_t txPacket[SERIAL_COMMUNICATION_DATA_PACKET_SIZE]; /*!< tx buffer >!*/
    uint8_t txNotify;                                        /*!< tx notification flag >!*/
}serialCommunicaitonService_t;
/*****************************************************************************
* Public functions
*****************************************************************************/
/*! Function that Initialize serial communication service structure
 *  \param serial pointer to serialCommunicaitonService_t structure
 */
void SerialCommunicationInit(serialCommunicaitonService_t* serial);

/*! Function that handle serial communication Rx characteristics
 *  \param serial pointer to serialCommunicaitonService_t structure
 *  \param eventParam event parameter from main event handler
 *  \return int16_t status
 */
int16_t SerialCommunicationServiceCharacteristicHandler(serialCommunicaitonService_t* serial, void* eventParam);

/*! Function that send a tx notification
 *  \param serial pointer to serialCommunicaitonService_t structure
 *  \param dataLength lenght of the data to send. Between ]0, DATA_PACKET_SIZE]
 *  \return int16_t status
 */
int16_t SerialCommunicationServiceTxNotificationCCCD(serialCommunicaitonService_t* serial, uint8_t dataLength);

/*! Function that handle the CCCD flag. Notification enable or disable
 *  \param serial pointer to serialCommunicaitonService_t structure
 *  \param eventParam pointer to event parameter
 *  \return int16_t status
 */
int16_t SerialCommunicationServiceNotificationHandler(serialCommunicaitonService_t* serial, void* eventParam);

/*! Reset the notification flag. Should be used when there's a ble disconnection.
 *  \param serial pointer to serialCommunicaitonService_t structure
 */
void SerialCommunicationServiceReset(serialCommunicaitonService_t* serial);

#endif /* __SERIALCOMMUNICATIONSERVICE_H__ */   

/* [] END OF FILE */
