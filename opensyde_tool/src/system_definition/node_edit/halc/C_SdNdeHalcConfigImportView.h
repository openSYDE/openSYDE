//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC configuration import items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGIMPORTVIEW_H
#define C_SDNDEHALCCONFIGIMPORTVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_OgeTreeViewToolTipBase.h"

#include "C_SdNdeHalcConfigImportModel.h"
#include "C_OSCHalcConfig.h"
#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdNdeHalcConfigImportView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcConfigImportView(void);

   stw_types::sint32 Init(const stw_opensyde_core::C_OSCHalcConfig & orc_Config,
                          const stw_opensyde_core::C_OSCHalcConfigStandalone & orc_ImportConfig);
   void GetAdaptedConfiguration(stw_opensyde_core::C_OSCHalcConfig & orc_AdaptedConfig);
   bool IsSelectionOfLinkedChannelsValid(std::vector<stw_types::uint32> & orc_DomainIndices,
                                         std::vector<std::vector<stw_types::uint32> > & orc_MissingChannelIndices);
   void CheckChannels(const std::vector<stw_types::uint32> & orc_DomainIndices,
                      const std::vector<std::vector<stw_types::uint32> > & orc_ChannelIndices);

private:
   stw_opensyde_gui_logic::C_SdNdeHalcConfigImportModel mc_Model;
   stw_opensyde_gui_elements::C_OgeTreeViewToolTipBaseDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
