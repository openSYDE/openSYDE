//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELTREEVIEW_H
#define C_SDNDEHALCCHANNELTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdNdeHalcChannelTreeModel.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
public:
   C_SdNdeHalcChannelTreeView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelTreeView(void);

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void UpdateChannelText(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void Clear(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   C_SdNdeHalcChannelTreeView(const C_SdNdeHalcChannelTreeView &);
   C_SdNdeHalcChannelTreeView & operator =(const C_SdNdeHalcChannelTreeView &);

   stw_opensyde_gui_logic::C_SdNdeHalcChannelTreeModel mc_Model;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnCopy(void) const;
   void m_OnPaste(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
