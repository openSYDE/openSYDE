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
#include "stwtypes.hpp"

#include "C_OgeTreeViewToolTipBase.hpp"

#include "C_SdNdeHalcConfigImportModel.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OscHalcConfigStandalone.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdNdeHalcConfigImportView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeHalcConfigImportView(void);

   int32_t Init(const stw::opensyde_core::C_OscHalcConfig & orc_Config,
                const stw::opensyde_core::C_OscHalcConfigStandalone & orc_ImportConfig);
   void GetAdaptedConfiguration(stw::opensyde_core::C_OscHalcConfig & orc_AdaptedConfig);
   bool IsSelectionOfLinkedChannelsValid(std::vector<uint32_t> & orc_DomainIndices,
                                         std::vector<std::vector<uint32_t> > & orc_MissingChannelIndices);
   void CheckChannels(const std::vector<uint32_t> & orc_DomainIndices,
                      const std::vector<std::vector<uint32_t> > & orc_ChannelIndices);

private:
   stw::opensyde_gui_logic::C_SdNdeHalcConfigImportModel mc_Model;
   stw::opensyde_gui_elements::C_OgeTreeViewToolTipBaseDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
