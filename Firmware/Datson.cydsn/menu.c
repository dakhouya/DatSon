/*! \file Menu.c 
  * \brief handle the menu generation
  */

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "menu.h" 

/*****************************************************************************
* Public functions
*****************************************************************************/
/*! Function that Initialize the menu structure
 *  \param menu pointer to menu structure
 */
void MenuInit(menu_t* menu){
    LCD_Start();
    LCD_ClearDisplay();
    LCD_DisplayOn();
    LCD_Position(0u, 0u);
    LCD_PrintString("Datson Started");
}

/*! Function that enable or disable the bluetooth logo
 *  \param menu pointer to menu structure
 *  \param enable status of the bluetooth connection
 */
void MenuBluetoothConnected(menu_t* menu, uint8_t enable){
    if(enable){
        LCD_Position(0u, 19u);
        LCD_PutChar(LCD_CUSTOM_0);
    }
    else{
        LCD_Position(0u, 19u);
        LCD_PutChar(' ');
    }
}

/* [] END OF FILE */
