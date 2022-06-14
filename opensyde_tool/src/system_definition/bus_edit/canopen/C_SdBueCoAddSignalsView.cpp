//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View part for add signals tree

   View part for add signals tree

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_SdBueCoAddSignalsView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueCoAddSignalsView::C_SdBueCoAddSignalsView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->C_SdBueCoAddSignalsView::setModel(&this->mc_Model);

   this->m_InitColumns();

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

   this->setDragEnabled(false);
   this->setSelectionBehavior(QAbstractItemView::SelectRows);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set index

   \param[in]  orc_MessageId  Message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::SetIndex(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   this->mc_Model.SetIndex(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cleans up the last model configuration as preparation for the entire deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::PrepareCleanUp()
{
   this->mc_Model.PrepareCleanUp();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected signals

   \return
   Selected signals
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_SdBueCoAddSignalsResultEntry> C_SdBueCoAddSignalsView::GetSelectedSignals() const
{
   std::vector<C_SdBueCoAddSignalsResultEntry> c_Retval;
   const std::map<uint32, std::vector<uint32> > c_UniqueSignals = C_SdBueCoAddSignalsModel::h_GetUniqueIndices(
      this->selectedIndexes());
   for (std::map<uint32, std::vector<uint32> >::const_iterator c_ItTopLevel = c_UniqueSignals.cbegin();
        c_ItTopLevel != c_UniqueSignals.cend(); ++c_ItTopLevel)
   {
      for (std::vector<uint32>::const_iterator c_ItSignal = c_ItTopLevel->second.cbegin();
           c_ItSignal != c_ItTopLevel->second.cend(); ++c_ItSignal)
      {
         const C_SdBueCoAddSignalsResultEntry * const pc_Entry = this->mc_Model.GetDataForIndex(c_ItTopLevel->first,
                                                                                                *c_ItSignal);
         if (pc_Entry != NULL)
         {
            c_Retval.push_back(*pc_Entry);
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if view empty

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueCoAddSignalsView::IsEmpty() const
{
   return this->mc_Model.rowCount() == 0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} WIP
void C_SdBueCoAddSignalsView::Search(const QString & orc_Text) const
{
   Q_UNUSED(orc_Text)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in]  orc_Selected      Selected
   \param[in]  orc_Deselected    Deselected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::selectionChanged(const QItemSelection & orc_Selected,
                                               const QItemSelection & orc_Deselected)
{
   QTreeView::selectionChanged(orc_Selected, orc_Deselected);
   Q_EMIT this->SigSelectionChanged(C_SdBueCoAddSignalsView::mh_CountUnique(this->selectedIndexes()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get unique rows

   \param[in]  orc_Indices    Indices

   \return
   Unique rows
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdBueCoAddSignalsView::mh_CountUnique(const QModelIndexList & orc_Indices)
{
   uint32 u32_Retval = 0;

   const std::map<stw_types::uint32,
                  std::vector<stw_types::uint32> > c_Rows = C_SdBueCoAddSignalsModel::h_GetUniqueIndices(orc_Indices);

   for (std::map<stw_types::uint32, std::vector<stw_types::uint32> >::const_iterator c_It = c_Rows.cbegin();
        c_It != c_Rows.cend(); ++c_It)
   {
      u32_Retval += c_It->second.size();
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init default columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::m_InitColumns(void)
{
   this->setColumnWidth(C_SdBueCoAddSignalsModel::h_EnumToColumn(C_SdBueCoAddSignalsModel::eINDEX), 150);
   this->setColumnWidth(C_SdBueCoAddSignalsModel::h_EnumToColumn(C_SdBueCoAddSignalsModel::eNAME), 200);
   this->setColumnWidth(C_SdBueCoAddSignalsModel::h_EnumToColumn(C_SdBueCoAddSignalsModel::eLENGTH), 80);
}
