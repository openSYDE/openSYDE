//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system definition (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDUTIL_H
#define C_PUISDUTIL_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include <QString>
#include "CSCLString.h"
#include "stwtypes.h"
#include "C_OSCSystemBus.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolListElementId.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdUtil
{
public:
   static bool h_CheckNameAvailable(const std::vector<const QString *> & orc_ExistingStrings,
                                    const QString & orc_Proposal, const stw_types::uint32 * const opu32_SkipIndex);
   static QString h_ConvertBusTypeToStringUppercase(const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_Type);
   static QString h_ConvertProtocolTypeToString(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Type);
   static QString h_ConvertProtocolTypeToDatapoolNameString(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Type);
   static QString h_ConvertDataPoolTypeToString(const stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type);
   static QString h_GetInterfaceName(const stw_opensyde_core::C_OSCSystemBus::E_Type oe_Type,
                                     const stw_types::uint8 ou8_InterfaceNumber);
   static stw_types::sint32 h_ConvertIndex(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ElementID,
                                           stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageID,
                                           stw_types::uint32 & oru32_SignalIndex);
   static stw_types::sint32 h_ConvertFromSignalIndex(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageID,
      const stw_types::uint32 ou32_SignalIndex, stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ElementID);

private:
   C_PuiSdUtil(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
