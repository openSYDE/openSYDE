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

   static stw_types::sint32 h_ImportFile(const stw_types::uint32 ou32_BusIndex,
                                         const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                                         QWidget * const opc_Parent, std::vector<stw_types::uint32> & orc_NodeIndexes,
                                         std::vector<stw_types::uint32> & orc_InterfaceIndexes);
   static stw_types::sint32 h_ExportFile(
      const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CommDef, QWidget * const opc_Parent,
      const stw_types::uint32 ou32_NumOfNodes, const stw_types::uint32 ou32_NumOfMessages,
      const stw_types::uint32 ou32_NumOfSignals);
   static void h_AdaptName(stw_scl::C_SCLString & orc_Name, stw_scl::C_SCLString & orc_Comment);

private:
   static const QSize mhc_SIZE_REPORT;
   static stw_types::sint32 mh_ImportDBCFile(const stw_types::uint32 ou32_BusIndex,
                                             const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                                             const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                             std::vector<stw_types::uint32> & orc_NodeIndexes,
                                             std::vector<stw_types::uint32> & orc_InterfaceIndexes);
   static stw_types::sint32 mh_ImportDCFEDSFile(const stw_types::uint32 ou32_BusIndex,
                                                const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                                                const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                                std::vector<stw_types::uint32> & orc_NodeIndexes,
                                                std::vector<stw_types::uint32> & orc_InterfaceIndexes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
