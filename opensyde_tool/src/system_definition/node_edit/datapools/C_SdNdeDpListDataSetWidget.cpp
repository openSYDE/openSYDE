//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set edit widget

   Data set edit widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdNdeDpListDataSetWidget.hpp"
#include "ui_C_SdNdeDpListDataSetWidget.h"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Data pool index
   \param[in]     oru32_ListIndex     List index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListDataSetWidget::C_SdNdeDpListDataSetWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                       const uint32_t & oru32_NodeIndex,
                                                       const uint32_t & oru32_DataPoolIndex,
                                                       const uint32_t & oru32_ListIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListDataSetWidget),
   mpc_ContextMenu(NULL),
   mpc_ActionAdd(NULL),
   mpc_ActionDelete(NULL),
   mpc_ActionCopy(NULL),
   mpc_ActionPaste(NULL),
   mpc_ActionCut(NULL),
   mpc_ActionMoveLeft(NULL),
   mpc_ActionMoveRight(NULL),
   mrc_Parent(orc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_ListIndex(oru32_ListIndex)
{
   const stw::opensyde_core::C_OscNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(oru32_NodeIndex,
                                                          oru32_DataPoolIndex,
                                                          oru32_ListIndex);

   this->mpc_Ui->setupUi(this);
   this->mrc_Parent.SetWidget(this);
   InitStaticNames();
   this->mpc_Ui->pc_TableView->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);

   //Deactivate debug title
   this->mpc_Ui->pc_GroupBoxEmpty->setTitle("");

   //Custom context menu
   m_SetupContextMenu();

   // Init icons
   m_InitButtonIcons();

   //Initial display
   if (pc_List != NULL)
   {
      m_OnDataSetCountChange(pc_List->c_DataSets.size());
   }
   m_OnDataSetSelectionChange(0);

   //Connects
   connect(this->mpc_Ui->pc_BushButtonOk, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDpListDataSetWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDpListDataSetWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::SigColumnCountChange, this,
           &C_SdNdeDpListDataSetWidget::m_OnDataSetCountChange);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::SigSelectionChanged, this,
           &C_SdNdeDpListDataSetWidget::m_OnDataSetSelectionChange);

   //Connect buttons
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SdNdeDpListDataSetWidget::m_DoInsert);
   connect(this->mpc_Ui->pc_PushButtonCopy, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::Copy);
   connect(this->mpc_Ui->pc_PushButtonCut, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::Cut);
   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::Delete);
   connect(this->mpc_Ui->pc_PushButtonMoveRight, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::DoMoveRight);
   connect(this->mpc_Ui->pc_PushButtonMoveLeft, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::DoMoveLeft);
   connect(this->mpc_Ui->pc_PushButtonPaste, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDpListDataSetView::Paste);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::SigButtonChange, this,
           &C_SdNdeDpListDataSetWidget::m_HandleButtonChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListDataSetWidget::~C_SdNdeDpListDataSetWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::InitStaticNames(void) const
{
   const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   if (pc_List != NULL)
   {
      mrc_Parent.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("List %1")).arg(pc_List->c_Name.c_str()));
   }
   mrc_Parent.SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Dataset Configuration")));
   this->mpc_Ui->pc_LabelReplacement->setText(C_GtGetText::h_GetText(
                                                 "No Dataset is declared, add any via the '+' button"));
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                         C_GtGetText::h_GetText("Add new Dataset"));
   this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Copy"),
                                                          C_GtGetText::h_GetText("Copy selected Dataset(s)"));
   this->mpc_Ui->pc_PushButtonCut->SetToolTipInformation(C_GtGetText::h_GetText("Cut"),
                                                         C_GtGetText::h_GetText("Cut selected Dataset(s)"));
   this->mpc_Ui->pc_PushButtonPaste->SetToolTipInformation(C_GtGetText::h_GetText("Paste"),
                                                           C_GtGetText::h_GetText("Paste Dataset(s)"));
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                            C_GtGetText::h_GetText("Delete selected Dataset(s)"));
   this->mpc_Ui->pc_PushButtonMoveLeft->SetToolTipInformation(C_GtGetText::h_GetText("Move left"),
                                                              C_GtGetText::h_GetText("Move selected Dataset(s) left"));
   this->mpc_Ui->pc_PushButtonMoveRight->SetToolTipInformation(C_GtGetText::h_GetText("Move right"),
                                                               C_GtGetText::h_GetText("Move selected Dataset(s) right"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes
*/
//----------------------------------------------------------------------------------------------------------------------
QUndoCommand * C_SdNdeDpListDataSetWidget::TakeUndoCommand(void) const
{
   return this->mpc_Ui->pc_TableView->TakeUndoCommand();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model view manager

   \param[in,out] opc_Value Model view manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::SetModelViewManager(C_SdNdeDpListModelViewManager * const opc_Value) const
{
   this->mpc_Ui->pc_TableView->SetModelViewManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle list paste

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   //Handle all enter key cases manually
   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_Event->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_Event->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_Event->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_Event->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_Parent.accept();
      }
      else
      {
         q_CallOrig = false;
      }
   }
   else
   {
      if (this->mpc_Ui->pc_GroupBoxGeneral->isAncestorOf(this->focusWidget()) == true)
      {
         const int32_t s32_Key = opc_Event->key();
         switch (s32_Key)
         {
         case Qt::Key_C:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Copy();
               opc_Event->accept();
            }
            break;
         case Qt::Key_X:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Cut();
               opc_Event->accept();
            }
            break;
         case Qt::Key_V:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->m_DoPaste();
               opc_Event->accept();
            }
            break;
         case Qt::Key_Delete:
            q_CallOrig = false;
            this->mpc_Ui->pc_TableView->Delete();
            opc_Event->accept();
            break;
         case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
         case Qt::Key_Plus:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->m_DoInsert();
               opc_Event->accept();
            }
            break;
         case Qt::Key_Left:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->DoMoveLeft();
               opc_Event->accept();
            }
            break;
         case Qt::Key_Right:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->DoMoveRight();
               opc_Event->accept();
            }
            break;
         default:
            //Nothing to do
            break;
         }
      }
   }
   if (q_CallOrig == true)
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether adding additional data sets would breach the maximum number

   If not show a corresponding message box and let caller know.

   \param[in] ou32_NumberOfNewDataSets   number of additional data sets that shall be added

   \return
   true:   still space for additional data sets
   false:  not enough space for additional data sets
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListDataSetWidget::m_IsThereStillSpaceForDataSets(const uint32_t ou32_NumberOfNewDataSets)
{
   bool q_Return = true;

   if (((static_cast<uint32_t>(this->mpc_Ui->pc_TableView->model()->columnCount()) + ou32_NumberOfNewDataSets)) >
       mu32_NODE_DATA_SET_PER_LIST_MAX)
   {
      C_OgeWiCustomMessage c_MessageBox(this);

      c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Only %1 Datasets allowed per Datapool list.")).
                                  arg(mu32_NODE_DATA_SET_PER_LIST_MAX));
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();

      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize icons of buttons for all states.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_InitButtonIcons(void) const
{
   // TODO icon states: svg icons move left and move right; 4 different states
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
   this->mpc_Ui->pc_PushButtonMoveLeft->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg");
   this->mpc_Ui->pc_PushButtonMoveRight->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg");
   this->mpc_Ui->pc_PushButtonPaste->SetCustomIcons("://images/system_definition/NodeEdit/lists/Paste.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteHovered.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteClicked.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteDisabled.svg");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On ok clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_OkClicked(void)
{
   this->mrc_Parent.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On cancel clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_CancelClicked(void)
{
   this->mrc_Parent.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set buttons status

   \param[in] orq_AddActive       Push button add active
   \param[in] orq_CutActive       Push button cut active
   \param[in] orq_CopyActive      Push button copy active
   \param[in] orq_PasteActive     Push button paste active
   \param[in] orq_DeleteActive    Push button delete active
   \param[in] orq_MoveLeftActive  Push button move left active
   \param[in] orq_MoveRightActive Push button move right active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive,
                                                      const bool & orq_CopyActive, const bool & orq_PasteActive,
                                                      const bool & orq_DeleteActive, const bool & orq_MoveLeftActive,
                                                      const bool & orq_MoveRightActive)
{
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(orq_AddActive);
   if (this->mpc_ActionAdd != NULL)
   {
      this->mpc_ActionAdd->setEnabled(orq_AddActive);
   }
   this->mpc_Ui->pc_PushButtonCut->setEnabled(orq_CutActive);
   if (this->mpc_ActionCut != NULL)
   {
      this->mpc_ActionCut->setEnabled(orq_CutActive);
   }
   this->mpc_Ui->pc_PushButtonCopy->setEnabled(orq_CopyActive);
   if (this->mpc_ActionCopy != NULL)
   {
      this->mpc_ActionCopy->setEnabled(orq_CopyActive);
   }
   this->mpc_Ui->pc_PushButtonPaste->setEnabled(orq_PasteActive);
   if (this->mpc_ActionPaste != NULL)
   {
      this->mpc_ActionPaste->setEnabled(orq_PasteActive);
   }
   this->mpc_Ui->pc_PushButtonDelete->setEnabled(orq_DeleteActive);
   if (this->mpc_ActionDelete != NULL)
   {
      this->mpc_ActionDelete->setEnabled(orq_DeleteActive);
   }
   this->mpc_Ui->pc_PushButtonMoveLeft->setEnabled(orq_MoveLeftActive);
   if (this->mpc_ActionMoveLeft != NULL)
   {
      this->mpc_ActionMoveLeft->setEnabled(orq_MoveLeftActive);
   }
   this->mpc_Ui->pc_PushButtonMoveRight->setEnabled(orq_MoveRightActive);
   if (this->mpc_ActionMoveRight != NULL)
   {
      this->mpc_ActionMoveRight->setEnabled(orq_MoveRightActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data set count change

   \param[in] ors32_NewColumnCount New column count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_OnDataSetCountChange(const int32_t & ors32_NewColumnCount) const
{
   if (ors32_NewColumnCount > 0)
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(true);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data set selction change

   \param[in] oru32_SelectionCount Selected item count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_OnDataSetSelectionChange(const uint32_t & oru32_SelectionCount) const
{
   QString c_Text;

   //Handle selection label
   if (oru32_SelectionCount == 0)
   {
      c_Text = "";
   }
   else if (oru32_SelectionCount == 1)
   {
      c_Text = static_cast<QString>(C_GtGetText::h_GetText("1 Dataset selected"));
   }
   else
   {
      c_Text = static_cast<QString>(C_GtGetText::h_GetText("%1 Datasets selected")).arg(oru32_SelectionCount);
   }
   this->mpc_Ui->pc_SelectionLabel->setText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw::opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Add new Dataset"), this, &C_SdNdeDpListDataSetWidget::m_DoInsert,
      static_cast<int32_t>(Qt::CTRL) +
      static_cast<int32_t>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionCut = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Cut"), this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::Cut,
      static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_X));
   this->mpc_ActionCopy = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Copy"), this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::Copy,
      static_cast<int32_t>(Qt::CTRL) +
      static_cast<int32_t>(Qt::Key_C));
   this->mpc_ActionPaste = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Paste"), this, &C_SdNdeDpListDataSetWidget::m_DoPaste,
      static_cast<int32_t>(Qt::CTRL) +
      static_cast<int32_t>(Qt::Key_V));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionMoveLeft = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Left"), this->mpc_Ui->pc_TableView,
                                                               &C_SdNdeDpListDataSetView::DoMoveLeft,
                                                               static_cast<int32_t>(Qt::CTRL) +
                                                               static_cast<int32_t>(Qt::Key_Left));
   this->mpc_ActionMoveRight = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                   "Move Right"), this->mpc_Ui->pc_TableView,
                                                                &C_SdNdeDpListDataSetView::DoMoveRight,
                                                                static_cast<int32_t>(Qt::CTRL) +
                                                                static_cast<int32_t>(Qt::Key_Right));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionDelete = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Delete"), this->mpc_Ui->pc_TableView, &C_SdNdeDpListDataSetView::Delete,
      static_cast<int32_t>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeDpListDataSetWidget::customContextMenuRequested, this,
           &C_SdNdeDpListDataSetWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_Global = this->mapToGlobal(orc_Pos);

   if (this->mpc_Ui->pc_GroupBoxGeneral->rect().contains(this->mpc_Ui->pc_GroupBoxGeneral->mapFromGlobal(c_Global)) ==
       true)
   {
      this->mpc_ContextMenu->popup(c_Global);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle insert action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_DoInsert(void)
{
   if (m_IsThereStillSpaceForDataSets(1U) == true)
   {
      this->mpc_Ui->pc_TableView->Insert(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paste action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListDataSetWidget::m_DoPaste(void)
{
   std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> c_OscNames;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > c_OscDataSetValues;

   //how many items does the user want to paste ?
   if (C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromClipBoard(c_OscNames,
                                                                      c_OscDataSetValues) == stw::errors::C_NO_ERR)
   {
      if (m_IsThereStillSpaceForDataSets(c_OscNames.size()) == true)
      {
         this->mpc_Ui->pc_TableView->Paste();
      }
   }
   else
   {
      //not a parseable data set information; makes not sense to try pasting anything
   }
}
