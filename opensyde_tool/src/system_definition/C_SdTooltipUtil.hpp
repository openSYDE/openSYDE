//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to centralize tooltip handling (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOOLTIPUTIL_HPP
#define C_SDTOOLTIPUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_OscCanMessage.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTooltipUtil
{
public:
   static QString h_GetToolTipContentMessage(const stw::opensyde_core::C_OscCanMessage & orc_Message);
   static QString h_GetToolTipContentSignal(const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                            const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                            const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_DpListElement,
                                            const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                            const QString & orc_AutoMinMaxInformation = "",
                                            const QString & orc_AdditionalInformation = "");
   static QString h_ConvertTypeToNameSimplified(const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type & ore_Type);

private:
   C_SdTooltipUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
