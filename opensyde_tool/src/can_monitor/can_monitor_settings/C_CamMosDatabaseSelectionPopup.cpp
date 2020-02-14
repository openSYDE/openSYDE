//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget for filter ID selection from database (implementation)

   Popup widget for filter ID selection from database

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_CamMosDatabaseSelectionPopup.h"
#include "ui_C_CamMosDatabaseSelectionPopup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     oq_AllowMultiSelect Flag to allow multi select for the message selection
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseSelectionPopup::C_CamMosDatabaseSelectionPopup(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                               const bool oq_AllowMultiSelect) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CamMosDatabaseSelectionPopup),
   mpc_ContextMenu(NULL),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // initialize label colors
   this->mpc_Ui->pc_LabelInitialSignalNoElements->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelInitialSignalNoElements->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelInitialSignalNoElements->SetFontPixel(11);

   this->mpc_Ui->pc_LabelSearchNoElementsFound->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelSearchNoElementsFound->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSearchNoElementsFound->SetFontPixel(11);

   this->mpc_Ui->pc_LabelSelection->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelSelection->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSelection->SetFontPixel(11);

   // hide search-nothing-found label
   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);

   //Selection handling
   if (oq_AllowMultiSelect == true)
   {
      this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   }
   else
   {
      this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::SingleSelection);
   }
   //Data handling
   this->mpc_Ui->pc_TreeView->Init();

   //After tree was filled
   m_UpdateSelection(0);

   //Initial hide
   m_HandleHiding();

   // context menu
   this->m_SetupContextMenu(oq_AllowMultiSelect);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_CamMosDatabaseSelectionPopup::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_CamMosDatabaseSelectionPopup::m_CancelClicked);
   connect(this->mpc_Ui->pc_LineEditSearch, &C_OgeLeProperties::textChanged, this,
           &C_CamMosDatabaseSelectionPopup::m_OnSearch);
   connect(this->mpc_Ui->pc_TreeView, &C_CamMosDatabaseSelectionView::SigAccept, this,
           &C_CamMosDatabaseSelectionPopup::m_OkClicked);
   connect(this->mpc_Ui->pc_TreeView, &C_CamMosDatabaseSelectionView::SigSelectionChanged, this,
           &C_CamMosDatabaseSelectionPopup::m_UpdateSelection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseSelectionPopup::~C_CamMosDatabaseSelectionPopup(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Message"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Selection"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Select message"));

   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Filter"));
   this->mpc_Ui->pc_LabelSearchNoElementsFound->setText(C_GtGetText::h_GetText("No messages found."));
   this->mpc_Ui->pc_LabelInitialSignalNoElements->setText(C_GtGetText::h_GetText(
                                                             "No messages found. \n\n"
                                                             "Possible reasons:\n"
                                                             "   - Database deactivated\n"
                                                             "   - Database not loaded yet\n"
                                                             "   - Database loading failed\n"
                                                             "   - Database doesn't contain messages"));

   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialSignalNoElements->setTitle("");

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save the user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_TreeView->SaveExpandedIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data elements

   array:
   0: database name
   1: message name

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::array<QString, 2> > C_CamMosDatabaseSelectionPopup::GetSelectedDataElements(void)
const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_CamMosDatabaseSelectionPopup::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle view hiding / replacement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_HandleHiding(void) const
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
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in] orc_Text String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_OnSearch(const QString & orc_Text) const
{
   this->mpc_Ui->pc_TreeView->Search(orc_Text);
   if (orc_Text.isEmpty() == true)
   {
      this->mpc_Ui->pc_TreeView->collapseAll();
      m_HandleHiding();
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedDataElements().size());
   }
   else
   {
      this->mpc_Ui->pc_TreeView->expandAll();
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
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in] osn_SelectionCount Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_UpdateSelection(const sintn osn_SelectionCount) const
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
         if (osn_SelectionCount > 1)
         {
            this->mpc_Ui->pc_LabelSelection->setText(QString(C_GtGetText::h_GetText("%1 selected messages")).arg(
                                                        osn_SelectionCount));
         }
         else
         {
            this->mpc_Ui->pc_LabelSelection->setText(QString(C_GtGetText::h_GetText("1 selected message")));
         }
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(QString(C_GtGetText::h_GetText("No selected message")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_SetupContextMenu(const bool & orq_MultiSelect)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_CamMosDatabaseSelectionView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"),
                                    this->mpc_Ui->pc_TreeView,
                                    &C_CamMosDatabaseSelectionView::collapseAll);

   // select all action if multi selection is allowed
   if (orq_MultiSelect == true)
   {
      this->mpc_ContextMenu->addSeparator();
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select all"),
                                       this->mpc_Ui->pc_TreeView,
                                       &C_CamMosDatabaseSelectionView::selectAll,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));
   }

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_CamMosDatabaseSelectionPopup::customContextMenuRequested, this,
           &C_CamMosDatabaseSelectionPopup::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionPopup::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   // check if position is on tree view
   if (this->mpc_Ui->pc_TreeView->geometry().contains(orc_Pos))
   {
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}
