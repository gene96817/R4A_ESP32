/**********************************************************************
  NVM_Menu.cpp

  Robots-For-All (R4A)
  Menu for non-volatile memory (NVM) support
**********************************************************************/

#include "R4A_ESP32.h"

//****************************************
// NVM Menu
//****************************************

const R4A_MENU_ENTRY r4aEsp32NvmMenuTable[] =
{
    // Command  menuRoutine                     menuParam           HelpRoutine         align   HelpText
    {"cat",     r4aEsp32NvmMenuFileCat,         (intptr_t)"ffff",   r4aMenuHelpSuffix,  4,      "Display the contents of file ffff"},   //  0
    {"cp",      r4aEsp32NvmMenuFileCopy,      (intptr_t)"src dest", r4aMenuHelpSuffix,  8,      "Copy src file to dest file"},          //  1
    {"d",     r4aEsp32NvmMenuDisplayParameters, 0,          nullptr,                    0,      "Display NVM parameters"},              //  2
    {"f",     r4aEsp32NvmMenuDumpParameterFile, 0,          nullptr,                    0,      "Dump the parameter file"},             //  3
    {"g",  r4aEsp32NvmMenuGetDefaultParameters, 0,          nullptr,                    0,      "Get default parameters"},              //  4
    {"p",      r4aEsp32NvmMenuParameterDisplay, (intptr_t)"pppp",   r4aMenuHelpSuffix,  4,      "Display parameter pppp value"},        //  5
    {"s",       r4aEsp32NvmMenuParameterSet, (intptr_t)"pppp ssss", r4aMenuHelpSuffix,  9,      "Set parameter pppp value to xxxx"},    //  6
    {"w",    r4aEsp32NvmMenuWriteParameterFile, 0,          nullptr,                    0,      "Write the parameter file"},            //  7
    {"x",       nullptr,                     R4A_MENU_MAIN, nullptr,                    0,      "Exit the menu system"},                //  8
    {"z",       r4aEsp32NvmMenuParameterClear,  (intptr_t)"pppp",   r4aMenuHelpSuffix,  4,      "Zero the parameter"},                  //  9
};                                                                                                                                      // 10
