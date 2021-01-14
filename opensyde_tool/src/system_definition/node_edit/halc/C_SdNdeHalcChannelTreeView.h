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
#include "C_SdNdeHalcChannelTreeDelegate.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelTreeView(void);

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void UpdateChannelText(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void SelectChannel(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                      const bool oq_UseChannelIndex);
   void Clear(void);
   void ResetDelegate(void);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);
   void SigEditName(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   C_SdNdeHalcChannelTreeView(const C_SdNdeHalcChannelTreeView &);
   C_SdNdeHalcChannelTreeView & operator =(const C_SdNdeHalcChannelTreeView &);

   stw_opensyde_gui_logic::C_SdNdeHalcChannelTreeModel mc_Model;
   stw_opensyde_gui_logic::C_SdNdeHalcChannelTreeDelegate mc_Delegate;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_CopyAction;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnCopy(void) const;
   void m_OnPaste(void);
   void m_OnReset(void);
   void m_OnChannelSelected(const QModelIndex & orc_Index);
   void m_OnSelectionChanged(void) const;

   bool m_LoadUsChannel(stw_types::uint32 & oru32_DomainIndex, stw_types::uint32 & oru32_ChannelIndex,
                        bool & orq_ChannelCase) const;
   void m_SaveUsChannel(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
