//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "C_SdNdeHalcChannelTreeView.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelTreeView::C_SdNdeHalcChannelTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mpc_ContextMenu(new C_OgeContextMenu),
   mpc_CopyAction(NULL)
{
   this->C_SdNdeHalcChannelTreeView::setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setHeaderHidden(true);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   this->m_SetupContextMenu();

   connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged,
           this, &C_SdNdeHalcChannelTreeView::m_OnChannelSelected);

   connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
           this, &C_SdNdeHalcChannelTreeView::m_OnSelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of all elements and the Qt memory management
C_SdNdeHalcChannelTreeView::~C_SdNdeHalcChannelTreeView()
{
   delete this->mpc_ContextMenu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward node index to model.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::SetNode(const uint32_t ou32_NodeIndex)
{
   this->mc_Model.SetNode(ou32_NodeIndex);
   this->expandAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update texts of specified channel.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::UpdateChannelText(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex)
{
   this->mc_Model.UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
   this->mc_Model.CheckError(ou32_DomainIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select specified channel

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::SelectChannel(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                               const bool oq_UseChannelIndex)
{
   const QModelIndex c_Index = this->mc_Model.GetModelIndexFromIndexes(ou32_DomainIndex, ou32_ChannelIndex,
                                                                       oq_UseChannelIndex);

   this->setCurrentIndex(c_Index);
   this->expand(c_Index.parent());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear complete model.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::Clear(void)
{
   this->mc_Model.Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset delegate to update parent selection look.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::ResetDelegate(void)
{
   this->mc_Delegate.SetCurrentIndexParentRow(-1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::LoadUserSettings(void)
{
   uint32_t u32_DomainIndex;
   uint32_t u32_ChannelIndex;
   bool q_ChannelCase;

   // last known channel
   if (this->m_LoadUsChannel(u32_DomainIndex, u32_ChannelIndex, q_ChannelCase) == true)
   {
      this->SelectChannel(u32_DomainIndex, u32_ChannelIndex, q_ChannelCase);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::SaveUserSettings(void) const
{
   // last known channel
   this->m_SaveUsChannel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press release event slot

   Here: Handle F2 key.

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F2))
   {
      if (this->currentIndex().isValid() == true)
      {
         Q_EMIT (this->SigEditName());
      }
   }

   C_OgeTreeViewToolTipBase::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Draw the separator lines and background for selected items

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->viewport() != NULL)
   {
      QRect c_Rect;
      QPainter c_Painter(this->viewport());
      const QModelIndexList & rc_SelectedIndexes = this->selectionModel()->selectedIndexes();
      QModelIndexList::const_iterator c_ItSelectedIndex;
      QModelIndex c_Parent;
      QModelIndex c_LastIndex;

      c_Painter.setPen(Qt::transparent);

      // draw background of selected items on complete row width
      for (c_ItSelectedIndex = rc_SelectedIndexes.begin(); c_ItSelectedIndex != rc_SelectedIndexes.end();
           ++c_ItSelectedIndex)
      {
         const QModelIndex & rc_CurrentIndex = *c_ItSelectedIndex;

         c_Rect = this->visualRect(*c_ItSelectedIndex);
         c_Rect.setX(0);

         c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_10);
         if (this->mc_Model.hasChildren(rc_CurrentIndex) == false)
         {
            c_Painter.drawRect(c_Rect);
         }
      }

      // draw the special lines
      if (rc_SelectedIndexes.isEmpty() == false)
      {
         c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_7);

         // distinguish top and child item
         c_Parent = this->currentIndex().parent();
         if (c_Parent.isValid() == false)
         {
            // top level
            c_Parent = this->currentIndex();
            c_LastIndex = c_Parent;
         }
         else
         {
            // determine last index for bottom line
            if (this->isExpanded(c_Parent) == true)
            {
               c_LastIndex = this->mc_Model.index(this->mc_Model.rowCount(c_Parent) - 1, 0, c_Parent);
            }
            else
            {
               c_LastIndex = c_Parent;
            }
         }

         // draw the top line
         c_Rect = this->visualRect(c_Parent);
         c_Painter.drawRect(0, c_Rect.y(), this->width(), 2);

         // draw the bottom line
         c_Rect = this->visualRect(c_LastIndex);
         c_Painter.drawRect(0, c_Rect.y() + c_Rect.height(), this->width(), 2);
      }
   }

   C_OgeTreeViewToolTipBase::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_SetupContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeHalcChannelTreeView::customContextMenuRequested,
           this, &C_SdNdeHalcChannelTreeView::m_OnCustomContextMenuRequested);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"), this, &C_SdNdeHalcChannelTreeView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"), this,
                                    &C_SdNdeHalcChannelTreeView::collapseAll);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_CopyAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy Configuration"),
                                                           this, &C_SdNdeHalcChannelTreeView::m_OnCopy);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Apply Configuration"),
                                    this, &C_SdNdeHalcChannelTreeView::m_OnPaste);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Reset to Default"), this,
                                    &C_SdNdeHalcChannelTreeView::m_OnReset);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   if (this->indexAt(orc_Pos).isValid() == true)
   {
      const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

      // disable copy action if multiple indices are selected
      if (this->mpc_CopyAction != NULL)
      {
         if (this->selectedIndexes().size() > 1)
         {
            this->mpc_CopyAction->setEnabled(false);
         }
         else
         {
            this->mpc_CopyAction->setEnabled(true);
         }
      }

      // popup
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy action trigger.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnCopy(void) const
{
   this->mc_Model.Copy(this->currentIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paste action trigger.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnPaste(void)
{
   this->mc_Model.Paste(this, this->selectionModel()->selectedIndexes());

   // trigger reload of channel widget
   Q_EMIT (this->selectionModel()->currentRowChanged(this->currentIndex(), this->currentIndex()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset action trigger.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnReset(void)
{
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Reset configuration"));
   c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to reset the selected channel(s) to their "
                                                   "default configuration and lose your current settings? For linked "
                                                   "channel(s) the use case will be reset to default."));
   c_Message.SetOkButtonText(C_GtGetText::h_GetText("Reset"));
   c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
   if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
   {
      this->mc_Model.Reset(this->selectionModel()->selectedIndexes());

      // trigger reload of channel widget
      Q_EMIT (this->selectionModel()->currentRowChanged(this->currentIndex(), this->currentIndex()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel row selected signal.

   \param[in]  orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnChannelSelected(const QModelIndex & orc_Index)
{
   if ((orc_Index.isValid() == true) && (this->mc_Model.hasChildren(orc_Index) == false))
   {
      uint32_t u32_DomainIndex;
      uint32_t u32_ChannelIndex;
      bool q_ChannelCase;

      C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_Index, u32_DomainIndex, u32_ChannelIndex,
                                                              q_ChannelCase);

      // delegate needs to know parent row of current index
      if (orc_Index.parent().isValid() == true)
      {
         this->mc_Delegate.SetCurrentIndexParentRow(orc_Index.parent().row());
      }
      else
      {
         this->mc_Delegate.SetCurrentIndexParentRow(-1);
      }
      Q_EMIT (this->SigChannelSelected(u32_DomainIndex, u32_ChannelIndex, q_ChannelCase));
   }
   else
   {
      this->mc_Delegate.SetCurrentIndexParentRow(-1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of selection changed signal.

   Make sure current item stays selected.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnSelectionChanged(void) const
{
   this->selectionModel()->select(this->currentIndex(), QItemSelectionModel::Select);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load selected channel from user settings

   \param[out]  oru32_DomainIndex   Domain index
   \param[out]  oru32_ChannelIndex  Channel index
   \param[out]  orq_ChannelCase     Channel case

   \retval   true: found user setting
   \retval   false: not found; indices not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelTreeView::m_LoadUsChannel(uint32_t & oru32_DomainIndex, uint32_t & oru32_ChannelIndex,
                                                 bool & orq_ChannelCase) const
{
   bool q_Found = false;

   const uint32_t u32_NodeIndex = this->mc_Model.GetNodeIndex();
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

   // make sure they are initialized
   oru32_DomainIndex = 0;
   oru32_ChannelIndex = 0;
   orq_ChannelCase = true;

   if (pc_Node != NULL)
   {
      const C_UsNode c_UsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
      const QString c_Domain = c_UsNode.GetSelectedHalcDomainName();
      const QString c_ChannelId = c_UsNode.GetSelectedHalcChannel();

      if (c_Domain.isEmpty() == false)
      {
         // search domain name
         for (uint32_t u32_DomainIt = 0; u32_DomainIt < pc_Node->c_HalcConfig.GetDomainSize(); u32_DomainIt++)
         {
            const C_OscHalcDefDomain * const pc_Domain = pc_Node->c_HalcConfig.GetDomainDefDataConst(u32_DomainIt);

            if ((pc_Domain != NULL) && (pc_Domain->c_Name.c_str() == c_Domain))
            {
               oru32_DomainIndex = u32_DomainIt;

               if (c_ChannelId.isEmpty() == false)
               {
                  //search channel ID
                  for (uint32_t u32_ChannelIt = 0; u32_ChannelIt < pc_Domain->c_Channels.size(); u32_ChannelIt++)
                  {
                     const C_OscHalcDefChannelDef * const pc_Channel =
                        C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileChannelDataConst(u32_NodeIndex,
                                                                                           oru32_DomainIndex,
                                                                                           u32_ChannelIt);
                     if ((pc_Channel != NULL) && (pc_Channel->c_Name.c_str() == c_ChannelId))
                     {
                        oru32_ChannelIndex = u32_ChannelIt;
                        q_Found = true;
                     }
                  }
               }
               else
               {
                  // domain without channel case
                  q_Found = true;
                  orq_ChannelCase = false;
               }
            }
         }
      }
   }

   return q_Found;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save selected channel to user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_SaveUsChannel(void) const
{
   uint32_t u32_DomainIndex;
   uint32_t u32_ChannelIndex;
   bool q_ChannelCase;
   const uint32_t u32_NodeIndex = this->mc_Model.GetNodeIndex();

   C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(this->currentIndex(), u32_DomainIndex, u32_ChannelIndex,
                                                           q_ChannelCase);
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);

   if (pc_Node != NULL)
   {
      bool q_Reset = true;

      if (this->currentIndex().isValid() == true)
      {
         const C_OscHalcDefDomain * const pc_Domain = pc_Node->c_HalcConfig.GetDomainDefDataConst(u32_DomainIndex);
         if (pc_Domain != NULL)
         {
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedHalcDomain(pc_Node->c_Properties.c_Name.c_str(),
                                                                          pc_Domain->c_Name.c_str());
            if (q_ChannelCase == true)
            {
               const C_OscHalcDefChannelDef * const pc_Channel =
                  C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileChannelDataConst(u32_NodeIndex, u32_DomainIndex,
                                                                                     u32_ChannelIndex);

               if (pc_Channel != NULL)
               {
                  C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedHalcChannel(pc_Node->c_Properties.c_Name.c_str(),
                                                                                 pc_Channel->c_Name.c_str());
                  q_Reset = false;
               }
            }
            else
            {
               C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedHalcChannel(pc_Node->c_Properties.c_Name.c_str(), "");
               q_Reset = false;
            }
         }
      }

      // reset if none is selected
      if (q_Reset == true)
      {
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedHalcDomain(pc_Node->c_Properties.c_Name.c_str(), "");
         C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedHalcChannel(pc_Node->c_Properties.c_Name.c_str(), "");
      }
   }
}
