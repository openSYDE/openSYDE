//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELTREEVIEW_HPP
#define C_SDNDEHALCCHANNELTREEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_SdNdeHalcChannelTreeModel.hpp"
#include "C_SdNdeHalcChannelTreeDelegate.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelTreeView(void) override;

   void SetNode(const uint32_t ou32_NodeIndex);
   void UpdateChannelText(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void SelectChannel(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex);
   void Clear(void);
   void ResetDelegate(void);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);
   void SigEditName(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   C_SdNdeHalcChannelTreeView(const C_SdNdeHalcChannelTreeView &);
   C_SdNdeHalcChannelTreeView & operator =(const C_SdNdeHalcChannelTreeView &) &;

   stw::opensyde_gui_logic::C_SdNdeHalcChannelTreeModel mc_Model;
   stw::opensyde_gui_logic::C_SdNdeHalcChannelTreeDelegate mc_Delegate;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_CopyAction;

   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnCopy(void) const;
   void m_OnPaste(void);
   void m_OnReset(void);
   void m_OnChannelSelected(const QModelIndex & orc_Index);
   void m_OnSelectionChanged(void) const;

   bool m_LoadUsChannel(uint32_t & oru32_DomainIndex, uint32_t & oru32_ChannelIndex, bool & orq_ChannelCase) const;
   void m_SaveUsChannel(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
