//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View part for add signals tree

   View part for add signals tree

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include <QMouseEvent>

#include "C_SdBueCoAddSignalsView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
   QItemSelectionModel * const pc_LastSelectionModel = this->selectionModel();

   this->mc_SortModel.setSortRole(static_cast<int32_t>(Qt::DisplayRole));
   this->mc_SortModel.setSourceModel(&this->mc_Model);
   this->C_SdBueCoAddSignalsView::setModel(&this->mc_SortModel);
   //Delete last selection model, see Qt documentation for setModel
   delete pc_LastSelectionModel;

   this->m_InitColumns();

   //Configure sorting
   this->setSortingEnabled(true);

   //Configure filter
   this->mc_SortModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();

   this->setDragEnabled(false);
   this->setSelectionBehavior(QAbstractItemView::SelectRows);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueCoAddSignalsView::m_ShowHideVerticalScrollBar);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdBueCoAddSignalsView::m_ShowHideHorizontalScrollBar);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set index

   \param[in]  orc_MessageId  Message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::SetIndex(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   this->mc_Model.SetIndex(orc_MessageId);
   this->sortByColumn(C_SdBueCoAddSignalsModel::h_EnumToColumn(C_SdBueCoAddSignalsModel::eINDEX), Qt::AscendingOrder);
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
std::vector<C_OscCanOpenManagerMappableSignal> C_SdBueCoAddSignalsView::GetSelectedSignals() const
{
   std::vector<C_OscCanOpenManagerMappableSignal> c_Retval;
   const std::map<uint32_t, std::vector<uint32_t> > c_UniqueSignals = C_SdBueCoAddSignalsModel::h_GetUniqueIndices(m_MapModelIndices(
                                                                                                                      this
                                                                                                                      ->
                                                                                                                      selectedIndexes()));
   for (std::map<uint32_t, std::vector<uint32_t> >::const_iterator c_ItTopLevel = c_UniqueSignals.cbegin();
        c_ItTopLevel != c_UniqueSignals.cend(); ++c_ItTopLevel)
   {
      for (std::vector<uint32_t>::const_iterator c_ItSignal = c_ItTopLevel->second.cbegin();
           c_ItSignal != c_ItTopLevel->second.cend(); ++c_ItSignal)
      {
         const C_OscCanOpenManagerMappableSignal * const pc_Entry = this->mc_Model.GetDataForIndex(c_ItTopLevel->first,
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
void C_SdBueCoAddSignalsView::Search(const QString & orc_Text)
{
   this->mc_SortModel.SetFilter(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse double click event slot

   Here: Add dialog exit if valid selection

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(opc_Event->globalPos()));

   QTreeView::mouseDoubleClickEvent(opc_Event);
   if ((c_Index.isValid() == true) && (this->selectedIndexes().size() > 0))
   {
      if (C_SdBueCoAddSignalsView::mh_CountUnique(m_MapModelIndices(this->selectedIndexes())) == 1UL)
      {
         Q_EMIT this->SigAccept();
      }
   }
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
   Q_EMIT this->SigSelectionChanged(C_SdBueCoAddSignalsView::mh_CountUnique(m_MapModelIndices(this->selectedIndexes())));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get unique rows

   \param[in]  orc_Indices    Indices

   \return
   Unique rows
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueCoAddSignalsView::mh_CountUnique(const QModelIndexList & orc_Indices)
{
   uint32_t u32_Retval = 0;

   const std::map<uint32_t,
                  std::vector<uint32_t> > c_Rows = C_SdBueCoAddSignalsModel::h_GetUniqueIndices(orc_Indices);

   for (std::map<uint32_t, std::vector<uint32_t> >::const_iterator c_It = c_Rows.cbegin();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Showing or hiding the vertical scrollbar for no resizing the parent widget

   \param[in]  os32_Min  Min
   \param[in]  os32_Max  Max
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show hide horizontal scroll bar

   \param[in]  os32_Min  Min
   \param[in]  os32_Max  Max
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsView::m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->horizontalScrollBar()->hide();
   }
   else
   {
      this->horizontalScrollBar()->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map model indices

   \param[in]  orc_SortModelIndices    Sort model indices

   \return
   Converted model indices
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndexList C_SdBueCoAddSignalsView::m_MapModelIndices(const QModelIndexList & orc_SortModelIndices) const
{
   QModelIndexList c_Retval;

   c_Retval.reserve(orc_SortModelIndices.size());
   for (QModelIndexList::ConstIterator c_ItSortModelIndex = orc_SortModelIndices.cbegin();
        c_ItSortModelIndex != orc_SortModelIndices.cend(); ++c_ItSortModelIndex)
   {
      c_Retval.push_back(this->mc_SortModel.mapToSource(*c_ItSortModelIndex));
   }
   return c_Retval;
}
