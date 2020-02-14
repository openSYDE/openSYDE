//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for complete lists handling (implementation)

   Widget for complete lists handling

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeDpListsWidget.h"
#include "ui_C_SdNdeDpListsWidget.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListsWidget::C_SdNdeDpListsWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListsWidget),
   mu32_LastKnownListSelectionCount(0),
   mu32_LastKnownTableSelectionCount(0)
{
   mpc_Ui->setupUi(this);

   Clear();

   //Connect buttons
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::InsertAction);
   connect(this->mpc_Ui->pc_PushButtonCopy, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::Copy);
   connect(this->mpc_Ui->pc_PushButtonCut, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::Cut);
   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::Delete);
   connect(this->mpc_Ui->pc_PushButtonMoveDown, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::DoMoveDown);
   connect(this->mpc_Ui->pc_PushButtonMoveUp, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::DoMoveUp);
   connect(this->mpc_Ui->pc_PushButtonPaste, &QPushButton::clicked, this->mpc_Ui->pc_TreeWidget,
           &C_SdNdeDpListsTreeWidget::Paste);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigErrorChange, this,
           &C_SdNdeDpListsWidget::m_HandleErrorChange);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigSizeChange, this,
           &C_SdNdeDpListsWidget::m_HandleSizeChange);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigSelectionChanged, this,
           &C_SdNdeDpListsWidget::m_HandleSelection);
   //lint -e{64,918,1025,1703} Qt interface
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigButtonChange, this,
           &C_SdNdeDpListsWidget::m_HandleButtonChange);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigChanged, this,
           &C_SdNdeDpListsWidget::m_HandleChanged);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigSave, this,
           &C_SdNdeDpListsWidget::SigSave);
   connect(this->mpc_Ui->pc_TreeWidget, &C_SdNdeDpListsTreeWidget::SigSaveAs, this,
           &C_SdNdeDpListsWidget::SigSaveAs);

   this->InitText();
   this->m_InitButtonIcons();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListsWidget::~C_SdNdeDpListsWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::InitText(void) const
{
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"), "");
   this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Copy"), "");
   this->mpc_Ui->pc_PushButtonCut->SetToolTipInformation(C_GtGetText::h_GetText("Cut"), "");
   this->mpc_Ui->pc_PushButtonPaste->SetToolTipInformation(C_GtGetText::h_GetText("Paste"), "");
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"), "");
   this->mpc_Ui->pc_PushButtonMoveDown->SetToolTipInformation(C_GtGetText::h_GetText("Move down"), "");
   this->mpc_Ui->pc_PushButtonMoveUp->SetToolTipInformation(C_GtGetText::h_GetText("Move up"), "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear list widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::Clear(void) const
{
   //Initially deactivate all buttons
   this->mpc_Ui->pc_TreeWidget->clear();

   this->mpc_Ui->pc_PushButtonAdd->setEnabled(false);
   this->mpc_Ui->pc_PushButtonCopy->setEnabled(false);
   this->mpc_Ui->pc_PushButtonCut->setEnabled(false);
   this->mpc_Ui->pc_PushButtonDelete->setEnabled(false);
   this->mpc_Ui->pc_PushButtonMoveDown->setEnabled(false);
   this->mpc_Ui->pc_PushButtonMoveUp->setEnabled(false);
   this->mpc_Ui->pc_PushButtonPaste->setEnabled(false);

   //Hide buttons
   this->mpc_Ui->pc_PushButtonAdd->setVisible(false);
   this->mpc_Ui->pc_PushButtonCopy->setVisible(false);
   this->mpc_Ui->pc_PushButtonCut->setVisible(false);
   this->mpc_Ui->pc_PushButtonDelete->setVisible(false);
   this->mpc_Ui->pc_PushButtonMoveDown->setVisible(false);
   this->mpc_Ui->pc_PushButtonMoveUp->setVisible(false);
   this->mpc_Ui->pc_PushButtonPaste->setVisible(false);

   this->mpc_Ui->pc_SelectionLabel->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::SetDataPool(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   //Reactivate all buttons
   this->mpc_Ui->pc_PushButtonAdd->setVisible(true);
   this->mpc_Ui->pc_PushButtonCopy->setVisible(true);
   this->mpc_Ui->pc_PushButtonCut->setVisible(true);
   this->mpc_Ui->pc_PushButtonDelete->setVisible(true);
   this->mpc_Ui->pc_PushButtonMoveDown->setVisible(true);
   this->mpc_Ui->pc_PushButtonMoveUp->setVisible(true);
   this->mpc_Ui->pc_PushButtonPaste->setVisible(true);

   //Show buttons
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(true);

   this->mpc_Ui->pc_SelectionLabel->setVisible(false);

   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mpc_Ui->pc_TreeWidget->SetDataPool(oru32_NodeIndex, oru32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool list or dataelement

   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_DataElementIndex   Optional data element index (if not used set to -1)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::OpenDetail(const sint32 os32_ListIndex, const sint32 os32_DataElementIndex) const
{
   this->mpc_Ui->pc_TreeWidget->OpenDetail(os32_ListIndex, os32_DataElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize icons of buttons for all states.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_InitButtonIcons() const
{
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonCopy->SetCustomIcons("://images/system_definition/NodeEdit/lists/Copy.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyHovered.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyClicked.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyDisabled.svg");
   this->mpc_Ui->pc_PushButtonCut->SetCustomIcons("://images/system_definition/NodeEdit/lists/Cut.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutHovered.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutClicked.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutDisabled.svg");
   this->mpc_Ui->pc_PushButtonDelete->SetCustomIcons("://images/main_page_and_navi_bar/Icon_delete.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_hover.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_clicked.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_disabled.svg");
   this->mpc_Ui->pc_PushButtonMoveDown->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveDown.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveDownHovered.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveDownClicked.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveDownDisabled.svg");
   this->mpc_Ui->pc_PushButtonMoveUp->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveUp.svg",
                                                     "://images/system_definition/NodeEdit/lists/MoveUpHovered.svg",
                                                     "://images/system_definition/NodeEdit/lists/MoveUpClicked.svg",
                                                     "://images/system_definition/NodeEdit/lists/MoveUpDisabled.svg");
   this->mpc_Ui->pc_PushButtonPaste->SetCustomIcons("://images/system_definition/NodeEdit/lists/Paste.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteHovered.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteClicked.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteDisabled.svg");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error change for complete data pool

   \param[in] orq_Error Error status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_HandleErrorChange(const bool & orq_Error)
{
   Q_EMIT this->SigErrorChange(orq_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle size change for complete data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_HandleSizeChange(void)
{
   Q_EMIT this->SigSizeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle table selection change

   \param[in] oru32_Count Number of selected items
   \param[in] orq_List    Flag if items belong to list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_HandleSelection(const uint32 & oru32_Count, const bool & orq_List)
{
   QString c_Text;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex,
      this->mu32_DataPoolIndex);
   bool q_List;
   bool q_Table;

   if (orq_List == true)
   {
      this->mu32_LastKnownListSelectionCount = oru32_Count;
   }
   else
   {
      this->mu32_LastKnownTableSelectionCount = oru32_Count;
   }

   //Which mode
   if (this->mu32_LastKnownListSelectionCount > 0)
   {
      q_List = true;
   }
   else
   {
      q_List = false;
   }
   if (this->mu32_LastKnownTableSelectionCount > 0)
   {
      q_Table = true;
   }
   else
   {
      q_Table = false;
   }
   //Handle selection label
   if (((q_List == false) && (q_Table == false)) || ((q_List == true) && (q_Table == true)))
   {
      c_Text = "";
   }
   else
   {
      if (q_List == true)
      {
         if (this->mu32_LastKnownListSelectionCount == 1)
         {
            c_Text = QString(C_GtGetText::h_GetText("1 List selected"));
         }
         else
         {
            c_Text = QString(C_GtGetText::h_GetText("%1 Lists selected")).arg(this->mu32_LastKnownListSelectionCount);
         }
      }
      else
      {
         const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(pc_DataPool->e_Type);
         if (this->mu32_LastKnownTableSelectionCount == 1)
         {
            //Translation: 1: Data element type
            c_Text = QString(C_GtGetText::h_GetText("1 %1 selected")).arg(c_Type);
         }
         else
         {
            //Translation: 1: Number of selected items 2: Data element type
            c_Text =
               QString(C_GtGetText::h_GetText("%1 %2s selected")).arg(this->mu32_LastKnownTableSelectionCount).arg(
                  c_Type);
         }
      }
   }
   this->mpc_Ui->pc_SelectionLabel->setText(c_Text);
   this->mpc_Ui->pc_SelectionLabel->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set buttons status

   \param[in] orq_AddActive      Push button add active
   \param[in] orq_CutActive      Push button cut active
   \param[in] orq_CopyActive     Push button copy active
   \param[in] orq_PasteActive    Push button paste active
   \param[in] orq_DeleteActive   Push button delete active
   \param[in] orq_MoveDownActive Push button move down active
   \param[in] orq_MoveUpActive   Push button move up active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive,
                                                const bool & orq_CopyActive, const bool & orq_PasteActive,
                                                const bool & orq_DeleteActive, const bool & orq_MoveDownActive,
                                                const bool & orq_MoveUpActive) const
{
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(orq_AddActive);
   this->mpc_Ui->pc_PushButtonCut->setEnabled(orq_CutActive);
   this->mpc_Ui->pc_PushButtonCopy->setEnabled(orq_CopyActive);
   this->mpc_Ui->pc_PushButtonPaste->setEnabled(orq_PasteActive);
   this->mpc_Ui->pc_PushButtonDelete->setEnabled(orq_DeleteActive);
   this->mpc_Ui->pc_PushButtonMoveDown->setEnabled(orq_MoveDownActive);
   this->mpc_Ui->pc_PushButtonMoveUp->setEnabled(orq_MoveUpActive);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change detected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListsWidget::m_HandleChanged(void)
{
   Q_EMIT this->SigChanged();
}
