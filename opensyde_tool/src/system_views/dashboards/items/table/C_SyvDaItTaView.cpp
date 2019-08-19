//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for dashboard table widget (implementation)

   Table view for dashboard table widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QMouseEvent>
#include "stwtypes.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_SyvDaItTaView.h"
#include "C_SyvDaItTableHeaderView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Data   Data storage
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTaView::C_SyvDaItTaView(C_PuiSvDbDataElementHandler * const opc_Data, QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent),
   mc_Model(opc_Data)
{
   //Model
   this->setModel(&mc_Model);

   //Drawing delegate
   this->setItemDelegate(&mc_Delegate);

   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
   this->SetSelectionAvailable(true);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(EditTrigger::NoEditTriggers);
   //Consider all elements for resize
   this->setHorizontalHeader(new C_SyvDaItTableHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setStretchLastSection(false);
   this->horizontalHeader()->setFixedHeight(27);
   this->horizontalHeader()->setMinimumSectionSize(4);
   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setDefaultSectionSize(20);
   //Icon
   this->setIconSize(QSize(16, 16));

   //Hide vertical header
   this->verticalHeader()->hide();

   //Hover event
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTaView::~C_SyvDaItTaView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize min & max values and names for data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::UpdateStaticValues(void)
{
   this->mc_Model.InitMinMaxAndName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get newest values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::UpdateValue(void)
{
   this->mc_Model.UpdateValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::UpdateError(void)
{
   this->mc_Model.UpdateError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] osn_Value                       Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   this->mc_Model.UpdateTransparence(ou32_DataElementIndex, osn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new data element

   \param[in] orc_DataPoolElementId New data element ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::AddItem(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
{
   const QModelIndexList c_IndexList = this->selectedIndexes();
   const uint32 u32_NewItem = this->mc_Model.AddItem(c_IndexList, orc_DataPoolElementId);

   // select new item
   this->selectRow(u32_NewItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move selected items

   \param[in] oq_Up Flag to switch to move one step up or down
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::MoveSelected(const bool oq_Up)
{
   const QModelIndexList c_IndexList = this->selectedIndexes();

   this->mc_Model.MoveItems(c_IndexList, oq_Up);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove all selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::RemoveSelectedItems(std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements)
{
   const QModelIndexList c_IndexList = this->selectedIndexes();

   this->mc_Model.RemoveItems(c_IndexList, orc_RemovedDataElements);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected item indices

   \return
   Selected item indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SyvDaItTaView::GetUniqueSelectedRows(void) const
{
   std::vector<uint32> c_Retval;
   const QModelIndexList c_IndexList = this->selectedIndexes();
   this->mc_Model.GetUniqueRows(c_IndexList, c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get only valid selected item indices

   \return
   Only valid selected item indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SyvDaItTaView::GetUniqueAndValidSelectedRows(void) const
{
   std::vector<uint32> c_Retval;
   const std::vector<uint32> c_Tmp = GetUniqueSelectedRows();
   for (uint32 u32_It = 0UL; u32_It < c_Tmp.size(); ++u32_It)
   {
      const C_PuiSvDbNodeDataPoolListElementId * const pc_DataElementId =
         this->mc_Model.GetDataPoolElementIndex(c_Tmp[u32_It]);
      if ((pc_DataElementId != NULL) && (pc_DataElementId->GetIsValid() == true))
      {
         c_Retval.push_back(c_Tmp[u32_It]);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore column widths

   \param[in] orc_ColumnWidths Stored column widths (Restores default values if empty)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::SetCurrentColumnWidths(const std::vector<sint32> & orc_ColumnWidths)
{
   if (orc_ColumnWidths.size() > 0)
   {
      for (uint32 u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
      {
         this->setColumnWidth(static_cast<sintn>(u32_ItCol), orc_ColumnWidths[u32_ItCol]);
      }
   }
   else
   {
      //Default
      this->setColumnWidth(C_SyvDaItTaModel::h_EnumToColumn(C_SyvDaItTaModel::eICON), 26);
      this->setColumnWidth(C_SyvDaItTaModel::h_EnumToColumn(C_SyvDaItTaModel::eNAME), 150);
      this->setColumnWidth(C_SyvDaItTaModel::h_EnumToColumn(C_SyvDaItTaModel::eVALUE), 150);
      this->setColumnWidth(C_SyvDaItTaModel::h_EnumToColumn(C_SyvDaItTaModel::eBAR), 150);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SyvDaItTaView::GetCurrentColumnWidths(void) const
{
   std::vector<sint32> c_Retval;
   c_Retval.reserve(this->model()->columnCount());
   for (sint32 s32_ItCol = 0; s32_ItCol < this->model()->columnCount(); ++s32_ItCol)
   {
      c_Retval.push_back(this->columnWidth(s32_ItCol));
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selection available if necessary

   \param[in] oq_Active Selection active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::SetSelectionAvailable(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->setSelectionMode(QAbstractItemView::ExtendedSelection);
   }
   else
   {
      this->clearSelection();
      this->setSelectionMode(QAbstractItemView::NoSelection);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaView::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   this->mc_Delegate.SetDisplayStyle(oe_Style, oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "DarkMode", oq_DarkMode);
   this->mc_Model.SetDisplayStyle(oe_Style, oq_DarkMode);
   switch (oe_Style)
   {
   //TBD by Karsten: done
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      this->setGridStyle(Qt::NoPen);
      this->setShowGrid(false);
      this->setAlternatingRowColors(true);
      break;
   case C_PuiSvDbWidgetBase::eFLAT:
      this->setGridStyle(Qt::NoPen);
      this->setShowGrid(false);
      this->setAlternatingRowColors(true);
      break;
   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      this->setGridStyle(Qt::NoPen);
      this->setShowGrid(false);
      this->setAlternatingRowColors(true);
      break;
   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      this->setGridStyle(Qt::SolidLine);
      this->setShowGrid(true);
      this->setAlternatingRowColors(false);
      break;
   }
}
