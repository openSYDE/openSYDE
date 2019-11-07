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
#include "TGLUtils.h"

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
   mpc_UsageBar(NULL),
   mq_UsageViewActive(false),
   mu32_NodeIndex(0),
   me_DataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   mc_InstanceName(""),
   mq_Active(false)
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
   this->mpc_Ui->pc_LabelUsagePercentage->setSizePolicy(c_SizePolicy);
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
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigErrorCheck,
           this, &C_SdNdeDpSelectorWidget::m_OnErrorCheck);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigNoDataPoolSelected,
           this, &C_SdNdeDpSelectorWidget::SigNoDataPoolSelected);
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
C_SdNdeDpSelectorWidget::~C_SdNdeDpSelectorWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_LabelStateImg, mpc_UsageBar and the Qt memory management
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

   \param[in]     orc_Title         Title for category label
   \param[in]     orc_Description   Description for category label
   \param[in]     orc_InstanceName  Name of the instance of this class of using stylesheets
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
/*! \brief   Specify associated node

   \param[in] oe_Type                  Datapool type
   \param[in] ou32_NodeIndex           Node index
   \param[in] oq_UsageViewActive       Flag for showing usage bars for datapool

   \return
   true   datapool added
   false  no datapool added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorWidget::SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                                             const uint32 ou32_NodeIndex, const bool oq_UsageViewActive)
{
   bool q_Return;

   this->mq_UsageViewActive = oq_UsageViewActive;
   this->mu32_NodeIndex = ou32_NodeIndex;

   if (this->mq_UsageViewActive == true)
   {
      this->mpc_Ui->pc_LabelUsagePercentage->setVisible(true);

      if (this->mpc_UsageBar == NULL)
      {
         this->mpc_UsageBar = new C_SdNdeDpSelectorUsageWidget(this);
         connect(this->mpc_UsageBar, &C_SdNdeDpSelectorUsageWidget::SigHideOtherToolTips, this,
                 &C_SdNdeDpSelectorWidget::HideToolTip);
         connect(this->mpc_Ui->pc_LabelDpCategory, &C_OgeLabCategoryHeading::SigHideOtherToolTips, this,
                 &C_SdNdeDpSelectorWidget::HideToolTip);
      }

      if (this->mpc_UsageBar != NULL)
      {
         // adapt the size
         this->mpc_UsageBar->setGeometry(0, this->height() - 10, this->width(), this->height());
      }
   }
   else
   {
      this->mpc_Ui->pc_LabelUsagePercentage->setVisible(false);
   }

   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigDataPoolChanged,
           this, &C_SdNdeDpSelectorWidget::m_DpChangedUpdateUsageView);
   connect(this->mpc_Ui->pc_ListWidget, &C_SdNdeDpSelectorListWidget::SigUpdateLists, this,
           &C_SdNdeDpSelectorWidget::SigUpdateLists);

   this->me_DataPoolType = oe_Type;
   q_Return = this->mpc_Ui->pc_ListWidget->SetTypeAndNode(oe_Type, this->mu32_NodeIndex, oq_UsageViewActive);

   if (q_Return == true)
   {
      this->ErrorCheck();
   }

   this->m_UpdateWidget();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the conflict state of the active datapool

   \param[in] oq_Active     Flag if conflict is active or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetCurrentDataPoolConflict(const bool oq_Active)
{
   const bool q_Return = this->mpc_Ui->pc_ListWidget->SetActualDataPoolConflict(oq_Active);

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
/*! \brief   Updates the widgets for the actual datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::UpdateActualDataPool(void) const
{
   this->mpc_Ui->pc_ListWidget->UpdateActualDataPool();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the datapool with a specific index active

   \param[in]  ou32_DataPoolIndex   Real datapool index

   \return
   true     datapool found with this index
   false    no datapool found with this index
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorWidget::SetDataPoolActive(const uint32 ou32_DataPoolIndex) const
{
   return this->mpc_Ui->pc_ListWidget->SetDataPoolActive(ou32_DataPoolIndex);
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
   bool q_NvmProblem = false;
   bool q_Error = this->mpc_Ui->pc_ListWidget->CheckDataPoolsForConflict(&c_InvalidDatapoolIndices);

   if (this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eNVM)
   {
      // check the entire NVM size
      if (C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(this->mu32_NodeIndex) == true)
      {
         q_NvmProblem = true;
         q_Error = true;
      }
   }

   C_SdUtil::h_GetErrorToolTipDataPools(this->mu32_NodeIndex, c_InvalidDatapoolIndices, q_NvmProblem, c_Heading,
                                        c_Content);

   this->mpc_LabelStateImg->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   //Update the group label (items should have updated labels already)
   this->mpc_LabelStateImg->setVisible(q_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget active or inactive

   Adapts the stylesheet

   \param[in]     oq_Active   Is widget active or not active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetActive(const bool oq_Active)
{
   this->mq_Active = oq_Active;

   if (this->mq_Active == true)
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
   this->mpc_Ui->pc_ListWidget->SetActive(oq_Active);

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
   return this->mq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget in maximized or minimized mode

   \param[in]     oq_Maximized   Flag for maximized
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::SetMaximized(const bool oq_Maximized)
{
   if (this->mpc_UsageBar != NULL)
   {
      this->mpc_UsageBar->setVisible(oq_Maximized);
      this->mpc_Ui->pc_LabelUsagePercentage->setVisible(oq_Maximized);
   }

   if (oq_Maximized == true)
   {
      this->setMaximumHeight(252);
      this->setMinimumHeight(252);
   }
   else
   {
      this->setMinimumHeight(162);
      this->setMaximumHeight(162);
   }

   this->mpc_Ui->pc_ListWidget->SetMaximized(oq_Maximized);
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
/*! \brief   Overwritten resize event slot

   Here: Resize usage widget size

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   if (this->mpc_UsageBar != NULL)
   {
      this->mpc_UsageBar->resize(this->width(), 10);
   }
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

   if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
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
/*! \brief   Overrided focus in event

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::focusInEvent(QFocusEvent * const opc_Event)
{
   QWidget::focusInEvent(opc_Event);

   this->mpc_Ui->pc_ListWidget->SetActive(true);

   if ((this->mpc_Ui->pc_ListWidget->currentRow() < 0) &&
       (this->mpc_Ui->pc_ListWidget->count() > 0))
   {
      this->mpc_Ui->pc_ListWidget->setCurrentRow(0);
   }

   Q_EMIT this->SigWidgetFocused(this->me_DataPoolType, this->mpc_Ui->pc_ListWidget->currentRow());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_UpdateWidget(void)
{
   if (this->mpc_Ui->pc_ListWidget->GetCountLines() > 1)
   {
      // not all items are visible at the same time
      // adapt the ui
      // show the arrows only if the item is active
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(this->mq_Active);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(this->mq_Active);

      if (this->mpc_Ui->pc_ListWidget->GetActualLine() < 1)
      {
         // position is at the start of the list
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(false);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(true);
      }
      else if (this->mpc_Ui->pc_ListWidget->GetActualLine() >= (this->mpc_Ui->pc_ListWidget->GetCountLines() - 1))
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
      if (this->mq_Active == true)
      {
         this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_11);
      }
      else
      {
         this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_10);
      }
      this->mpc_Ui->pc_IndexViewWidget->SetCountIndex(this->mpc_Ui->pc_ListWidget->GetCountLines());
      this->mpc_Ui->pc_IndexViewWidget->SetCurrentIndex(this->mpc_Ui->pc_ListWidget->GetActualLine());
   }
   else
   {
      // all items are visible at the same time
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(false);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(false);
      this->mpc_Ui->pc_IndexViewWidget->setVisible(false);
   }

   // update the label with the datapool count
   this->mpc_Ui->pc_LabelDpCount->setText(QString::number(this->mpc_Ui->pc_ListWidget->count()));

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_DpChangedUpdateUsageView(const bool oq_InitChange)
{
   if (oq_InitChange == false)
   {
      Q_EMIT this->SigChanged();
   }

   if (this->mpc_UsageBar != NULL)
   {
      // update the usage view
      std::vector<uint32> c_VecUsedSize;
      std::vector<QString> c_VecDatapoolNames;
      uint32 u32_Percentage;
      uint32 u32_SumNvmSize = 0;
      uint32 u32_DpCounter;
      const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
         this->mu32_NodeIndex);

      if (pc_Node != NULL)
      {
         const stw_opensyde_core::C_OSCDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
         tgl_assert(pc_DevDef != NULL);
         if (pc_DevDef != NULL)
         {
            QString c_LabelTooltip = QString("%1% ") + C_GtGetText::h_GetText("reserved by Datapools") + QString(
               " (%2 / %3)");
            // check all datapools of the lists
            for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
            {
               if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
               {
                  // save the name of the datapool
                  c_VecDatapoolNames.push_back(QString(pc_Node->c_DataPools[u32_DpCounter].c_Name.c_str()));
                  c_VecUsedSize.push_back(pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize);
                  u32_SumNvmSize += pc_Node->c_DataPools[u32_DpCounter].u32_NvMSize;
               }
            }

            u32_Percentage = this->mpc_UsageBar->SetUsage(pc_DevDef->u32_UserEepromSizeBytes, c_VecUsedSize,
                                                          c_VecDatapoolNames);
            // show the percentage
            this->mpc_Ui->pc_LabelUsagePercentage->setText(QString::number(u32_Percentage) + "%");
            c_LabelTooltip = c_LabelTooltip.arg(QString::number(u32_Percentage),
                                                C_Uti::h_GetByteCountAsString(u32_SumNvmSize),
                                                C_Uti::h_GetByteCountAsString(pc_DevDef->u32_UserEepromSizeBytes));
            this->SetToolTipInformation(C_GtGetText::h_GetText("NVM Statistics"), c_LabelTooltip);

            if (u32_SumNvmSize > pc_DevDef->u32_UserEepromSizeBytes)
            {
               C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsagePercentage, "Conflict", true);
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsagePercentage, "Conflict", false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ListFocused(const sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange)
{
   if (this->mq_Active == false)
   {
      this->SetActive(true);
   }
   Q_EMIT this->SigWidgetFocused(this->me_DataPoolType, osn_DataPoolWidgetIndex, oq_ForceChange);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ButtonRightClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetActualLine(this->mpc_Ui->pc_ListWidget->GetActualLine() + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_ButtonLeftClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetActualLine(this->mpc_Ui->pc_ListWidget->GetActualLine() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_OnErrorCheck(void)
{
   Q_EMIT this->SigErrorCheck();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   //Check if add is possible
   this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
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
      bool q_NvmProblem = false;
      std::vector<uint32> c_InvalidDataPoolIndices;

      for (sintn sn_ItDp = 0; sn_ItDp < this->mpc_Ui->pc_ListWidget->count(); ++sn_ItDp)
      {
         const sint32 s32_Index = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                    this->me_DataPoolType,
                                                                                    static_cast<uint32>(sn_ItDp));
         if (s32_Index >= 0)
         {
            bool q_NameConflict;
            bool q_NameInvalid;
            bool q_IsErrorInListOrMessage;
            pc_Node->CheckErrorDataPool(static_cast<uint32>(s32_Index), &q_NameConflict, &q_NameInvalid,
                                        &q_IsErrorInListOrMessage, NULL);
            if (((q_NameConflict == true) || (q_NameInvalid == true)) || (q_IsErrorInListOrMessage == true))
            {
               q_Error = true;
               c_InvalidDataPoolIndices.push_back(static_cast<uint32>(s32_Index));
            }
         }
      }

      if (this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eNVM)
      {
         // check the entire NVM size
         if (C_PuiSdHandler::h_GetInstance()->CheckNodeNvmDataPoolsSizeConflict(this->mu32_NodeIndex) == true)
         {
            q_Error = true;
            q_NvmProblem = true;
         }
      }
      if (q_Error == true)
      {
         C_SdUtil::h_GetErrorToolTipDataPools(this->mu32_NodeIndex, c_InvalidDataPoolIndices, q_NvmProblem, c_Heading,
                                              c_Content);
      }
      this->mpc_LabelStateImg->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}
