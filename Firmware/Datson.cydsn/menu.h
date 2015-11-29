/*! \file Menu.h  
  * \brief handle the menu generation
  */

/****************************************************************************/
#ifndef __MENU_H__
#define __MENU_H__
/****************************************************************************/

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>
    
/*****************************************************************************
* Constant
*****************************************************************************/
#define MENU_BLUETOOTH_ENABLE  (1)
#define MENU_BLUETOOTH_DISABLE (0)
/*****************************************************************************
* Type definition 
*****************************************************************************/  

typedef struct menu
{     
    uint8_t a;
}menu_t;
/*****************************************************************************
* Public prototypes
*****************************************************************************/
/*! Function that Initialize the menu structure
 *  \param menu pointer to menu structure
 */
void MenuInit(menu_t* menu);

/*! Function that enable or disable the bluetooth logo
 *  \param menu pointer to menu structure
 *  \param enable status of the bluetooth connection
 */
void MenuBluetoothConnected(menu_t* menu, uint8_t enable);


#endif /* __HEXOSKINCOMMANDSERVICE_H__ */   

/* [] END OF FILE */
