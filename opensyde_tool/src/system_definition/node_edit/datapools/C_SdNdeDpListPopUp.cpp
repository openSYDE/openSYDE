//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List edit pop up widget

   List edit pop up widget.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "constants.h"
#include "C_SdNdeDpListPopUp.h"
#include "ui_C_SdNdeDpListPopUp.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "TGLUtils.h"
#include "C_OgeWiUtil.h"
#include "C_GiSyColorSelectWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent           Reference to parent
   \param[in]     oru32_NodeIndex      Node index
   \param[in]     oru32_DataPoolIndex  Data pool index
   \param[in]     oru32_ListIndex      List index
   \param[in,out] opc_ModelViewManager Model view manager
   \param[in,out] opc_TreeWidget       Tree widget for notification events
   \param[in,out] opc_UndoManager      Undo manager
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListPopUp::C_SdNdeDpListPopUp(C_OgePopUpDialog & orc_Parent, const uint32 & oru32_NodeIndex,
                                       const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex,
                                       C_SdNdeDpListModelViewManager * const opc_ModelViewManager,
                                       QTreeWidget * const opc_TreeWidget,
                                       C_SdNdeUnoDataPoolManager * const opc_UndoManager, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListPopUp),
   mrc_Parent(orc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_ListIndex(oru32_ListIndex),
   mpc_ModelViewManager(opc_ModelViewManager),
   mpc_TreeWidget(opc_TreeWidget),
   mpc_UndoManager(opc_UndoManager),
   ms32_UndoStartCount(0),
   mq_SaveAsAfterClose(false)
{
   this->mpc_Ui->setupUi(this);

   //Pop up
   this->mrc_Parent.SetWidget(this);
   this->mrc_Parent.HideTitle();

   //Debug text
   this->mpc_Ui->pc_GroupBoxHeader->setTitle("");

   //undo
   if (this->mpc_UndoManager != NULL)
   {
      this->ms32_UndoStartCount = static_cast<sint32>(this->mpc_UndoManager->index());
   }

   //-- Init header widget --
   this->mpc_Ui->pc_WidgetHeader->SetInitParameters(this->mpc_TreeWidget, this->mpc_UndoManager,
                                                    this->mpc_ModelViewManager, this->mu32_NodeIndex,
                                                    this->mu32_DataPoolIndex, this->mu32_ListIndex);
   this->mpc_Ui->pc_WidgetHeader->SetIndex(this->mu32_ListIndex);

   //Config
   this->mpc_Ui->pc_WidgetHeader->PrepareExpandedMode();

   //Connects
   connect(this->mpc_Ui->pc_WidgetHeader, &C_SdNdeDpListHeaderWidget::SigClose, this,
           &C_SdNdeDpListPopUp::m_Close);

   //-- Init table widget --
   this->mpc_Ui->pc_WidgetTable->SetTreeWidgetAndUndoManager(this->mpc_TreeWidget, this->mpc_UndoManager);
   this->mpc_Ui->pc_WidgetTable->SetModelViewManager(this->mpc_ModelViewManager);
   this->mpc_Ui->pc_WidgetTable->SetList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   //Config

   //Connects
   connect(this->mpc_Ui->pc_WidgetTable, &C_SdNdeDpListTableWidget::SigSelectionChanged, this,
           &C_SdNdeDpListPopUp::m_HandleSelection);
   connect(this->mpc_Ui->pc_WidgetTable, &C_SdNdeDpListTableWidget::SigButtonChange, this,
           &C_SdNdeDpListPopUp::m_HandleButtonChange);
   connect(this->mpc_Ui->pc_WidgetTable, &C_SdNdeDpListTableWidget::SigSizeChangePossible,
           this, &C_SdNdeDpListPopUp::UpdateHeader);
   connect(this->mpc_Ui->pc_WidgetTable, &C_SdNdeDpListTableWidget::SigErrorChangePossible,
           this, &C_SdNdeDpListPopUp::UpdateHeader);

   //-- Init buttons --
   Clear();

   //Connect buttons
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::InsertAction);
   connect(this->mpc_Ui->pc_PushButtonCopy, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::Copy);
   connect(this->mpc_Ui->pc_PushButtonCut, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::Cut);
   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::Delete);
   connect(this->mpc_Ui->pc_PushButtonMoveDown, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::DoMoveDown);
   connect(this->mpc_Ui->pc_PushButtonMoveUp, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::DoMoveUp);
   connect(this->mpc_Ui->pc_PushButtonPaste, &QPushButton::clicked, this->mpc_Ui->pc_WidgetTable,
           &C_SdNdeDpListTableWidget::Paste);

   this->InitText();
   this->m_InitButtonIcons();

   //Initial focus
   this->mpc_Ui->pc_WidgetTable->SelectDataElement(0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListPopUp::~C_SdNdeDpListPopUp(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::InitText(void) const
{
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex,
      this->mu32_DataPoolIndex);

   tgl_assert(pc_DataPool != NULL);
   if (pc_DataPool != NULL)
   {
      //Generic tooltips are used.
      //const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(pc_DataPool->e_Type);
      this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                            C_GtGetText::h_GetText("Add new list"));
      this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Copy"),
                                                             C_GtGetText::h_GetText("Copy selected list(s)"));
      this->mpc_Ui->pc_PushButtonCut->SetToolTipInformation(C_GtGetText::h_GetText("Cut"),
                                                            C_GtGetText::h_GetText("Cut selected list(s)"));
      this->mpc_Ui->pc_PushButtonPaste->SetToolTipInformation(C_GtGetText::h_GetText("Paste"),
                                                              C_GtGetText::h_GetText("Paste list(s)"));
      this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                               C_GtGetText::h_GetText("Delete selected list(s)"));
      this->mpc_Ui->pc_PushButtonMoveDown->SetToolTipInformation(C_GtGetText::h_GetText("Move down"),
                                                                 C_GtGetText::h_GetText("Move selected lists(s) down"));
      this->mpc_Ui->pc_PushButtonMoveUp->SetToolTipInformation(C_GtGetText::h_GetText("Move up"),
                                                               C_GtGetText::h_GetText("Move selected list(s) up"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear list widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::Clear(void) const
{
   //Initially deactivate all buttons
   this->mpc_Ui->pc_PushButtonCopy->setEnabled(false);
   this->mpc_Ui->pc_PushButtonCut->setEnabled(false);
   this->mpc_Ui->pc_PushButtonDelete->setEnabled(false);
   this->mpc_Ui->pc_PushButtonMoveDown->setEnabled(false);
   this->mpc_Ui->pc_PushButtonMoveUp->setEnabled(false);
   this->mpc_Ui->pc_PushButtonPaste->setEnabled(false);

   //Hide label
   this->mpc_Ui->pc_SelectionLabel->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Manually trigger header update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::UpdateHeader(void) const
{
   this->mpc_Ui->pc_WidgetHeader->SetIndex(this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get save as after close flag

   \return
   Save as after close flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListPopUp::GetSaveAsAfterClose(void) const
{
   return mq_SaveAsAfterClose;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Manually draw background

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   //draw background
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this, &c_Painter);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle list paste

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (this->mpc_UndoManager != NULL)
   {
      //Debug commands
      if (((opc_Event->modifiers().testFlag(Qt::ShiftModifier) == true) &&
           (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)) &&
          (opc_Event->modifiers().testFlag(Qt::AltModifier) == true))
      {
         switch (opc_Event->key())
         {
         case Qt::Key_O:
            //Undo redo command view
            this->mpc_UndoManager->ToggleCommandDisplay();
            break;
         default:
            break;
         }
      }
      //Standard commands
      switch (opc_Event->key())
      {
      case Qt::Key_Z:
         if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
         {
            if (this->ms32_UndoStartCount < static_cast<sint32>(this->mpc_UndoManager->index()))
            {
               this->mpc_UndoManager->DoUndo();
            }
         }
         break;
      case Qt::Key_Y:
         if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
         {
            this->mpc_UndoManager->DoRedo();
         }
         break;
      case Qt::Key_S:
         if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
         {
            this->mq_SaveAsAfterClose = true;
            this->mrc_Parent.accept();
         }
         else
         {
            Q_EMIT this->SigSave();
         }
         break;
      case Qt::Key_F11:
         this->mrc_Parent.accept();
         break;
      case Qt::Key_F12:
         this->mq_SaveAsAfterClose = true;
         this->mrc_Parent.accept();
         break;
      case Qt::Key_F8:
         this->m_OpenColorPicker();
         this->mrc_Parent.accept();
         break;
      default:
         //Nothing to do
         break;
      }
   }

   QWidget::keyPressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize icons of buttons for all states.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::m_InitButtonIcons(void) const
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
/*! \brief   Close pop up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::m_Close(void)
{
   this->mrc_Parent.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle table selection change

   \param[in] oru32_ListIndex List index
   \param[in] oru32_Count     Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::m_HandleSelection(const uint32 & oru32_ListIndex, const uint32 & oru32_Count) const
{
   QString c_Text;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex,
      this->mu32_DataPoolIndex);

   tgl_assert(oru32_ListIndex == this->mu32_ListIndex);
   tgl_assert(pc_DataPool != NULL);
   if (pc_DataPool != NULL)
   {
      bool q_Visible = true;
      const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(pc_DataPool->e_Type);
      if (oru32_Count == 0)
      {
         q_Visible = false;
      }
      else if (oru32_Count == 1)
      {
         //Translation: 1: Data element type
         c_Text = static_cast<QString>(C_GtGetText::h_GetText("1 %1 selected")).arg(c_Type);
      }
      else
      {
         //Translation: 1: Number of selected items 2: Data element type
         c_Text =
            static_cast<QString>(C_GtGetText::h_GetText("%1 %2s selected")).arg(oru32_Count).arg(c_Type);
      }
      this->mpc_Ui->pc_SelectionLabel->setText(c_Text);
      this->mpc_Ui->pc_SelectionLabel->setVisible(q_Visible);
   }
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
void C_SdNdeDpListPopUp::m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive,
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
/*! \brief   Open color picker
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListPopUp::m_OpenColorPicker(void)
{
   QPointer<C_OgePopUpDialog> const c_Popup = new C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, mc_STYLE_GUIDE_COLOR_7);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   // open color picker dialog
   if (c_Popup->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      pc_ColorWidget->ChooseSelectedColor();
   }

   if (c_Popup != NULL)
   {
      c_Popup->HideOverlay();
      c_Popup->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_ColorWidget and the Qt memory management
