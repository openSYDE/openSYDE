//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the update package configuration (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACCONFIG_H
#define C_SYVUPPACCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <vector>

#include "C_PuiSvNodeUpdate.h"
#include "C_OSCNodeApplication.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacConfigNodeApp
{
public:
   C_SyvUpPacConfigNodeApp();

   stw_opensyde_core::C_OSCNodeApplication::E_Type e_Type;
   QString c_Name;
   QString c_Path;
};

class C_SyvUpPacConfigNode
{
public:
   C_SyvUpPacConfigNode();

   QString c_Name;       // Node name as identificator
   QString c_DeviceType; // Device type as identificator

   std::vector<C_SyvUpPacConfigNodeApp> c_AppConfigs;
   std::vector<QString> c_ParamSetConfigs;
   std::vector<QString> c_FileConfigs;

   QString c_PemFilePath;

   C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity;
   C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger;
};

class C_SyvUpPacConfig
{
public:
   C_SyvUpPacConfig();

   std::vector<C_SyvUpPacConfigNode> c_NodeConfigs;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
