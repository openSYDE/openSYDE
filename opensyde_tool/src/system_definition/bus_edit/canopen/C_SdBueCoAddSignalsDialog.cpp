//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog for choosing and adding signals to CANopen manager

   Pop up dialog for choosing and adding signals to CANopen manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_SdBueCoAddSignalsDialog.h"
#include "ui_C_SdBueCoAddSignalsDialog.h"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent       Reference to parent
   \param[in]      orc_MessageId    Message id
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueCoAddSignalsDialog::C_SdBueCoAddSignalsDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                     const C_OSCCanMessageIdentificationIndices & orc_MessageId) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdBueCoAddSignalsDialog),
   mpc_ContextMenu(NULL),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);

   // Deactivate debug string
   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setTitle("");

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mpc_Ui->pc_TreeView->SetIndex(orc_MessageId);

   //After tree was filled
   m_UpdateSelection(0);

   // Initial hide
   m_HandleHiding();

   // context menu
   this->m_SetupContextMenu();

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdBueCoAddSignalsDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdBueCoAddSignalsDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_LineEditSearch, &C_OgeLeProperties::textChanged, this,
           &C_SdBueCoAddSignalsDialog::m_OnSearch);
   connect(this->mpc_Ui->pc_TreeView, &C_SdBueCoAddSignalsView::SigSelectionChanged, this,
           &C_SdBueCoAddSignalsDialog::m_UpdateSelection);
   connect(this->mpc_Ui->pc_TreeView, &C_SdBueCoAddSignalsView::SigAccept, this,
           &C_SdBueCoAddSignalsDialog::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
C_SdBueCoAddSignalsDialog::~C_SdBueCoAddSignalsDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Add Signals"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Select Signal from Object Dictionary"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Select Signal from Object Dictionary"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Filter"));
   this->mpc_Ui->pc_LabelSearchNoElementsFound->setText(C_GtGetText::h_GetText("No signals found."));
   this->mpc_Ui->pc_LabelInitialSignalNoElements->setText(C_GtGetText::h_GetText(
                                                             "No signals defined on the connected bus."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected signals

   \return
   Current selected signals
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_SdBueCoAddSignalsResultEntry> C_SdBueCoAddSignalsDialog::GetSelectedSignals(void) const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedSignals();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cleans up the last view configuration as preparation for the entire deletion

   In some situations it is problematic if the model still has the mpc_InvisibleRootItem set, when it hides with
   the dialog in the background waiting to die.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1762} WIP
void C_SdBueCoAddSignalsDialog::PrepareCleanUp(void)
{
   this->mpc_Ui->pc_TreeView->PrepareCleanUp();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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
void C_SdBueCoAddSignalsDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle view hiding / replacement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_HandleHiding(void) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_TreeView->setVisible(false);
      if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == true)
      {
         this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(true);
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(true);
         this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
      }
   }
   else
   {
      this->mpc_Ui->pc_TreeView->setVisible(true);
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in]  orc_Text    String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_OnSearch(const QString & orc_Text) const
{
   this->mpc_Ui->pc_TreeView->Search(orc_Text);
   if (orc_Text.isEmpty() == true)
   {
      m_HandleHiding();
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedSignals().size());
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
         m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedSignals().size());
      }
      this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in]  osn_SelectionCount   Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_UpdateSelection(const sintn osn_SelectionCount) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      if (osn_SelectionCount > 0)
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "%1 selected signal(s)")).
                                                  arg(osn_SelectionCount));
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                          "No selected signal")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_SdBueCoAddSignalsView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_SdBueCoAddSignalsView::collapseAll);

   // select all action
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_SdBueCoAddSignalsView::selectAll,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdBueCoAddSignalsDialog::customContextMenuRequested, this,
           &C_SdBueCoAddSignalsDialog::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsDialog::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if position is on tree view
   if (this->mpc_Ui->pc_TreeView->geometry().contains(orc_Pos))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}
