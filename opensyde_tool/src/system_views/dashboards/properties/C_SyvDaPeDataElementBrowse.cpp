//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element selection (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_SyvDaPeDataElementBrowse.hpp"

#include "ui_C_SyvDaPeDataElementBrowse.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaPeDataElementBrowse::mhs32_INDEX_DATAPOOL_ELEMENT = 0;
const int32_t C_SyvDaPeDataElementBrowse::mhs32_INDEX_BUS_SIGNAL = 1;
int32_t C_SyvDaPeDataElementBrowse::mhs32_LastSelectedComboBoxIndex = 0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                               Parent
   \param[in]      ou32_ViewIndex                           View index
   \param[in]      oq_MultiSelect                           Optional flag to allow multi selection
   \param[in]      oq_ShowOnlyWriteElements                 Optional flag to show only writable elements
   \param[in]      oq_ShowArrayElements                     Optional flag to hide all array elements (if false)
   \param[in]      oq_ShowArrayIndexElements                Optional flag to hide all array index elements (if false)
   \param[in]      oq_Show64BitValues                       Optional flag to hide all 64 bit elements (if false)
   \param[in]      oq_ShowNvmLists                          Optional flag to only show NVM LISTs
   \param[in]      opc_AlreasyUsedElements                  Optional pointer to vector with already used elements. All added elements
                                                            will be marked as used an will be disabled. Usable for non NVM list mode
   \param[in]      oq_UseInSysViews                         True if the dialog is used within system views/commissioning, false otherwise
   \param[in]      ou32_SdDataLoggerUseCaseNodeIndex        System definition data logger use case: node index
   \param[in]      ou32_SdDataLoggerUseCaseDataLoggerIndex  System definition data logger use case: data logger index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeDataElementBrowse::C_SyvDaPeDataElementBrowse(C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex,
                                                       const bool oq_MultiSelect, const bool oq_ShowOnlyWriteElements,
                                                       const bool oq_ShowArrayElements,
                                                       const bool oq_ShowArrayIndexElements,
                                                       const bool oq_Show64BitValues, const bool oq_ShowNvmLists,
                                                       const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, const bool oq_UseInSysViews, const uint32_t ou32_SdDataLoggerUseCaseNodeIndex,
                                                       const uint32_t ou32_SdDataLoggerUseCaseDataLoggerIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeDataElementBrowse),
   mpc_ContextMenu(NULL),
   mrc_ParentDialog(orc_Parent)
{
   bool q_TreeFilled = false;

   mpc_Ui->setupUi(this);

   if (oq_MultiSelect == true)
   {
      this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   }
   else
   {
      this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::SingleSelection);
   }

   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);

   //Deactivate debug string
   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setTitle("");

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   //Init (BEFORE combo box access!)
   InitStaticNames();

   // Save the flags for m_SwitchType slot for combo box
   this->mq_ShowOnlyWriteElements = oq_ShowOnlyWriteElements;
   this->mq_ShowArrayElements = oq_ShowArrayElements;
   this->mq_ShowArrayIndexElements = oq_ShowArrayIndexElements;
   this->mq_Show64BitValues = oq_Show64BitValues;
   this->mpc_AlreasyUsedElements = opc_AlreasyUsedElements;

   // set title
   if (oq_ShowNvmLists == true)
   {
      this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("NVM List")));
      this->mpc_Ui->pc_ComboBoxType->setVisible(false);
      this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("Select NVM List"));
   }
   else
   {
      this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("Data Element")));
      this->mpc_Ui->pc_ComboBoxType->setVisible(true);
      this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("Select Data Element"));

      //Restoration only necessary for read elements!
      if (oq_ShowOnlyWriteElements == false)
      {
         //Init combo box (restore)
         //lint -e{1938}  no write access in constructor, initialization is done outside constructor
         this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeDataElementBrowse::mhs32_LastSelectedComboBoxIndex);
         // InitSV will not be called when calling m_SwitchType and the view index will not be updated
         this->mpc_Ui->pc_TreeView->SetViewIndex(ou32_ViewIndex);
         //lint -e{1938}  no write access in constructor, initialization is done outside constructor
         this->m_SwitchType(C_SyvDaPeDataElementBrowse::mhs32_LastSelectedComboBoxIndex);
         q_TreeFilled = true;
      }
   }

   // Used in system definition for selecting data pool elements
   if (oq_UseInSysViews == false)
   {
      this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("Data Element")));
      this->mpc_Ui->pc_ComboBoxType->setVisible(false);
      this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("Select Data Element"));
   }

   //Avoid filling/resetting the tree if already set up (and user settings/ last known state takes priority)
   if (q_TreeFilled == false)
   {
      this->mpc_Ui->pc_TreeView->InitSv(ou32_ViewIndex, oq_ShowOnlyWriteElements, oq_ShowArrayElements,
                                        oq_ShowArrayIndexElements,
                                        oq_Show64BitValues,
                                        oq_ShowNvmLists,
                                        opc_AlreasyUsedElements, oq_UseInSysViews, ou32_SdDataLoggerUseCaseNodeIndex,
                                        ou32_SdDataLoggerUseCaseDataLoggerIndex);
   }
   //After tree was filled
   m_UpdateSelection(0);
   this->mrc_ParentDialog.SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Selection")));

   //Deactivate combo box for write widgets
   this->mpc_Ui->pc_ComboBoxType->setEnabled(!oq_ShowOnlyWriteElements);
   //Initial hide
   m_HandleHiding();

   // context menu
   this->m_SetupContextMenu(oq_MultiSelect);

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDaPeDataElementBrowse::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SyvDaPeDataElementBrowse::m_CancelClicked);
   connect(this->mpc_Ui->pc_LineEditSearch, &C_OgeLeProperties::textChanged, this,
           &C_SyvDaPeDataElementBrowse::m_OnSearch);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                         int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SyvDaPeDataElementBrowse::m_SwitchType);
   connect(this->mpc_Ui->pc_TreeView, &C_TblTreDataElementView::SigAccept, this,
           &C_SyvDaPeDataElementBrowse::m_OkClicked);
   connect(this->mpc_Ui->pc_TreeView, &C_TblTreDataElementView::SigSelectionChanged, this,
           &C_SyvDaPeDataElementBrowse::m_UpdateSelection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
C_SyvDaPeDataElementBrowse::~C_SyvDaPeDataElementBrowse(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Datapool Element"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Bus Signal"));
   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Filter"));
   this->mpc_Ui->pc_LabelSearchNoElementsFound->setText(C_GtGetText::h_GetText("No data elements found."));
   this->mpc_Ui->pc_LabelInitialDataElementsNoElements->setText(C_GtGetText::h_GetText("No data elements."));
   this->mpc_Ui->pc_LabelInitialSignalNoElements->setText(C_GtGetText::h_GetText(
                                                             "No signals defined on the connected bus."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save the user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::SaveUserSettings(void) const
{
   C_SyvDaPeDataElementBrowse::mhs32_LastSelectedComboBoxIndex = this->mpc_Ui->pc_ComboBoxType->currentIndex();
   this->mpc_Ui->pc_TreeView->SaveExpandedIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data elements

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_SyvDaPeDataElementBrowse::GetSelectedDataElements(void) const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cleans up the last view configuration as preparation for the entire deletion

   In some situations it is problematic if the model still has the mpc_InvisibleRootItem set, when it hides with
   the dialog in the background waiting to die.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::PrepareCleanUp(void)
{
   this->mpc_Ui->pc_TreeView->CleanUpLastView();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle view hiding / replacement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_HandleHiding(void) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_TreeView->setVisible(false);
      if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == true)
      {
         if (this->mpc_Ui->pc_ComboBoxType->currentIndex() == C_SyvDaPeDataElementBrowse::mhs32_INDEX_DATAPOOL_ELEMENT)
         {
            this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(true);
            this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
         }
         else
         {
            this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(true);
            this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
         }
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(true);
         this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
         this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
      }
   }
   else
   {
      this->mpc_Ui->pc_TreeView->setVisible(true);
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_OnSearch(const QString & orc_Text) const
{
   this->mpc_Ui->pc_TreeView->Search(orc_Text);
   if (orc_Text.isEmpty() == true)
   {
      m_HandleHiding();
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedDataElements().size());
   }
   else
   {
      m_HandleHiding();
      if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
      {
         this->mpc_Ui->pc_LabelSelection->setVisible(false);
      }
      else
      {
         m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedDataElements().size());
      }
      this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch displayed type

   \param[in]  os32_Index  Type index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_SwitchType(const int32_t os32_Index) const
{
   switch (os32_Index)
   {
   case C_SyvDaPeDataElementBrowse::mhs32_INDEX_DATAPOOL_ELEMENT:
      this->mpc_Ui->pc_TreeView->SwitchMode(C_TblTreDataElementModel::eDATAPOOL_ELEMENT, this->mq_ShowOnlyWriteElements,
                                            this->mq_ShowArrayElements, this->mq_ShowArrayIndexElements,
                                            this->mq_Show64BitValues, this->mpc_AlreasyUsedElements);
      break;
   case C_SyvDaPeDataElementBrowse::mhs32_INDEX_BUS_SIGNAL:
      this->mpc_Ui->pc_TreeView->SwitchMode(C_TblTreDataElementModel::eBUS_SIGNAL, false, false, false, true,
                                            this->mpc_AlreasyUsedElements);
      break;
   default:
      //Unknown
      break;
   }
   m_HandleHiding();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in]  os32_SelectionCount  Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_UpdateSelection(const int32_t os32_SelectionCount) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      if (os32_SelectionCount > 0)
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "%1 selected data element(s)")).
                                                  arg(os32_SelectionCount));
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "No selected data element")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries

   \param[in]  orq_MultiSelect   Multi select state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_SetupContextMenu(const bool & orq_MultiSelect)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_TblTreDataElementView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_TblTreDataElementView::collapseAll);

   // select all action if multi selection is allowed
   if (orq_MultiSelect == true)
   {
      this->mpc_ContextMenu->addSeparator();
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select all"),
                                       this->mpc_Ui->pc_TreeView,
                                       &C_TblTreDataElementView::selectAll,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));
   }

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SyvDaPeDataElementBrowse::customContextMenuRequested, this,
           &C_SyvDaPeDataElementBrowse::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeDataElementBrowse::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if position is on tree view
   if (this->mpc_Ui->pc_TreeView->geometry().contains(orc_Pos))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}
