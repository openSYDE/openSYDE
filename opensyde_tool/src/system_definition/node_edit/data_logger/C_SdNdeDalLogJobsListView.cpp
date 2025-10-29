//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List view for log jobs

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QKeyEvent>
#include <QScrollBar>
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobsListView.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *  \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsListView::C_SdNdeDalLogJobsListView(QWidget * const opc_Parent) :
   QListView(opc_Parent),
   ms32_DeletedItemIndex(-1),
   mq_SelectAllItems(false)
{
   this->C_SdNdeDalLogJobsListView::setModel(&this->mc_Model);

   this->setViewMode(QListView::ListMode);
   this->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->horizontalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Avoid styling table inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SdNdeDalLogJobsListView", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SdNdeDalLogJobsListView", true);

   connect(&this->mc_Model, &C_SdNdeDalLogJobsListModel::SigDataChanged, this,
           &C_SdNdeDalLogJobsListView::SigDataChanged);
   connect(&this->mc_Model, &C_SdNdeDalLogJobsListModel::rowsRemoved, this,
           &C_SdNdeDalLogJobsListView::m_LogJobDeleted);
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeDalLogJobsListView::m_ShowHideVerticalScrollBar);
   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_SdNdeDalLogJobsListView::m_ShowHideHorizontalScrollBar);
}

// ----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsListView::~C_SdNdeDalLogJobsListView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if any items present

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobsListView::IsEmpty() const
{
   return (this->mc_Model.rowCount() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load Log Jobs

   \param[in]       ou32_NodeIndex     Node Index for which log jobs are to be fetched

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::LoadLogJobs(const uint32_t ou32_NodeIndex)
{
   this->mc_Model.UpdateData(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current index value

   \param[in]       ore_Selection      The type of operation decided the current index
   \param[in]       ou32_LogJobIndex   Optional parameter only used when a list index is explicitly to be selected

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::SetSelection(const C_SdNdeDalLogJobsListView::E_LogJobSelection & ore_Selection,
                                             const uint32_t ou32_LogJobIndex)
{
   const uint32_t u32_RowCount = this->mc_Model.rowCount();

   if ((u32_RowCount > 0))
   {
      switch (ore_Selection)
      {
      case eINITIALIZE: // set default index to first item i.e 0 when the list loads initially
         if (this->currentIndex().row() == -1)
         {
            this->setCurrentIndex(this->mc_Model.index(0, 0));
         }
         break;
      case eLOGJOB_ADDED: // set selection to the newly added job to the end of the list
         {
            const QModelIndex c_ModelIndex = this->mc_Model.index(static_cast<int32_t>(u32_RowCount - 1), 0);
            this->setCurrentIndex(c_ModelIndex);
            break;
         }
      case eSELECTINDEX: // set selection to the given index
         this->setCurrentIndex(this->mc_Model.index(ou32_LogJobIndex, 0));
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete log jobs

   \param[in]       orc_DataLoggerJobIndices     Indexes of log jobs to be deleted

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::DeleteLogJobs(const std::vector<uint32_t> & orc_DataLoggerJobIndices)
{
   if ((this->IsEmpty() == false) &&
       (orc_DataLoggerJobIndices.size() <= static_cast<uint32_t>(this->mc_Model.rowCount())))
   {
      this->mc_Model.DoRemoveRows(orc_DataLoggerJobIndices);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load selected LogJob from Overview widget

   \param[in]  ou32_LogJobIndex     LogJob index to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::LoadSelectedLogJob(const uint32_t ou32_LogJobIndex)
{
   this->setCurrentIndex(mc_Model.index(ou32_LogJobIndex, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten selection changed event slot

   Here: Emit signal with new number of selected items

   \param[in]  orc_Selected      Selected
   \param[in]  orc_Deselected    Deselected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::selectionChanged(const QItemSelection & orc_Selected,
                                                 const QItemSelection & orc_Deselected)
{
   const uint32_t u32_SelectedItemCount = this->selectedIndexes().size();
   const uint32_t u32_RowCount = this->mc_Model.rowCount();

   // Atleast one list item selected
   if (u32_SelectedItemCount > 0)
   {
      uint32_t u32_ListIndex = 0;

      // Single item is selected (orc_Deselected = 1)
      if (orc_Selected.size() == 1)
      {
         u32_ListIndex =  static_cast<uint32_t>(orc_Selected.at(0).top());
      }
      // Transition from multi-selection to single item selection (special case for first or last item)
      if ((orc_Deselected.size() == 1) && (orc_Selected.size() == 0))
      {
         if (orc_Deselected.at(0).top() == 1) // first list item selected
         {
            u32_ListIndex = 0;
         }
         else if (orc_Deselected.at(0).top() == 0) // last item selected
         {
            u32_ListIndex = u32_RowCount - 1;
         }
         else
         {
            //      Unknown
         }
      }

      // Transition from multi-selection to single item selection (all items except first and last)
      if (orc_Deselected.size() > 1)
      {
         u32_ListIndex =  static_cast<uint32_t>(orc_Deselected.at(1).top() - 1);
      }

      // Multi-selection (Select All (either by shortcut "Ctrl+A" or by dragging selection)
      if (u32_SelectedItemCount == u32_RowCount)
      {
         if (orc_Deselected.size() >= 1)
         {
            u32_ListIndex = static_cast<uint32_t>(orc_Deselected.at(1).top() - 1);
         }
         // Transition from single item to all items selection (special case for previously last or first list item)
         // the flag "this->mq_SelectAllItems" is used to identify whether all items are selected using shortcut
         // "Ctrl+A" or by dragging selection
         else if ((orc_Selected.size() == 1) && (this->mq_SelectAllItems == true))
         {
            if (orc_Selected.at(0).top() == 1) // first list item
            {
               u32_ListIndex =  0;
            }
            else // last list item
            {
               u32_ListIndex = u32_RowCount - 1;
            }

            this->mq_SelectAllItems = false;
         }
         // Any list element previously selected (other than first and last)
         else if (orc_Selected.size() > 1)
         {
            u32_ListIndex =  static_cast<uint32_t>(orc_Selected.at(1).top() - 1);
         }
         else
         {
            // Unknown
         }
      }

      QListView::selectionChanged(orc_Selected, orc_Deselected);
      Q_EMIT this->SigSelectionChanged(u32_ListIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_A))
   {
      // Enable the flag in order to identify "Ctrl +A" (Select All) option (in selectionChanged() method)
      this->mq_SelectAllItems = true;
   }

   QListView::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for logs jobs deletion (rows deleted from model)


   \param[in]        orc_parent     Detailed input parameter description
   \param[in]        os32_First     Detailed output parameter description
   \param[in]        os32_Last      Detailed input/output parameter description

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::m_LogJobDeleted(const QModelIndex & orc_Parent, const int32_t os32_First,
                                                const int32_t os32_Last)
{
   Q_UNUSED(orc_Parent)
   Q_UNUSED(os32_Last)

   int32_t s32_CurrentIndex = os32_First;

   // ensure that the model has some data.
   // if deleted item was previously the last item on the list, ensure the new current index is valid
   if ((this->mc_Model.rowCount() > 0) && (s32_CurrentIndex == this->mc_Model.rowCount()))
   {
      s32_CurrentIndex = os32_First - 1;
   }

   this->setCurrentIndex(this->mc_Model.index(s32_CurrentIndex, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show hide vertical scroll bar

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
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

   \param[in]  os32_Min  Minimum range
   \param[in]  os32_Max  Maximum range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const
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
