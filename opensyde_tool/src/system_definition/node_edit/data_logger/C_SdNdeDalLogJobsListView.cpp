//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List view for log jobs

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobsListView.hpp"

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
   ms32_DeletedItemIndex(-1)
{
   this->C_SdNdeDalLogJobsListView::setModel(&this->mc_Model);

   this->setViewMode(QListView::ListMode);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   connect(&this->mc_Model, &C_SdNdeDalLogJobsListModel::SigDataChanged, this,
           &C_SdNdeDalLogJobsListView::SigDataChanged);
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

   \param[in]       ore_Selection     The type of operation decided the current index

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::SetSelection(const C_SdNdeDalLogJobsListView::E_LogJobSelection & ore_Selection)
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
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete a log job

   \param[in]       ou32_LogJobIndex     The index of the log job to be deleted

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListView::DeleteLogJob(const uint32_t ou32_LogJobIndex)
{
   if ((this->IsEmpty() == false) && (ou32_LogJobIndex < static_cast<uint32_t>(this->mc_Model.rowCount())))
   {
      this->ms32_DeletedItemIndex = ou32_LogJobIndex;
      this->mc_Model.DoRemoveRow(ou32_LogJobIndex);
   }
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

   // since this is a single selection list, there is only one selected item at a time.
   // Hence we always take the first element "this->selectedIndexes().at(0)"
   if (u32_SelectedItemCount > 0)
   {
      const uint32_t u32_RowCount = this->mc_Model.rowCount();
      uint32_t u32_ListIndex = 0;

      // If a list item was deleted (deleted index is between 0 and item count after deletion).
      // The emitted signal passes the index of the deselected item i.e. the item that was last deleted. This
      // corresponds to the index of the log job array at the data management side.
      if ((this->ms32_DeletedItemIndex >= 0) && (this->ms32_DeletedItemIndex < static_cast<int32_t>(u32_RowCount - 1)))
      {
         this->ms32_DeletedItemIndex = -1;
         if (orc_Deselected.size() > 0)
         {
            u32_ListIndex = static_cast<uint32_t>(orc_Deselected.at(0).top());
         }
      }
      // This index change corresponds to a change in selection (by user)
      else
      {
         if (orc_Selected.size() > 0)
         {
            u32_ListIndex =  static_cast<uint32_t>(orc_Selected.at(0).top());
         }
      }
      Q_EMIT this->SigSelectionChanged(u32_ListIndex);
   }
}
