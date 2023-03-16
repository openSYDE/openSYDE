//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGTREEVIEW_H
#define C_SDNDEHALCCONFIGTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_SdNdeHalcConfigTreeModel.hpp"
#include "C_SdNdeHalcConfigTreeDelegate.hpp"
#include "C_OgePubIconOnly.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdNdeHalcConfigTreeView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcConfigTreeView(void) override;

   void SetNode(const uint32_t ou32_NodeIndex);
   void SetHalcChannelUseCase(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                              const uint32_t ou32_UseCaseIndex, const bool oq_UseChannelIndex);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

private:
   stw::opensyde_gui_logic::C_SdNdeHalcConfigTreeDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_SdNdeHalcConfigTreeModel mc_Model;
   stw::opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;

   void m_InitColumnWidths(void);

   //Avoid call
   C_SdNdeHalcConfigTreeView(const C_SdNdeHalcConfigTreeView &);
   C_SdNdeHalcConfigTreeView & operator =(const C_SdNdeHalcConfigTreeView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
