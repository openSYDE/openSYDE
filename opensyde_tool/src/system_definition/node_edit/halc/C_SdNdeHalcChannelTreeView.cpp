//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcChannelTreeView.h"
#include "C_OgeContextMenu.h"
#include "C_GtGetText.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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
   mpc_ContextMenu(new C_OgeContextMenu)
{
   this->setModel(&this->mc_Model);
   this->setHeaderHidden(true);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   this->m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelTreeView::~C_SdNdeHalcChannelTreeView()
{
   delete this->mpc_ContextMenu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward node index to model.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNode(ou32_NodeIndex);
   this->expandAll(); // TODO save and restore expanded domains in user settings
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update texts of specified channel.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::UpdateChannelText(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex)
{
   this->mc_Model.UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
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
/*! \brief   Overridden key press release event slot

   Here: Handle copy paste.

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOriginal = true;

   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_Insert:
      case Qt::Key_C:
         // copy
         this->m_OnCopy();
         // do not call original as it copies tree item text
         q_CallOriginal = false;
         opc_KeyEvent->accept();
         break;
      case Qt::Key_V:
         // paste
         this->m_OnPaste();
         break;
      default:
         break;
      }
   }

   if (q_CallOriginal == true)
   {
      C_OgeTreeViewToolTipBase::keyPressEvent(opc_KeyEvent);
   }
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

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"), this, &C_SdNdeHalcChannelTreeView::m_OnCopy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"), this, &C_SdNdeHalcChannelTreeView::m_OnPaste,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   this->mpc_ContextMenu->popup(c_PosGlobal);
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
