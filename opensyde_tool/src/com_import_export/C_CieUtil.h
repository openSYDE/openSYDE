//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class with functions for comm import export (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEUTIL_H
#define C_CIEUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_CieConverter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieUtil
{
public:
   C_CieUtil(void);

   static stw_types::sint32 h_ImportFile(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                                         const stw_types::sint32 os32_DataPoolIndex,
                                         const stw_types::uint32 ou32_InterfaceIndex, QWidget * const opc_Parent);
   static stw_types::sint32 h_ExportFile(
      const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CommDef, QWidget * const opc_Parent,
      const stw_types::uint32 ou32_NumOfNodes, const stw_types::uint32 ou32_NumOfMessages,
      const stw_types::uint32 ou32_NumOfSignals);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
