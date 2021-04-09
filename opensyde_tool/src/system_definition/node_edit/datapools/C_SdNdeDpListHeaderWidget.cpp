//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list header (implementation)

   Widget to display one data pool list header

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>

#include <QApplication>
#include <QPointer>
#include "stwerrors.h"
#include "constants.h"
#include "C_SdNdeDpListHeaderWidget.h"
#include "ui_C_SdNdeDpListHeaderWidget.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpListsTreeWidget.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDpListDataSetWidget.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_OSCUtils.h"
#include "C_SdNdeDpListPopUp.h"
#include "TGLUtils.h"
#include "C_SdNdeDpListCommentDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_errors;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent           Optional pointer to parent
   \param[in,out] opc_ListWidgetItem   According list widget item
   \param[in,out] opc_UndoManager      Undo manager
   \param[in,out] opc_ModelViewManager Model view manager
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     ou32_DataPoolIndex   Data pool index
   \param[in]     ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListHeaderWidget::C_SdNdeDpListHeaderWidget(QWidget * const opc_Parent, QTreeWidget * const opc_ListWidget,
                                                     C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                                                     C_SdNdeDpListModelViewManager * const opc_ModelViewManager,
                                                     const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListHeaderWidget),
   mpc_TreeWidget(opc_ListWidget),
   mpc_UndoManager(opc_UndoManager),
   mpc_ModelViewManager(opc_ModelViewManager),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_DataPoolIndex(ou32_DataPoolIndex),
   mu32_ListIndex(ou32_ListIndex),
   mq_InitActive(true),
   mq_DataSetError(false)
{
   QSizePolicy c_SizePolicy;

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_UsageWidget->SetToolTipWidgetName(static_cast<QString>(C_GtGetText::h_GetText("List")));
   this->NotifySelection(false);

   //Init buttons
   this->mpc_Ui->pc_PushButtonFullScreen->SetCustomIcons("://images/system_definition/NodeEdit/lists/Fullscreen.svg",
                                                         "://images/system_definition/NodeEdit/lists/FullscreenHovered.svg",
                                                         "://images/system_definition/NodeEdit/lists/FullscreenClicked.svg",
                                                         "://images/system_definition/NodeEdit/lists/FullscreenDisabled.svg");
   this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                     "://images/IconOpenListHovered.svg",
                                                     "://images/IconOpenListClicked.svg",
                                                     "://images/IconOpenListDisabledBright.svg");
   this->mpc_Ui->pc_PushButtonComment->SetSvg("://images/system_definition/IconCommentAdd.svg");
   this->mpc_Ui->pc_PushButtonDataset->SetSvg("://images/system_definition/NodeEdit/datasets/IconDatasets0.svg");

   RegisterExpandOrCollapse(false);

   // Error icon
   this->mpc_Ui->pc_LabelListError->setText("");
   this->mpc_Ui->pc_LabelListError->setPixmap(QIcon("://images/Error_iconV2.svg").pixmap(mc_ICON_SIZE_20));
   c_SizePolicy = this->mpc_Ui->pc_LabelListError->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_LabelListError->setSizePolicy(c_SizePolicy); // keep space free when invisible

   //Init labels
   InitStaticNames();
   this->mpc_Ui->pc_LabelComment->SetFontPixel(14);
   this->mpc_Ui->pc_LabelComment->SetForegroundColor(6);

   //Deactivate debug label
   this->mpc_Ui->pc_GroupBoxUsage->setTitle("");

   //Spin box
   this->mpc_Ui->pc_SpinBoxSize->SetMinimumCustom(10);
   this->mpc_Ui->pc_SpinBoxSize->SetMaximumCustom(std::numeric_limits<sintn>::max());

   //Timer
   this->mc_DoubleClickTimer.setInterval(QApplication::doubleClickInterval());
   this->mc_DoubleClickTimer.setSingleShot(true);
   connect(&this->mc_DoubleClickTimer, &QTimer::timeout, this,
           &C_SdNdeDpListHeaderWidget::m_OnDoubleClickTimeout);

   //OK button (exit full screen)
   this->mpc_Ui->pc_PushButtonCollapse->SetCustomIcons("://images/system_definition/NodeEdit/lists/FullscreenExit.svg",
                                                       "://images/system_definition/NodeEdit/lists/FullscreenExitHovered.svg",
                                                       "://images/system_definition/NodeEdit/lists/FullscreenExitClicked.svg",
                                                       "://images/system_definition/NodeEdit/lists/FullscreenExitDisabled.svg");

   this->mpc_Ui->pc_PushButtonCollapse->hide();
   connect(this->mpc_Ui->pc_PushButtonCollapse, &C_OgePubIconOnly::clicked, this, &C_SdNdeDpListHeaderWidget::m_OnOK);

   //signals & slots
   connect(this->mpc_Ui->pc_PushButtonExpand, &QPushButton::toggled, this,
           &C_SdNdeDpListHeaderWidget::m_OnPushButtonExpandClicked);
   connect(this->mpc_Ui->pc_PushButtonFullScreen, &QPushButton::clicked, this,
           &C_SdNdeDpListHeaderWidget::PopUp);
   connect(this->mpc_Ui->pc_PushButtonDataset, &C_OgePubSvgIconOnly::clicked, this,
           &C_SdNdeDpListHeaderWidget::m_OpenDataSetEdit);
   connect(this->mpc_Ui->pc_LineEditName, &stw_opensyde_gui_elements::C_OgeLeListHeader::textChanged, this,
           &C_SdNdeDpListHeaderWidget::m_CheckName);
   connect(this->mpc_Ui->pc_PushButtonComment, &C_OgePubSvgIconOnly::clicked, this,
           &C_SdNdeDpListHeaderWidget::m_OnEditCommentClicked);

   //Data changes
   connect(this->mpc_Ui->pc_SpinBoxSize, &stw_opensyde_gui_elements::C_OgeSpxEditProperties::editingFinished, this,
           &C_SdNdeDpListHeaderWidget::m_EditSizeFinished);
   connect(this->mpc_Ui->pc_LineEditName, &stw_opensyde_gui_elements::C_OgeLeListHeader::editingFinished, this,
           &C_SdNdeDpListHeaderWidget::m_EditNameFinished);

   //Focus
   connect(this->mpc_Ui->pc_LineEditName, &stw_opensyde_gui_elements::C_OgeLeListHeader::SigFocus, this,
           &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_SpinBoxSize, &stw_opensyde_gui_elements::C_OgeSpxEditProperties::SigFocus, this,
           &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_PushButtonExpand, &stw_opensyde_gui_elements::C_OgePubIconOnly::pressed, this,
           &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_PushButtonFullScreen, &stw_opensyde_gui_elements::C_OgePubIconOnly::pressed, this,
           &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_PushButtonComment, &C_OgePubSvgIconOnly::pressed,
           this, &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_PushButtonDataset, &C_OgePubSvgIconOnly::pressed,
           this, &C_SdNdeDpListHeaderWidget::m_HandleFocus);
   connect(this->mpc_Ui->pc_LabelListError, &C_OgeLabToolTipBase::SigLastChanceToUpdateToolTip, this,
           &C_SdNdeDpListHeaderWidget::m_UpdateErrorToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListHeaderWidget::~C_SdNdeDpListHeaderWidget(void)
{
   delete mpc_Ui;
   //lint -e1740 Never took ownership of those items
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_TreeWidget       Tree widget to inform
   \param[in,out] opc_UndoManager      Undo manager
   \param[in,out] opc_ModelViewManager Model view manager
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     ou32_DataPoolIndex   Data pool index
   \param[in]     ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::SetInitParameters(QTreeWidget * const opc_TreeWidget,
                                                  C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                                                  C_SdNdeDpListModelViewManager * const opc_ModelViewManager,
                                                  const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                  const uint32 ou32_ListIndex)
{
   this->mpc_TreeWidget = opc_TreeWidget;
   this->mpc_UndoManager = opc_UndoManager;
   this->mpc_ModelViewManager = opc_ModelViewManager;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataPoolIndex = ou32_DataPoolIndex;
   this->mu32_ListIndex = ou32_ListIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new index

   \param[in] oru32_Value New index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::SetIndex(const uint32 & oru32_Value)
{
   this->mu32_ListIndex = oru32_Value;
   m_UpdateListNamePrefix();
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelSize->setText(C_GtGetText::h_GetText("Size: "));
   this->mpc_Ui->pc_SpinBoxSize->setSuffix(C_GtGetText::h_GetText(" Bytes"));
   this->mpc_Ui->pc_LineEditName->setPlaceholderText(C_GtGetText::h_GetText("Add your List Name"));
   this->mpc_Ui->pc_PushButtonComment->SetToolTipInformation(
      C_GtGetText::h_GetText("Edit Comment"),
      C_GtGetText::h_GetText("Add or edit comment for this List."));
   this->mpc_Ui->pc_PushButtonFullScreen->SetToolTipInformation(
      C_GtGetText::h_GetText("Full Screen"),
      C_GtGetText::h_GetText("Shows this list in full-screen-mode to improve your editing experience."));
   this->mpc_Ui->pc_PushButtonCollapse->SetToolTipInformation(
      C_GtGetText::h_GetText("Exit Full Screen"),
      C_GtGetText::h_GetText("Go back to Datapool Lists."));
   this->mpc_Ui->pc_LabelSize->SetToolTipInformation(
      C_GtGetText::h_GetText("Size"),
      C_GtGetText::h_GetText("Set the number of bytes which this list will reserve."));
   this->mpc_Ui->pc_PushButtonDataset->SetToolTipInformation(
      C_GtGetText::h_GetText("Edit Datasets"),
      C_GtGetText::h_GetText("Datasets can be used to set defined values for each element contained in this list."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register expand or collapse

   \param[in]     orq_Expanded true:  expanded
                               false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::RegisterExpandOrCollapse(const bool & orq_Expanded) const
{
   this->mpc_Ui->pc_PushButtonExpand->setChecked(orq_Expanded);
   if (orq_Expanded == true)
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconCloseList.svg",
                                                        "://images/IconArrowBottomHovered.svg",
                                                        "://images/IconArrowBottomClicked.svg",
                                                        "://images/IconArrowBottomDisabledBright.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                        "://images/IconOpenListHovered.svg",
                                                        "://images/IconOpenListClicked.svg",
                                                        "://images/IconOpenListDisabledBright.svg");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::CheckError(void)
{
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      bool q_CheckSize;
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_UsageInvalid = false;
      bool q_OutOfDataPool = false;
      bool q_ElementsInvalid = false;
      if (pc_DataPool->e_Type == stw_opensyde_core::C_OSCNodeDataPool::E_Type::eNVM)
      {
         q_CheckSize = true;
      }
      else
      {
         q_CheckSize = false;
      }
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, &q_NameConflict, &q_NameInvalid, &q_UsageInvalid,
                                  &q_OutOfDataPool, &this->mq_DataSetError, &q_ElementsInvalid, NULL, NULL);
      if (q_CheckSize == false)
      {
         q_UsageInvalid = false;
         q_OutOfDataPool = false;
      }
      if ((((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_UsageInvalid == true)) ||
            (q_ElementsInvalid == true)) || (this->mq_DataSetError == true)) || (q_OutOfDataPool == true))
      {
         if (this->mpc_Ui->pc_LabelListError->isVisible() == false)
         {
            Q_EMIT (this->SigErrorChange());
         }
         m_UpdateErrorToolTip();
         this->mpc_Ui->pc_LabelListError->setVisible(true);
      }
      else
      {
         if (this->mpc_Ui->pc_LabelListError->isVisible() == true)
         {
            Q_EMIT (this->SigErrorChange());
         }
         //Hide warning sign
         this->mpc_Ui->pc_LabelListError->setVisible(false);
      }
      //Set error color
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid",
                                             ((q_NameConflict == false) && (q_NameInvalid == false)));
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsage, "Valid", (q_UsageInvalid == false));
      UpdateDataSetCount();
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelSize, "Valid", (q_OutOfDataPool == false));
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxSize, "Valid", (q_OutOfDataPool == false));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update UI elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::UpdateUI(void)
{
   m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data set size change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::UpdateDataSetCount(void)
{
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   if (pc_List != NULL)
   {
      const QString c_Error = this->mq_DataSetError ? "Error" : "";
      QString c_Svg;

      if (pc_List->c_DataSets.size() < 11)
      {
         c_Svg = static_cast<QString>("://images/system_definition/NodeEdit/datasets/IconDatasets%1%2.svg").
                 arg(pc_List->c_DataSets.size()).arg(c_Error);
      }
      else
      {
         c_Svg = static_cast<QString>("://images/system_definition/NodeEdit/datasets/IconDatasetsMore%1.svg").arg(
            c_Error);
      }

      this->mpc_Ui->pc_PushButtonDataset->SetSvg(c_Svg);
   }

   Q_EMIT (this->SigUpdated());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Notify widget if it is currently selected

   \param[in] orq_Selected Flag if selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::NotifySelection(const bool & orq_Selected)
{
   if (orq_Selected == false)
   {
      this->mc_DoubleClickTimer.stop();
      this->mpc_Ui->pc_UsageWidget->SetColorFree(mc_STYLE_GUIDE_COLOR_10);
      this->mpc_Ui->pc_LineEditName->setEnabled(false);
      this->mpc_Ui->pc_SpinBoxSize->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_UsageWidget->SetColorFree(mc_STYLE_GUIDE_COLOR_11);
      this->mpc_Ui->pc_SpinBoxSize->setEnabled(true);
      this->mc_DoubleClickTimer.start();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial focus for header after add operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::SetEditFocus(void)
{
   if (this->mc_DoubleClickTimer.isActive() == true)
   {
      this->mc_DoubleClickTimer.stop();
      m_OnDoubleClickTimeout();
   }
   this->mpc_Ui->pc_LineEditName->setFocus();
   this->mpc_Ui->pc_LineEditName->selectAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare expanded mode state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::PrepareExpandedMode(void) const
{
   this->mpc_Ui->pc_PushButtonFullScreen->hide();
   this->mpc_Ui->pc_PushButtonCollapse->setVisible(true);
   this->mpc_Ui->pc_PushButtonExpand->setChecked(true);
   this->mpc_Ui->pc_PushButtonExpand->setEnabled(false);
   //Allow interaction
   m_OnDoubleClickTimeout();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle full screen trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::PopUp(void)
{
   QPointer<C_OgePopUpDialog> c_Dialog = new C_OgePopUpDialog(this, this, false);

   C_SdNdeDpListPopUp * const pc_PopUp = new C_SdNdeDpListPopUp(*c_Dialog,
                                                                this->mu32_NodeIndex,
                                                                this->mu32_DataPoolIndex,
                                                                this->mu32_ListIndex,
                                                                this->mpc_ModelViewManager,
                                                                this->mpc_TreeWidget,
                                                                this->mpc_UndoManager);

   //Connect
   connect(this, &C_SdNdeDpListHeaderWidget::SigUpdated, pc_PopUp, &C_SdNdeDpListPopUp::UpdateHeader);
   connect(pc_PopUp, &C_SdNdeDpListPopUp::SigSave, this, &C_SdNdeDpListHeaderWidget::SigSave);
   //Resize
   c_Dialog->ApplyMaximumSize();

   static_cast<void>(c_Dialog->exec());
   //Check save
   if (c_Dialog != NULL)
   {
      disconnect(pc_PopUp, &C_SdNdeDpListPopUp::SigSave, this, &C_SdNdeDpListHeaderWidget::SigSave);
      if (pc_PopUp->GetSaveAsAfterClose() == true)
      {
         Q_EMIT (this->SigSaveAs());
      }
      //Update UI only if not in save as screen
      else
      {
         //Disconnect
         disconnect(this, &C_SdNdeDpListHeaderWidget::SigUpdated, pc_PopUp, &C_SdNdeDpListPopUp::UpdateHeader);
         //Reload
         this->m_UpdateUi();
         Q_EMIT (this->SigUpdateTable());
      }

      c_Dialog->HideOverlay();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle event push button expand clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_OnPushButtonExpandClicked(const bool oq_Checked)
{
   //Resize expanded
   if (oq_Checked == true)
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconCloseList.svg",
                                                        "://images/IconArrowBottomHovered.svg",
                                                        "://images/IconArrowBottomClicked.svg",
                                                        "://images/IconArrowBottomDisabledBright.svg");
      Q_EMIT (this->SigExpand(this, true));
   }
   else
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                        "://images/IconOpenListHovered.svg",
                                                        "://images/IconOpenListClicked.svg",
                                                        "://images/IconOpenListDisabledBright.svg");
      Q_EMIT (this->SigExpand(this, false));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update list name prefix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_UpdateListNamePrefix(void) const
{
   const QString c_Text =
      static_cast<QString>(C_GtGetText::h_GetText("List #%1 - ")).arg(QString::number(this->mu32_ListIndex + 1));

   this->mpc_Ui->pc_LabelListNamePrefix->setText(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the event of an internal item getting keyboard focus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_HandleFocus(void)
{
   Q_EMIT (this->SigExclusiveSelection(this->mu32_ListIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update all fields according to data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_UpdateUi(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   //Disconnect
   this->mq_InitActive = true;

   //List or data pool may not exist on first calls
   tgl_assert(pc_Node != NULL);
   if ((pc_Node != NULL) && ((pc_List != NULL) && (pc_DataPool != NULL)))
   {
      // name and comment
      const QString c_SimplifiedComment = static_cast<QString>(pc_List->c_Comment.c_str()).simplified();

      this->mpc_Ui->pc_LineEditName->SetName(pc_List->c_Name.c_str());
      this->mpc_Ui->pc_LineEditName->SetCounter(pc_List->c_Elements.size());
      this->mpc_Ui->pc_LabelComment->SetCompleteText(c_SimplifiedComment);
      this->mpc_Ui->pc_LabelComment->SetToolTipInformation("", pc_List->c_Comment.c_str());

      if (pc_List->c_Comment.IsEmpty() == true)
      {
         this->mpc_Ui->pc_PushButtonComment->SetSvg("://images/system_definition/IconCommentAdd.svg");
      }
      else
      {
         this->mpc_Ui->pc_PushButtonComment->SetSvg("://images/system_definition/IconComment.svg");
      }

      //Data sets
      UpdateDataSetCount();
      //Size & address
      if (pc_DataPool->e_Type == stw_opensyde_core::C_OSCNodeDataPool::E_Type::eNVM)
      {
         const C_OSCDeviceDefinition * const pc_Device = pc_Node->pc_DeviceDefinition;
         const uint32 u32_Usage = this->mpc_Ui->pc_UsageWidget->SetUsage(pc_List->u32_NvMSize,
                                                                         pc_List->GetNumBytesUsed());
         this->mpc_Ui->pc_UsageWidget->update();

         this->mpc_Ui->pc_GroupBoxUsage->setVisible(true);
         this->mpc_Ui->pc_GroupBoxSize->setVisible(true);

         //Size max
         tgl_assert(pc_Device != NULL);
         if (pc_Device != NULL)
         {
            const uint32 u32_Maximum = pc_Device->u32_UserEepromSizeBytes;
            this->mpc_Ui->pc_SpinBoxSize->SetMaximumCustom(static_cast<sintn>(u32_Maximum));
         }

         //Size value
         this->mpc_Ui->pc_SpinBoxSize->setValue(pc_List->u32_NvMSize);
         this->mpc_Ui->pc_LabelUsage->setText(static_cast<QString>(C_GtGetText::h_GetText("Usage: %1%")).arg(u32_Usage));
      }
      else
      {
         //hide size specific elements
         this->mpc_Ui->pc_GroupBoxUsage->setVisible(false);
         this->mpc_Ui->pc_GroupBoxSize->setVisible(false);
      }
      //Error handling
      CheckError();
   }
   //Reconnect
   Q_EMIT (this->SigUpdated());
   this->mq_InitActive = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register edit name finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_EditNameFinished(void)
{
   if (this->mq_InitActive == false)
   {
      const QVariant c_Data = this->mpc_Ui->pc_LineEditName->GetName();

      if (this->mpc_UndoManager != NULL)
      {
         this->mpc_UndoManager->DoChangeListData(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                 dynamic_cast<C_SdNdeDpListsTreeWidget * const>(this->
                                                                                                mpc_TreeWidget),
                                                 this->mu32_ListIndex, c_Data, C_SdNdeDpUtil::eLIST_NAME);
      }

      // remove focus from line edit
      if (this->mpc_Ui->pc_LineEditName->hasFocus())
      {
         this->setFocus();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register edit comment finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_OnEditCommentClicked(void)
{
   if (this->mq_InitActive == false)
   {
      // open comment edit popup
      QPointer<C_OgePopUpDialog> c_Dialog = new C_OgePopUpDialog(this, this);
      C_SdNdeDpListCommentDialog * const pc_ArrayEditWidget =
         new C_SdNdeDpListCommentDialog(*c_Dialog, this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                        this->mu32_ListIndex);
      //Resize
      c_Dialog->SetSize(QSize(750, 400));

      if ((c_Dialog->exec() == static_cast<sintn>(QDialog::Accepted)) && (this->mpc_UndoManager != NULL))
      {
         // save to data on ok
         const QVariant c_Data = pc_ArrayEditWidget->GetComment();

         this->mpc_UndoManager->DoChangeListData(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                 dynamic_cast<C_SdNdeDpListsTreeWidget * const>(this->mpc_TreeWidget),
                                                 this->mu32_ListIndex, c_Data, C_SdNdeDpUtil::eLIST_COMMENT);
         // this also triggers label and icon update
      }

      if (c_Dialog != NULL)
      {
         c_Dialog->HideOverlay();
      }
   } //lint !e429  //no memory leak because of the parent of pc_ImportDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register edit size finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_EditSizeFinished(void)
{
   if (this->mq_InitActive == false)
   {
      //lint -e571 cast necessary to explicitly have QVariant of type uint64
      const QVariant c_Data = static_cast<uint64>(this->mpc_Ui->pc_SpinBoxSize->value());

      if (this->mpc_UndoManager != NULL)
      {
         this->mpc_UndoManager->DoChangeListData(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                 dynamic_cast<C_SdNdeDpListsTreeWidget * const>(this->
                                                                                                mpc_TreeWidget),
                                                 this->mu32_ListIndex, c_Data, C_SdNdeDpUtil::eLIST_SIZE);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open data set edit & save changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_OpenDataSetEdit(void)
{
   C_OSCNodeDataPoolList c_OSCList;
   C_PuiSdNodeDataPoolList c_UIList;

   QPointer<C_OgePopUpDialog> c_Dialog = new C_OgePopUpDialog(this, this);

   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                        this->mu32_ListIndex, c_OSCList, c_UIList) == C_NO_ERR)
   {
      C_SdNdeDpListDataSetWidget * const pc_DataSetWidget = new C_SdNdeDpListDataSetWidget(*c_Dialog,
                                                                                           this->mu32_NodeIndex,
                                                                                           this->mu32_DataPoolIndex,
                                                                                           this->mu32_ListIndex);

      pc_DataSetWidget->SetModelViewManager(this->mpc_ModelViewManager);
      //Resize
      c_Dialog->SetSize(QSize(790, 540));
      if (c_Dialog->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         //Register undo
         QUndoCommand * const pc_UndoCommand = pc_DataSetWidget->TakeUndoCommand();
         if ((pc_UndoCommand != NULL) && (this->mpc_UndoManager != NULL))
         {
            //Undo all because push automatically redos
            pc_UndoCommand->undo();
            this->mpc_UndoManager->DoPush(pc_UndoCommand);
         }
      }
      else
      {
         if (c_Dialog != NULL)
         {
            //Revert changes
            C_PuiSdHandler::h_GetInstance()->SetDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                             this->mu32_ListIndex, c_OSCList, c_UIList);
            this->CheckError();
         }
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   if (c_Dialog != NULL)
   {
      c_Dialog->HideOverlay();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error on name change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_CheckName(void) const
{
   QString c_Content;
   const stw_scl::C_SCLString c_Name = this->mpc_Ui->pc_LineEditName->GetName().toStdString().c_str();

   //check
   bool q_NameIsValid = C_OSCUtils::h_CheckValidCName(c_Name);

   if (q_NameIsValid == false)
   {
      c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters\n");
   }

   if (C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolListNameAvailable(this->mu32_NodeIndex,
                                                                           this->mu32_DataPoolIndex,
                                                                           c_Name,
                                                                           &this->mu32_ListIndex) == false)
   {
      q_NameIsValid = false;
      c_Content += C_GtGetText::h_GetText("- is already in use\n");
   }

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid", q_NameIsValid);
   if (q_NameIsValid == true)
   {
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("List Name");
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle double click timeout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_OnDoubleClickTimeout(void) const
{
   this->mpc_Ui->pc_LineEditName->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle OK action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_OnOK(void)
{
   Q_EMIT (this->SigClose());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update error tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderWidget::m_UpdateErrorToolTip(void) const
{
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      bool q_CheckSize;
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_UsageInvalid = false;
      bool q_DataSetError = false;
      bool q_OutOfDataPool = false;
      bool q_ElementsInvalid = false;

      std::vector<uint32> c_InvalidDataSetIndices;
      std::vector<uint32> c_InvalidElementIndices;
      if (pc_DataPool->e_Type == stw_opensyde_core::C_OSCNodeDataPool::E_Type::eNVM)
      {
         q_CheckSize = true;
      }
      else
      {
         q_CheckSize = false;
      }
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, &q_NameConflict, &q_NameInvalid, &q_UsageInvalid,
                                  &q_OutOfDataPool, &q_DataSetError, &q_ElementsInvalid,
                                  &c_InvalidDataSetIndices, &c_InvalidElementIndices);
      if (q_CheckSize == false)
      {
         q_UsageInvalid = false;
         q_OutOfDataPool = false;
      }
      if ((((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_UsageInvalid == true)) ||
            (q_ElementsInvalid == true)) || (this->mq_DataSetError == true)) || (q_OutOfDataPool == true))
      {
         const QString c_Heading = C_GtGetText::h_GetText("List has invalid content");
         QString c_Content;
         if ((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_UsageInvalid == true)) ||
             (q_OutOfDataPool == true))
         {
            c_Content += C_GtGetText::h_GetText("Invalid properties:\n");
            if (q_NameConflict == true)
            {
               c_Content += C_GtGetText::h_GetText("Duplicate list name detected.\n");
            }
            if (q_NameInvalid == true)
            {
               c_Content += C_GtGetText::h_GetText("List name is empty or contains invalid characters.\n");
            }
            if (q_UsageInvalid == true)
            {
               c_Content += C_GtGetText::h_GetText(
                  "List contains too many parameters. More than reserved by list size.\n");
            }
            if (q_OutOfDataPool == true)
            {
               c_Content +=
                  C_GtGetText::h_GetText("Reserved list size is bigger than the reserved NVM Datapool size.\n");
            }
            c_Content += "\n";
         }
         if (q_DataSetError == true)
         {
            c_Content += C_GtGetText::h_GetText("Invalid Datasets:\n");
            for (uint32 u32_ItDataSet = 0;
                 (u32_ItDataSet < c_InvalidDataSetIndices.size()) &&
                 (u32_ItDataSet < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                 ++u32_ItDataSet)
            {
               const C_OSCNodeDataPoolDataSet * const pc_DataSet =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListDataSet(
                     this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                     c_InvalidDataSetIndices[u32_ItDataSet]);
               if (pc_DataSet != NULL)
               {
                  c_Content += static_cast<QString>("%1\n").arg(pc_DataSet->c_Name.c_str());
               }
            }
            if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidDataSetIndices.size())
            {
               c_Content += static_cast<QString>("+%1\n").arg(
                  c_InvalidDataSetIndices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
            }
            c_Content += "\n";
         }
         if (q_ElementsInvalid == true)
         {
            c_Content += C_GtGetText::h_GetText("Invalid elements:\n");
            for (uint32 u32_ItElement = 0;
                 (u32_ItElement < c_InvalidElementIndices.size()) &&
                 (u32_ItElement < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                 ++u32_ItElement)
            {
               const C_OSCNodeDataPoolListElement * const pc_Appl =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex,
                                                                             c_InvalidElementIndices[u32_ItElement]);
               if (pc_Appl != NULL)
               {
                  c_Content += static_cast<QString>("%1\n").arg(pc_Appl->c_Name.c_str());
               }
            }
            if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidElementIndices.size())
            {
               c_Content += static_cast<QString>("+%1\n").arg(
                  c_InvalidElementIndices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
            }
            c_Content += "\n";
         }
         this->mpc_Ui->pc_LabelListError->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
      }
   }
}
