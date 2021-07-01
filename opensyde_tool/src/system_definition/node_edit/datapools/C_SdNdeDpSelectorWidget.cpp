//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all datapools of a node and a specific category

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpSelectorWidget.h"

#include "constants.h"

#include "ui_C_SdNdeDpSelectorWidget.h"
#include "C_OgeWiUtil.h"

#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"
#include "C_OSCDeviceDefinition.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorWidget::C_SdNdeDpSelectorWidget(QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpSelectorWidget),
   mq_UsageViewActive(false),
   mu32_NodeIndex(0),
   me_DataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   mc_InstanceName(""),
   mq_Selected(false)
{
   QSizePolicy c_SizePolicy;

   mpc_Ui->setupUi(this);

   InitStaticNames();

   // setting an item invisible may not change the layout
   c_SizePolicy = this->mpc_Ui->pc_PushButtonScrollLeft->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_PushButtonScrollLeft->setSizePolicy(c_SizePolicy);
   this->mpc_Ui->pc_PushButtonScrollRight->setSizePolicy(c_SizePolicy);
   this->mpc_Ui->pc_IndexViewWidget->setSizePolicy(c_SizePolicy);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_10);

   // configure the label for the dynamic icons
   this->mc_ConflictImg = QIcon("://images/Error_iconV2.svg").pixmap(mc_ICON_SIZE_24);

   // init icons
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonScrollLeft->SetCustomIcons("://images/IconScrollLeftEnabled.svg",
                                                         "://images/IconScrollLeftHovered.svg",
                                                         "://images/IconScrollLeftClicked.svg",
                                                         "://images/IconScrollLeftDisabledBright.svg");
   this->mpc_Ui->pc_PushButtonScrollRight->SetCustomIcons("://images/IconScrollRightEnabled.svg",
                                                          "://images/IconScrollRightHovered.svg",
                                                          "://images/IconScrollRightClicked.svg",
                                                          "://images/IconScrollRightDisabledBright.svg");

   this->mpc_LabelStateImg = new C_OgeLabToolTipBase(this);
   this->mpc_LabelStateImg->raise();
   this->mpc_LabelStateImg->move(50, 8);
   this->mpc_LabelStateImg->setVisible(false);
   this->mpc_LabelStateImg->setPixmap(this->mc_ConflictImg);
   this->mpc_LabelStateImg->resize(this->mc_ConflictImg.width(), this->mc_ConflictImg.height());

   this->mpc_Ui->pc_PushButtonAdd->setEnabled(true);

   this->mpc_Ui->pc_LabelDpCount->setText("0");

   this->mpc_Ui->pc_LabelDpLogo->SetSvg("://images/system_definition/IconDataPoolBig.svg");

   this->m_SetupContextMenu();

   connect(this->mpc_Ui->pc_PushButtonScrollLeft, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SdNdeDpSelectorWidget::m_ButtonLeftClicked);
   connect(this->mpc_Ui->pc_PushButtonScrollRight, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SdNdeDpSelectorWidget::m_ButtonRightClicked);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigListChanged,
           this, &C_SdNdeDpSelectorWidget::m_UpdateWidget);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigWidgetFocused,
           this, &C_SdNdeDpSelectorWidget::m_ListFocused);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigOpenDataPoolContent,
           this, &C_SdNdeDpSelectorWidget::m_OpenDataPoolItemContent);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigErrorCheck,
           this, &C_SdNdeDpSelectorWidget::m_OnErrorCheck);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigNoDataPoolSelected,
           this, &C_SdNdeDpSelectorWidget::SigNoDataPoolSelected);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigDataPoolHoverStateChanged,
           this, &C_SdNdeDpSelectorWidget::SigDataPoolHoverStateChanged);

   connect(this->mpc_Ui->pc_PushButtonAdd, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SdNdeDpSelectorWidget::m_AddNewDatapool);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigHideOtherToolTips, this,
           &C_SdNdeDpSelectorWidget::HideToolTip);
   connect(this->mpc_LabelStateImg, &C_OgeLabToolTipBase::SigHideOtherToolTips, this,
           &C_SdNdeDpSelectorWidget::HideToolTip);
   connect(this->mpc_Ui->pc_PushButtonAdd, &C_OgePubIconOnly::SigHideOtherToolTips, this,
           &C_SdNdeDpSelectorWidget::HideToolTip);
   connect(this->mpc_LabelStateImg, &C_OgeLabToolTipBase::SigLastChanceToUpdateToolTip, this,
           &C_SdNdeDpSelectorWidget::m_UpdateErrorToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_LabelStateImg and the Qt memory management
C_SdNdeDpSelectorWidget::~C_SdNdeDpSelectorWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::InitStaticNames(void) const
{
   //Tool tips
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                         C_GtGetText::h_GetText("Add new Datapool."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inits the widget with all necessary information

   \param[in]     orc_Title            Title for category label
   \param[in]     orc_TooltipTitle     Title for tooltip
   \param[in]     orc_TooltipContent   Tooltip content
   \param[in]     orc_InstanceName     Name of the instance of this class of using stylesheets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::InitWidget(const QString & orc_Title, const QString & orc_TooltipTitle,
                                         const QString & orc_TooltipContent, const QString & orc_InstanceName)
{
   this->mc_InstanceName = orc_InstanceName;
   this->mpc_Ui->pc_LabelDpCategory->setText(orc_Title);
   this->mpc_Ui->pc_LabelDpCategory->SetToolTipInformation(orc_TooltipTitle, orc_TooltipContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget selected

   Adapts the stylesheet

   \param[in]     oq_Selected   Is widget selected or not active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetSelected(const bool oq_Selected)
{
   this->mq_Selected = oq_Selected;

   if (this->mq_Selected == true)
   {
      // adapt the styling with the stylesheet
      this->setStyleSheet("stw_opensyde_gui--C_SdNdeDpSelectorWidget#" + this->mc_InstanceName + " {"
                          "background-color: " + mc_STYLESHEET_GUIDE_COLOR_10 + ";"
                          "}"
                          "QFrame#pc_FrameSdNdeDpSelectorItem {"
                          "background-color: " + mc_STYLESHEET_GUIDE_COLOR_11 + ";"
                          "}");
   }
   else
   {
      // reset stylesheet
      this->setStyleSheet("");
   }
   this->mpc_Ui->pc_ListWidget->SetSelected(oq_Selected);

   this->m_UpdateWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Is the widget active or inactive

   \return
   true     widget is active
   false    widget is not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorWidget::IsActive(void) const
{
   return this->mq_Selected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated node

   \param[in] oe_Type                  Datapool type
   \param[in] ou32_NodeIndex           Node index
   \param[in] oq_UsageViewActive       Flag for showing usage bars for datapool
   \param[in] oq_AddBtnVisible         Flag for showing add button

   \return
   true   datapool added
   false  no datapool added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorWidget::SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                                             const uint32 ou32_NodeIndex, const bool oq_UsageViewActive,
                                             const bool oq_AddBtnVisible)
{
   bool q_Return;

   this->mq_UsageViewActive = oq_UsageViewActive;
   this->mu32_NodeIndex = ou32_NodeIndex;

   this->mpc_Ui->pc_PushButtonAdd->setVisible(oq_AddBtnVisible);

   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigDataPoolChanged,
           this, &C_SdNdeDpSelectorWidget::SigDataPoolChanged);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigUpdateLists, this,
           &C_SdNdeDpSelectorWidget::SigUpdateLists);

   this->me_DataPoolType = oe_Type;
   q_Return = this->mpc_Ui->pc_ListWidget->SetTypeAndNode(oe_Type, this->mu32_NodeIndex, oq_UsageViewActive);

   if (q_Return == true)
   {
      this->ErrorCheck();
   }
   else
   {
      // Deactivate a possible left over error state.
      this->mpc_LabelStateImg->setVisible(false);
   }

   this->m_UpdateWidget();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the selected Datapool

   \param[in]       ou32_Index     Index of Datapool item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetSelectedDataPool(const uint32 ou32_Index) const
{
   this->mpc_Ui->pc_ListWidget->SetSelectedItem(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in] osn_DataPoolWidgetIndex  Datapool type index of Datapool which is affected
   \param[in] oq_Active                Flag if conflict is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetCurrentDataPoolConflict(const sintn osn_DataPoolWidgetIndex, const bool oq_Active)
{
   const bool q_Return = this->mpc_Ui->pc_ListWidget->SetActualDataPoolConflict(osn_DataPoolWidgetIndex, oq_Active);

   if (q_Return == true)
   {
      // update the label with the state icon
      this->mpc_LabelStateImg->setVisible(true);
   }
   else
   {
      // nothing to paint
      this->mpc_LabelStateImg->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widget for the all Datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::UpdateDataPools(void)
{
   this->mpc_Ui->pc_ListWidget->UpdateDataPools();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the widgets for the actual datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::UpdateActualDataPool(void) const
{
   this->mpc_Ui->pc_ListWidget->UpdateActualDataPool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Recheck error for all data pools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::ErrorCheck(void)
{
   QString c_Heading;
   QString c_Content;

   std::vector<uint32> c_InvalidDatapoolIndices;
   bool q_DatapoolNvmSizeConflict = false;
   bool q_DatapoolNvmOverlapConflict = false;
   bool q_Error = this->mpc_Ui->pc_ListWidget->CheckDataPoolsForConflict(&c_InvalidDatapoolIndices);

   if ((this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eNVM) ||
       (this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eHALC_NVM))
   {
      // check the entire NVM size
      if (C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(this->mu32_NodeIndex,
                                                                             &q_DatapoolNvmSizeConflict,
                                                                             &q_DatapoolNvmOverlapConflict) == true)
      {
         q_Error = true;
      }
   }

   C_SdUtil::h_GetErrorToolTipDataPools(this->mu32_NodeIndex, c_InvalidDatapoolIndices, q_DatapoolNvmSizeConflict,
                                        q_DatapoolNvmOverlapConflict, c_Heading, c_Content);

   this->mpc_LabelStateImg->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   //Update the group label (items should have updated labels already)
   this->mpc_LabelStateImg->setVisible(q_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided focus in event

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::focusInEvent(QFocusEvent * const opc_Event)
{
   QWidget::focusInEvent(opc_Event);

   this->mpc_Ui->pc_ListWidget->SetSelected(true);
   this->SetSelected(true);

   if ((this->mpc_Ui->pc_ListWidget->GetCurrentItemIndex() < 0) &&
       (this->mpc_Ui->pc_ListWidget->GetItemCount() > 0))
   {
      this->mpc_Ui->pc_ListWidget->SetSelectedItem(0);
   }

   // Assign the focus to the list widget with the key event handling
   this->mpc_Ui->pc_ListWidget->setFocus();

   Q_EMIT (this->SigWidgetFocused(this->me_DataPoolType));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::paintEvent(QPaintEvent * const opc_Event)
{
   //draw background
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   this->mpc_Ui->pc_ListWidget->UpdateSizeHint(this->mpc_Ui->pc_ListContainerWidget->height());
   this->mpc_Ui->pc_ListWidget->updateGeometry();

   C_OgeWiWithToolTip::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle list actions

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   if ((this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC) &&
       (this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC_NVM) &&
       (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true))
   {
      switch (opc_Event->key())
      {
      case Qt::Key_V:
         q_CallOrig = false;
         this->m_PasteDatapool();
         break;
      case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
      case Qt::Key_Plus:
         q_CallOrig = false;
         this->m_AddNewDatapool();
         break;
      default:
         //Nothing to do
         break;
      }
   }

   if (q_CallOrig == true)
   {
      C_OgeWiWithToolTip::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the state of the widget depending of the list widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_UpdateWidget(void)
{
   if (this->mpc_Ui->pc_ListWidget->GetViewPageCount() > 1)
   {
      // not all items are visible at the same time
      // adapt the ui
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(this->mq_Selected);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(this->mq_Selected);

      if (this->mpc_Ui->pc_ListWidget->GetCurrentViewPage() < 1)
      {
         // position is at the start of the list
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(false);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(true);
      }
      else if (this->mpc_Ui->pc_ListWidget->GetCurrentViewPage() >=
               (this->mpc_Ui->pc_ListWidget->GetViewPageCount() - 1))
      {
         // position is at the end of the list
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(true);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(true);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(true);
      }

      // update the index view
      this->mpc_Ui->pc_IndexViewWidget->setVisible(true);
      if (this->mq_Selected == true)
      {
         this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_11);
      }
      else
      {
         this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_10);
      }
      this->mpc_Ui->pc_IndexViewWidget->SetCountIndex(this->mpc_Ui->pc_ListWidget->GetViewPageCount());
      this->mpc_Ui->pc_IndexViewWidget->SetCurrentIndex(this->mpc_Ui->pc_ListWidget->GetCurrentViewPage());
   }
   else
   {
      // all items are visible at the same time
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(false);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(false);
      this->mpc_Ui->pc_IndexViewWidget->setVisible(false);
   }

   // update the label with the datapool count
   this->mpc_Ui->pc_LabelDpCount->setText(QString::number(this->mpc_Ui->pc_ListWidget->GetItemCount()));

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ListFocused(void)
{
   if (this->mq_Selected == false)
   {
      this->SetSelected(true);
   }
   Q_EMIT (this->SigWidgetFocused(this->me_DataPoolType));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_OpenDataPoolItemContent(const sintn osn_DataPoolWidgetIndex)
{
   Q_EMIT (this->SigOpenDataPoolContent(this->me_DataPoolType, osn_DataPoolWidgetIndex));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ButtonRightClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetCurrentViewPage(this->mpc_Ui->pc_ListWidget->GetCurrentViewPage() + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ButtonLeftClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetCurrentViewPage(this->mpc_Ui->pc_ListWidget->GetCurrentViewPage() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_OnErrorCheck(void)
{
   Q_EMIT (this->SigErrorCheck());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   // For HAL Datapools no context menu is necessary
   if ((this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC) &&
       (this->me_DataPoolType != stw_opensyde_core::C_OSCNodeDataPool::eHALC_NVM))
   {
      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add new Datapool"), this,
                                    &C_SdNdeDpSelectorWidget::m_AddNewDatapool,
                                    static_cast<sintn>(Qt::CTRL) +
                                    static_cast<sintn>(Qt::Key_Plus));
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"), this,
                                    &C_SdNdeDpSelectorWidget::m_PasteDatapool,
                                    static_cast<sintn>(Qt::CTRL) +
                                    static_cast<sintn>(Qt::Key_V));

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_SdNdeDpSelectorWidget::customContextMenuRequested, this,
           &C_SdNdeDpSelectorWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_AddNewDatapool(void) const
{
   this->mpc_Ui->pc_ListWidget->setFocus();
   this->mpc_Ui->pc_ListWidget->AddNewDatapool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paste action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_PasteDatapool(void) const
{
   this->mpc_Ui->pc_ListWidget->setFocus();
   this->mpc_Ui->pc_ListWidget->Paste();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do live update for datapool error tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_UpdateErrorToolTip(void)
{
   const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
      this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      QString c_Heading;
      QString c_Content;
      bool q_Error = false;
      bool q_DatapoolNvmSizeConflict = false;
      bool q_DatapoolNvmOverlapConflict = false;
      std::vector<uint32> c_InvalidDataPoolIndices;

      for (sintn sn_ItDp = 0; sn_ItDp < this->mpc_Ui->pc_ListWidget->GetItemCount(); ++sn_ItDp)
      {
         const sint32 s32_Index = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                    this->me_DataPoolType,
                                                                                    static_cast<uint32>(sn_ItDp));
         if (s32_Index >= 0)
         {
            bool q_NameConflict;
            bool q_NameInvalid;
            bool q_IsErrorInListOrMessage;
            bool q_IsErrorInListOrElementLength;
            pc_Node->CheckErrorDataPool(static_cast<uint32>(s32_Index), &q_NameConflict, &q_NameInvalid,
                                        &q_IsErrorInListOrMessage, &q_IsErrorInListOrElementLength, NULL);
            if (((q_NameConflict == true) || (q_NameInvalid == true)) || (q_IsErrorInListOrMessage == true) ||
                (q_IsErrorInListOrElementLength == true))
            {
               q_Error = true;
               c_InvalidDataPoolIndices.push_back(static_cast<uint32>(s32_Index));
            }
         }
      }

      if ((this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eNVM) ||
          (this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eHALC_NVM))
      {
         // check the entire NVM size
         if (C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(this->mu32_NodeIndex,
                                                                                &q_DatapoolNvmSizeConflict,
                                                                                &q_DatapoolNvmOverlapConflict) == true)
         {
            q_Error = true;
         }
      }
      if (q_Error == true)
      {
         C_SdUtil::h_GetErrorToolTipDataPools(this->mu32_NodeIndex, c_InvalidDataPoolIndices, q_DatapoolNvmSizeConflict,
                                              q_DatapoolNvmOverlapConflict, c_Heading, c_Content);
      }
      this->mpc_LabelStateImg->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}
