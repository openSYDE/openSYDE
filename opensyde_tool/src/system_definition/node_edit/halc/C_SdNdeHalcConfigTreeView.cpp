//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_SdNdeHalcConfigTreeView.h"
#include "C_PuiSdHandler.h"
#include "C_UsHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
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
C_SdNdeHalcConfigTreeView::C_SdNdeHalcConfigTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setSelectionMode(QAbstractItemView::NoSelection);

   //Hack for scroll bar below header
   this->mpc_PushButtonScrollTop = new C_OgePubIconOnly(this->verticalScrollBar());
   this->mpc_PushButtonScrollTop->setGeometry(0, 0, 25, 27); // necessary for correct drawing of the images
   this->mpc_PushButtonScrollTop->SetCustomIcons("://images/system_definition/NodeEdit/halc/HeadingPart.svg", "", "",
                                                 "");

   // default section sizes
   this->header()->setDefaultSectionSize(200);
   this->m_InitColumnWidths();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeView::~C_SdNdeHalcConfigTreeView(void)
{
   //lint -e{1540} never took ownership of this button (parent is scrollbar)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::SetNode(const uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNode(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC channel use case

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  ou32_UseCaseIndex    Use case index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::SetHalcChannelUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const uint32 ou32_UseCaseIndex, const bool oq_UseChannelIndex)
{
   this->mc_Model.SetHalcChannelUseCase(ou32_DomainIndex, ou32_ChannelIndex, ou32_UseCaseIndex, oq_UseChannelIndex);
   this->expandAll();

   const sint32 s32_Col = C_SdNdeHalcConfigTreeModel::h_EnumToColumn(C_SdNdeHalcConfigTreeModel::eVALUE);

   // open permanent editor widgets
   for (sint32 s32_RowParent = 0; s32_RowParent < this->mc_Model.rowCount(); s32_RowParent++)
   {
      const QModelIndex & rc_CurrentParent = this->mc_Model.index(s32_RowParent, s32_Col);

      // parent
      if (this->mc_Model.flags(rc_CurrentParent).testFlag(Qt::ItemIsEditable) == true)
      {
         this->openPersistentEditor(rc_CurrentParent);
      }

      // children
      for (sint32 s32_RowChild = 0; s32_RowChild < this->mc_Model.rowCount(rc_CurrentParent); s32_RowChild++)
      {
         const QModelIndex & rc_CurrentChild = rc_CurrentParent.child(s32_RowChild, s32_Col);

         if (this->mc_Model.flags(rc_CurrentChild).testFlag(Qt::ItemIsEditable) == true)
         {
            this->openPersistentEditor(rc_CurrentChild);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::LoadUserSettings(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      const C_UsNode c_Node = C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());

      if (this->m_SetColumnWidths(c_Node.GetHalcConfigColumnWidth()) == false)
      {
         this->m_InitColumnWidths();
      }
      // do not use user setting for last column as this makes the whole table very wide
      this->setColumnWidth(C_SdNdeHalcConfigTreeModel::h_EnumToColumn(C_SdNdeHalcConfigTreeModel::eDESCRIPTION), 300);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::SaveUserSettings(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mc_Model.GetNodeIndex());

   if (pc_Node != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSdNodeHalcConfigColumnWidth(pc_Node->c_Properties.c_Name.c_str(),
                                                                       this->m_GetColumnWidths());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::m_InitColumnWidths(void)
{
   this->setColumnWidth(C_SdNdeHalcConfigTreeModel::h_EnumToColumn(C_SdNdeHalcConfigTreeModel::eNAME), 200);
   this->setColumnWidth(C_SdNdeHalcConfigTreeModel::h_EnumToColumn(C_SdNdeHalcConfigTreeModel::eVALUE), 100);
   this->setColumnWidth(C_SdNdeHalcConfigTreeModel::h_EnumToColumn(C_SdNdeHalcConfigTreeModel::eDESCRIPTION), 300);
}
