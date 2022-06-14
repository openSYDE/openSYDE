//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export initialization module for CANopen

   See cpp file for detailed description

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTCANOPENINIT_H
#define C_OSCEXPORTCANOPENINIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLStringList.h"
#include "C_OSCNode.h"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportCanOpenInit
{
public:
   static stw_scl::C_SCLString h_GetFileName(void);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_FilePath, const C_OSCNode & orc_Node,
                                               const std::vector<stw_types::uint8> & orc_IFWithCanOpenMan,
                                               const stw_scl::C_SCLString & orc_ExportToolInfo = "");

protected:
   static void mh_ComposeDefineNumTotal(stw_scl::C_SCLString & orc_DefineValue,
                                        const std::vector<stw_types::uint8> & orc_IFWithCanOpenMan, const bool oq_IsTx,
                                        const stw_scl::C_SCLString & orc_Subject);
   static void mh_ComposeDefineNumDevices(stw_scl::C_SCLString & orc_DefineValue,
                                          const std::vector<stw_types::uint8> & orc_IFWithCanOpenMan);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
