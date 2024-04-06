#include "BootUtility.h"

void JumpToUserApp()
{
    /* Set location of Vector Table for user app */
    SetVTOR(USER_APP_VT_ADDRESS - BOOTLOADER_VT_ADDRESS);

    /* Set new location for Main Stack Pointer */
    uint32_t user_sp = *((uint32_t *)USER_APP_VT_ADDRESS); // First 4 bytes from the Vector Table represent the Stack Pointer
    SetStackPointer(user_sp);

    /* Jump to the reset handler of the user app */
    void (*fp_handler)(void) = (void (*)(void))USER_APP_RESET_HANDLER_ADDRESS; 
    fp_handler();
}