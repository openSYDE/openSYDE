//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view utility class (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUTIL_H
#define C_SYVUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUtil
{
public:
   enum E_NodeUpdateInitialStatus
   {
      eI_APPLICATION_MATCH,
      eI_TO_BE_UPDATED,
      eI_NO_RESPONSE,
      eI_UPDATE_DISABLED,
      eI_UNKNOWN
   };

   static bool h_CheckViewSetupError(const stw_types::uint32 ou32_ViewIndex, QString & orc_ErrorLabelHeadingText,
                                     QString & orc_ErrorLabelText, QString & orc_ErrorTooltipText);
   static stw_types::sint32 h_GetIndicesFromBusId(const stw_types::uint8 ou8_BusIdentifier,
                                                  const stw_types::uint8 ou8_NodeIdentifier,
                                                  stw_types::uint32 & oru32_NodeIndex,
                                                  stw_types::uint32 & oru32_BusIndex);
   static QString h_GetUpdateModeDescription(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   static QString h_GetCommonDashboardItemToolTip(const stw_types::uint32 ou32_ViewIndex,
                                                  const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id, const bool oq_ReadItem = true, const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode = stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::eWM_MANUAL);
   static void h_GetViewDisplayName(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 os32_SubMode,
                                    QString & orc_SubMode, QString & orc_SubSubMode);

private:
   C_SyvUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
