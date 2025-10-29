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
#include "C_Uti.hpp"
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
   mpc_ContextMenu(NULL),
   mu32_NodeIndex(0),
   mu32_DataLoggerJobIndex(0)

{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // Hide the filter line edit for now
   this->mpc_Ui->pc_LineEditFilter->setVisible(false);

   this->mpc_Ui->pc_TableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");

   this->m_SetupContextMenu();
   this->m_UpdateSelection();
   this->m_UpdateUi();

   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_AddClicked);

   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDalLogJobDataSelectionTableView::SigDataChanged, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_OnDataChangedInModel);

   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDalLogJobDataSelectionTableView::SigDeleteSelectedElements, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_DeleteSelectedDataElements);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
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

   this->ReloadDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload data elements in the model

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::ReloadDataElements()
{
   // Load data elements of the newly selected node
   this->m_LoadDataElements();

   this->mpc_Ui->pc_TableView->UpdateData(this->mc_DataElements, this->mu32_NodeIndex);
   this->m_UpdateSelection();
   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the number of local and remote elements

   \param[in,out]   oru32_LocalElements     Number of local elements
   \param[in,out]   oru32_RemoteElements    Number of remote elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::GetElementLocationCount(uint32_t & oru32_LocalElements,
                                                                  uint32_t & oru32_RemoteElements)
{
   this->mpc_Ui->pc_TableView->GetElementLocationCount(oru32_LocalElements, oru32_RemoteElements);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initializes all visible strings on the widget

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_LabelLoggingData->setText(C_GtGetText::h_GetText("Logging Data"));
   this->mpc_Ui->pc_LabelSelection->setText(C_GtGetText::h_GetText("No selected element"));
   this->mpc_Ui->pc_LabelNoMessages->setText(C_GtGetText::h_GetText("No data elements to display."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Add icon clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_AddClicked()
{
   // Dialog opens up for data element selection
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, 0U, true, false, true, true,
                                                                                 true, true, NULL, false,
                                                                                 this->mu32_NodeIndex);

   //Resize
   c_New->SetSize(QSize(800, 800));

   // Save the selected data elements to system definition
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> c_SelectedDataElements;
      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();

      //Cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);

      // If any data elements were selected from the popup dialog
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

               // Append the newly added data elements to the local vector
               // And to the vector that is passed on to the model for insertion in the table
               this->mc_DataElements.push_back(c_Data);
               c_SelectedDataElements.push_back(c_Data);

               // Persistent storage of data element in the system definition
               C_PuiSdHandler::h_GetInstance()->AddDataLoggerElement(this->mu32_NodeIndex,
                                                                     this->mu32_DataLoggerJobIndex,
                                                                     c_Data);
               Q_EMIT this->SigNumElementsChanged();
            }
         }
      }

      //Cursor
      QApplication::restoreOverrideCursor();

      // Update the data elements in the view
      this->mpc_Ui->pc_TableView->AddData(c_SelectedDataElements, this->mu32_NodeIndex);
      this->m_UpdateSelection();
      this->m_UpdateUi();

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

   if (pc_DataLoggerJob != NULL)
   {
      this->mc_DataElements.clear();
      this->mc_DataElements.reserve(pc_DataLoggerJob->c_ConfiguredDataElements.size());

      for (uint32_t u32_Counter = 0U; u32_Counter < pc_DataLoggerJob->c_ConfiguredDataElements.size(); ++u32_Counter)
      {
         this->mc_DataElements.push_back(pc_DataLoggerJob->c_ConfiguredDataElements[u32_Counter]);
      }
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

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_UpdateSelection() const
{
   if (this->mpc_Ui->pc_TableView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);

      // default string value set for 0 elements
      QString c_DataElementsText(C_GtGetText::h_GetText("No data elements"));
      uint32_t u32_LocalElements = 0;
      uint32_t u32_RemoteElements = 0;

      // Get the number of local / remote elements
      this->mpc_Ui->pc_TableView->GetElementLocationCount(u32_LocalElements, u32_RemoteElements);

      if (this->mc_DataElements.size() > 0)
      {
         if (this->mc_DataElements.size() == 1)
         {
            c_DataElementsText =
               static_cast<QString>(C_GtGetText::h_GetText("%1 data element (%2 local; %3 remote)")).arg(
                  this->mc_DataElements.size()).arg(u32_LocalElements).arg(u32_RemoteElements);
         }
         else
         {
            c_DataElementsText =
               static_cast<QString>(C_GtGetText::h_GetText("%1 data elements (%2 local; %3 remote)")).arg(
                  this->mc_DataElements.size()).arg(u32_LocalElements).arg(u32_RemoteElements);
         }
      }

      this->mpc_Ui->pc_LabelSelection->setText(c_DataElementsText);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Called when data is manually changed in the model by "editing" in the table

   \param[in]   orc_Index     Index of the modified data element
   \param[in]   oc_Data       Newly edited data

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_OnDataChangedInModel(const QModelIndex & orc_Index, const QString oc_Data)
{
   bool q_UseCustomName = true;
   QString c_Data(oc_Data);

   // If user has reset the logging name to the namespace text
   if (oc_Data.compare(C_GtGetText::h_GetText("[Namespace]")) == 0)
   {
      q_UseCustomName = false;
      c_Data = QString();
   }

   C_PuiSdHandler::h_GetInstance()->SetDataLoggerElementProperties(this->mu32_NodeIndex, this->mu32_DataLoggerJobIndex,
                                                                   orc_Index.row(), q_UseCustomName,
                                                                   c_Data.toStdString());

   // Update logging name in local vector (orc_Index.row() is equal to the index in the vector)
   this->m_UpdateCustomLoggingName(orc_Index.row(), oc_Data, q_UseCustomName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the custom logging name in the local vector for data management

   \param[in]     ou32_Index        The index at which
   \param[in]     oc_Data           Detailed input/output parameter description
   \param[in]     oq_UseCustomName  true if custom logging name defined by user, false otherwise

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_UpdateCustomLoggingName(const uint32_t ou32_Index, const QString oc_Data,
                                                                    const bool oq_UseCustomName)
{
   // update the custom logging name
   if (ou32_Index < this->mc_DataElements.size())
   {
      C_OscDataLoggerDataElementReference & rc_Data = this->mc_DataElements.at(ou32_Index);
      rc_Data.c_CustomName = oc_Data.toStdString();
      rc_Data.q_UseCustomName = oq_UseCustomName;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_SetupContextMenu()
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->setContextMenuPolicy(Qt::CustomContextMenu);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                       "Select all"), this->mpc_Ui->pc_TableView,
                                    &C_SdNdeDalLogJobDataSelectionTableView::selectAll,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));
   // select all action
   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                    this, &C_SdNdeDalLogJobDataSelectionWidget::m_DeleteSelectedDataElements,
                                    static_cast<int32_t>(Qt::Key_Delete));

   connect(this, &C_SdNdeDalLogJobDataSelectionWidget::customContextMenuRequested, this,
           &C_SdNdeDalLogJobDataSelectionWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if focus is on tree view
   if ((this->mpc_Ui->pc_TableView->hasFocus() /*&& (this->mpc_Ui->pc_TreeView->HasVisibleData())*/))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*!   \brief   Delete selected data elements
 *
 */
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionWidget::m_DeleteSelectedDataElements(void)
{
   std::vector<uint32_t> c_SelectedIndices;

   // Get the selected elements from the model (original indexes irrespective of sort order are fetched)
   this->mpc_Ui->pc_TableView->GetSelectedElements(c_SelectedIndices);

   // Delete operation takes place if one or mores items are selected
   if (c_SelectedIndices.size() > 0)
   {
      // delete from data management
      for (uint32_t u32_Index = 0; u32_Index < c_SelectedIndices.size(); u32_Index++)
      {
         const uint32_t u32_IndexToDelete = c_SelectedIndices.at(u32_Index);
         C_PuiSdHandler::h_GetInstance()->DeleteDataLoggerElement(this->mu32_NodeIndex, this->mu32_DataLoggerJobIndex,
                                                                  u32_IndexToDelete);

         // delete from local vector
         this->mc_DataElements.erase(this->mc_DataElements.begin() + u32_IndexToDelete);
      }

      // delete from model / view and ultimately the table
      this->mpc_Ui->pc_TableView->DeleteSelectedElements();
      this->m_UpdateSelection();
      this->m_UpdateUi();
      Q_EMIT this->SigNumElementsChanged();
   }
}
