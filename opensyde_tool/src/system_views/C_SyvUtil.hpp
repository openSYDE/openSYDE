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
#include "stwtypes.hpp"
#include "C_NagToolTip.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
      eI_ERROR,
      eI_UPDATE_DISABLED,
      eI_UNKNOWN
   };

   enum E_NodeUpdateStatus
   {
      eU_UPDATING,
      eU_WAITING,
      eU_UP_TO_DATE,
      eU_UPDATE_SUCCESS,
      eU_UPDATE_DISABLED,
      eU_UNKNOWN
   };

   static bool h_GetViewStatusLabelInfo(const uint32_t ou32_ViewIndex, const int32_t os32_ViewSubMode,
                                        QString & orc_ErrorLabelHeadingText, QString & orc_ErrorLabelText,
                                        QString & orc_ErrorTooltipText,
                                        stw::opensyde_gui::C_NagToolTip::E_Type & ore_TooltipType,
                                        QString & orc_IconPath, int32_t & ors32_ColorId);
   static int32_t h_GetIndicesFromBusId(const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier,
                                        uint32_t & oru32_NodeIndex, uint32_t & oru32_BusIndex);
   static QString h_GetUpdateModeDescription(const uint32_t ou32_ViewIndex,
                                             const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   static QString h_GetCommonDashboardItemToolTip(const uint32_t ou32_ViewIndex,
                                                  const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id, const bool oq_ReadItem = true, const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode = stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::eWM_MANUAL);
   static void h_GetViewDisplayName(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode, QString & orc_SubMode,
                                    QString & orc_SubSubMode);

private:
   C_SyvUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
