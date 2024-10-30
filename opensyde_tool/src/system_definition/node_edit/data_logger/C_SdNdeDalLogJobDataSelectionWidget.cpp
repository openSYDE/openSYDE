//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log job data selection

   Base widget for log job data selection

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"

#include "C_SdNdeDalLogJobDataSelectionWidget.hpp"
#include "ui_C_SdNdeDalLogJobDataSelectionWidget.h"

#include "../../../system_views/dashboards/properties/C_SyvDaPeDataElementBrowse.hpp"
#include "C_OgePopUpDialog.hpp"

#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobDataSelectionWidget::C_SdNdeDalLogJobDataSelectionWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobDataSelectionWidget),
   mu32_NodeIndex(0),
   mu32_DataLoggerJobIndex(0)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_TableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

   this->InitText();
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");

   this->m_UpdateUi();

   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_AddClicked);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDalLogJobDataSelectionTableView::SigSelectionChanged, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_UpdateSelection);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDalLogJobDataSelectionTableView::SigDataChanged, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_DataChangedInModel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobDataSelectionWidget::~C_SdNdeDalLogJobDataSelectionWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobDataSelectionWidget::LoadUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} TODO
void C_SdNdeDalLogJobDataSelectionWidget::SaveUserSettings() const
{
   //TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node data logger indices
 *
 *  Note : this is generally done when a new node is selected and correspondingly new data is to be loaded

   \param[in]  ou32_NodeIndex             Index of node
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1762,9175}
void C_SdNdeDalLogJobDataSelectionWidget::SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                               const uint32_t ou32_DataLoggerJobIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataLoggerJobIndex = ou32_DataLoggerJobIndex;

   // Load data elements of the newly selected node
   this->m_LoadDataElements();

   this->mpc_Ui->pc_TableView->UpdateData(this->mc_DataElements, this->mu32_NodeIndex);
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initializes all visible strings on the widget

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::InitText() const
{
   this->mpc_Ui->pc_LabelLoggingData->setText(C_GtGetText::h_GetText("Logging Data"));
   this->mpc_Ui->pc_LabelSelection->setText(C_GtGetText::h_GetText("No selected element"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Add icon clicked

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_AddClicked()
{
   // Dialog opens up for data element selection
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, 0U, true, false, true, false,
                                                                                 true, true, NULL, false);

   //Resize
   c_New->SetSize(QSize(800, 800));

   std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> c_SelectedDataElements;

   // Save the selected data elements to system definition
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();

      //Cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);

      if (c_DataElements.size() > 0)
      {
         c_SelectedDataElements.reserve(c_DataElements.size());

         uint32_t u32_Counter;
         for (u32_Counter = 0U; u32_Counter < c_DataElements.size(); ++u32_Counter)
         {
            if (c_DataElements[u32_Counter].GetIsValid() == true)
            {
               tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckAndHandleNewElement(
                             c_DataElements[u32_Counter]) == C_NO_ERR);

               C_OscDataLoggerDataElementReference c_Data;
               c_Data.c_ConfiguredElementId = c_DataElements[u32_Counter];

               this->mc_DataElements.push_back(c_Data);
               c_SelectedDataElements.push_back(c_Data);

               // Persistent storage of data element in the system definition
               C_PuiSdHandler::h_GetInstance()->AddDataLoggerElement(this->mu32_NodeIndex,
                                                                     this->mu32_DataLoggerJobIndex, c_Data);
            }
         }
      }

      //Cursor
      QApplication::restoreOverrideCursor();

      // Update the data elements in the view
      this->mpc_Ui->pc_TableView->UpdateData(c_SelectedDataElements, this->mu32_NodeIndex);
      this->m_UpdateUi();
      this->m_UpdateSelection(0);

      c_SelectedDataElements.clear();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      pc_Dialog->PrepareCleanUp();
      c_New->HideOverlay();
      c_New->deleteLater();
   }
   //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Loads the previously added datapool elements in the table

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_LoadDataElements()
{
   // Fetch the datalogger job from system definition
   const stw::opensyde_core::C_OscDataLoggerJob * const pc_DataLoggerJob =
      C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
         this->mu32_NodeIndex, this->mu32_DataLoggerJobIndex);

   this->mc_DataElements.clear();
   this->mc_DataElements.reserve(pc_DataLoggerJob->c_ConfiguredDataElements.size());

   for (uint32_t u32_Counter = 0U; u32_Counter < pc_DataLoggerJob->c_ConfiguredDataElements.size(); ++u32_Counter)
   {
      this->mc_DataElements.push_back(pc_DataLoggerJob->c_ConfiguredDataElements[u32_Counter]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle the visibility of UI elements


*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_UpdateUi() const
{
   if (this->mpc_Ui->pc_TableView->IsEmpty() == false)
   {
      this->mpc_Ui->pc_TableView->setVisible(true);
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      this->mpc_Ui->pc_LabelNoMessages->setVisible(false);
   }
   // no messages
   else
   {
      this->mpc_Ui->pc_TableView->setVisible(false);
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
      this->mpc_Ui->pc_LabelNoMessages->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief   Update number of selected items

   \param[in]  os32_SelectionCount   Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_UpdateSelection(const uint32_t ou32_SelectionCount) const
{
   if (this->mpc_Ui->pc_TableView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      if (ou32_SelectionCount > 0)
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "%1 selected data element(s)")).
                                                  arg(ou32_SelectionCount));
         this->mpc_Ui->pc_TableView->update();
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "No selected data element")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Called when data is manually changed in the model by "editing" in the table

   \param[in]   orc_Index     Index of the modified data element
   \param[in]   oc_Data       Newly edited data

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_DataChangedInModel(const QModelIndex orc_Index, const QString oc_Data)
{
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerElementProperties(this->mu32_NodeIndex, this->mu32_DataLoggerJobIndex,
                                                                   orc_Index.row(), true, oc_Data.toStdString());

   // The index row is the same as the index in the vector
   this->m_UpdateCustomLoggingName(orc_Index.row(), oc_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the custom logging name in the local vector for data management

   \param[in]     ou32_Index     The index at which
   \param[in]     oc_Data        Detailed input/output parameter description

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_UpdateCustomLoggingName(const uint32_t ou32_Index, const QString oc_Data)
{
   // update the custom logging name
   if (ou32_Index <= this->mc_DataElements.size())
   {
      C_OscDataLoggerDataElementReference & rc_Data = this->mc_DataElements.at(ou32_Index);
      rc_Data.c_CustomName = oc_Data.toStdString();
      rc_Data.q_UseCustomName = true;
   }
}
