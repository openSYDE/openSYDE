//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A datapool item widget for the list widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>

#include "constants.h"

#include "C_SdNdeDpSelectorItemWidget.h"
#include "ui_C_SdNdeDpSelectorItemWidget.h"

#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "TGLUtils.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_OSCNode.h"
#include "C_OSCNodeDataPool.h"
#include "C_SdNdeDpUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QSize C_SdNdeDpSelectorItemWidget::hc_MaximumSize = QSize(145, 151);
const QSize C_SdNdeDpSelectorItemWidget::hc_MinimumSize = QSize(145, 61);

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     oq_UsageViewActive   Flag for showing usage bars for datapool
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorItemWidget::C_SdNdeDpSelectorItemWidget(const bool oq_UsageViewActive, QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpSelectorItemWidget),
   mpc_UsageBar(NULL),
   mc_DatapoolId(C_OSCNodeDataPoolId()),
   mq_StateSafety(false),
   mq_StateConflict(false),
   mq_Shared(false),
   mq_Maximized(true),
   mq_Active(false),
   mq_UsageViewActive(oq_UsageViewActive),
   mu32_Size(0U),
   mu32_Used(0U),
   mu32_Number(0U),
   mc_Name("")
{
   mpc_Ui->setupUi(this);

   this->mc_ConflictImg = QIcon("://images/Error_iconV2.svg").pixmap(mc_ICON_SIZE_24);

   this->mpc_LabelStateImg = new C_OgeLabToolTipBase(this);
   this->mpc_LabelStateImg->raise();
   this->mpc_LabelStateImg->move(114, 0);
   this->mpc_LabelStateImg->setVisible(false);

   this->mpc_LabelShareImg = new QLabel(this);
   this->mpc_LabelShareImg->raise();
   this->mpc_LabelShareImg->move(114, 114);
   this->mpc_LabelShareImg->setVisible(false);
   this->mpc_LabelShareImg->setPixmap(QIcon("://images/system_definition/IconShare.svg").pixmap(mc_ICON_SIZE_20));

   if (this->mq_UsageViewActive == false)
   {
      this->mpc_Ui->pc_WidgetDpItemUsage->setVisible(false);
   }
   else
   {
      this->mpc_UsageBar = new C_SdNdeDpSelectorItemUsageWidget(this);
   }

   connect(this->mpc_LabelStateImg, &C_OgeLabToolTipBase::SigLastChanceToUpdateToolTip, this,
           &C_SdNdeDpSelectorItemWidget::SigUpdateErrorToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorItemWidget::~C_SdNdeDpSelectorItemWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_LabelStateImg and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the widget

   It is only necessary if the showEvent will not be triggered.
   For example in a delegate class.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::InitWidget(void)
{
   if (this->mpc_UsageBar != NULL)
   {
      // adapt the size
      this->mpc_UsageBar->setGeometry(3, this->height() - 6, 138, 5);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error tool tip

   \param[in] orc_Heading Error tool tip heading
   \param[in] orc_Content Error tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content)
{
   if (this->mpc_LabelStateImg != NULL)
   {
      this->mpc_LabelStateImg->SetToolTipInformation(orc_Heading, orc_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the number of the item

   \param[in]     ou32_Number          Actual number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetNumber(const uint32 ou32_Number)
{
   this->mu32_Number = ou32_Number;
   this->m_UpdateName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the shown datapool data

   \param[in]     orc_OSCDataPoolId        Actual datapool ID with its information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetData(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_OSCDataPoolId)
{
   this->mc_DatapoolId = orc_OSCDataPoolId;

   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the shown datapool data

   \param[in]     orc_DatapoolName         Datapool name
   \param[in]     orc_DatapoolComment      Datapool comment
   \param[in]     oq_Safety                Is the datapool safety relevant
   \param[in]     ou32_Size                Size of the datapool
   \param[in]     ou32_Used                Used size of the datapool
   \param[in]     ou32_Reserved            Reserved size of the datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetData(const QString & orc_DatapoolName, const QString & orc_DatapoolComment,
                                          const bool oq_Safety, const uint32 ou32_Size, const uint32 ou32_Used,
                                          const uint32 ou32_Reserved)
{
   this->mpc_Ui->pc_TextEditDpName->setText(orc_DatapoolName);
   this->mpc_Ui->pc_TextEditDpComment->setText(orc_DatapoolComment);
   this->mq_StateSafety = oq_Safety;
   if (this->mpc_UsageBar != NULL)
   {
      uint32 u32_PercentageUsed;
      uint32 u32_PercentageReserved;
      QString c_Label("%1% %2");

      this->mu32_Size = ou32_Size;
      this->mu32_Used = ou32_Used;
      this->mu32_Reserved = ou32_Reserved;

      this->mpc_UsageBar->SetData(ou32_Size, ou32_Used, ou32_Reserved, u32_PercentageUsed,
                                  u32_PercentageReserved);

      // update the label
      if ((this->mu32_Reserved > this->mu32_Size) &&
          (this->mu32_Reserved > this->mu32_Used))
      {
         c_Label = c_Label.arg(QString::number(u32_PercentageReserved), C_GtGetText::h_GetText("Reservation"));
      }
      else
      {
         c_Label = c_Label.arg(QString::number(u32_PercentageUsed), C_GtGetText::h_GetText("Usage"));
      }
      this->mpc_Ui->pc_LabelUsage->setText(c_Label);
   }

   this->m_UpdateLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown datapool data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::UpdateData(void)
{
   const C_OSCNodeDataPool * const pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      this->mc_DatapoolId.u32_NodeIndex, this->mc_DatapoolId.u32_DataPoolIndex);

   tgl_assert(pc_OSCDataPool != NULL);
   if (pc_OSCDataPool != NULL)
   {
      QString c_ToolTipText;
      const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
      uint32 u32_SharedGroup = 0U;

      this->mc_Name = pc_OSCDataPool->c_Name.c_str();
      this->m_UpdateName();
      this->mpc_Ui->pc_TextEditDpComment->setText(pc_OSCDataPool->c_Comment.c_str());
      this->mq_StateSafety = pc_OSCDataPool->q_IsSafety;
      this->mq_Shared = rc_SharedDatapools.IsSharedDatapool(this->mc_DatapoolId, &u32_SharedGroup);

      // Tooltip
      c_ToolTipText = this->mpc_Ui->pc_TextEditDpComment->toPlainText();
      c_ToolTipText += C_GtGetText::h_GetText("\n\nVersion: ");
      c_ToolTipText += QString("v%1.%2r%3").
                       arg(pc_OSCDataPool->au8_Version[0], 2, 10, QChar('0')).
                       arg(pc_OSCDataPool->au8_Version[1], 2, 10, QChar('0')).
                       arg(pc_OSCDataPool->au8_Version[2], 2, 10, QChar('0'));

      c_ToolTipText += C_GtGetText::h_GetText("\n\nConfiguration:\n");

      // Protocol type for COMM datapools
      if (pc_OSCDataPool->e_Type == C_OSCNodeDataPool::eCOM)
      {
         c_ToolTipText += C_GtGetText::h_GetText("   Communication Protocol: ");
         c_ToolTipText +=
            C_PuiSdUtil::h_ConvertProtocolTypeToString(
               C_PuiSdUtil::h_GetRelatedCANProtocolType(this->mc_DatapoolId.u32_NodeIndex,
                                                        this->mc_DatapoolId.u32_DataPoolIndex));
         c_ToolTipText += "\n";
      }

      // Scope flag
      c_ToolTipText += C_GtGetText::h_GetText("   Scope of Content: ");
      if (pc_OSCDataPool->q_ScopeIsPrivate == true)
      {
         c_ToolTipText += C_GtGetText::h_GetText("Private");
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("Public");
      }

      // Safety flag
      c_ToolTipText += C_GtGetText::h_GetText("\n   Safety Relevant Content: ");
      if (pc_OSCDataPool->q_IsSafety == true)
      {
         c_ToolTipText += C_GtGetText::h_GetText("Yes");
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("No");
      }

      // Application
      c_ToolTipText += C_GtGetText::h_GetText("\n   Related Programmable Application: ");
      if (pc_OSCDataPool->s32_RelatedDataBlockIndex >= 0)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
            this->mc_DatapoolId.u32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            tgl_assert(static_cast<uint32>(pc_OSCDataPool->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size());
            if (static_cast<uint32>(pc_OSCDataPool->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size())
            {
               c_ToolTipText += pc_Node->c_Applications[pc_OSCDataPool->s32_RelatedDataBlockIndex].c_Name.c_str();
            }
         }
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("Not Assigned");
      }
      c_ToolTipText += "\n\n";

      // Share state
      if (this->mq_Shared == true)
      {
         //c_ToolTipText += C_GtGetText::h_GetText("   Shared Datapool: ");

         std::vector<QString> c_SharedDatapoolGroup;
         uint32 u32_DatapoolCounter;

         c_ToolTipText += C_GtGetText::h_GetText("Share configuration with:\n");

         C_SdNdeDpUtil::GetSharedDatapoolGroup(u32_SharedGroup, this->mc_DatapoolId,
                                               this->mc_DatapoolId.u32_NodeIndex, c_SharedDatapoolGroup);

         for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < c_SharedDatapoolGroup.size(); ++u32_DatapoolCounter)
         {
            c_ToolTipText += "   " + c_SharedDatapoolGroup[u32_DatapoolCounter] + "\n";
         }
         c_ToolTipText += "\n";
      }

      if (this->mpc_UsageBar != NULL)
      {
         uint32 u32_PercentageUsed;
         uint32 u32_PercentageReserved;
         QString c_TextUsage;
         QString c_TextReservation;
         QString c_Label("%1% %2");

         this->mu32_Size = pc_OSCDataPool->u32_NvMSize;
         this->mu32_Used = pc_OSCDataPool->GetNumBytesUsed();
         this->mu32_Reserved = pc_OSCDataPool->GetListsSize();

         this->mpc_UsageBar->SetData(this->mu32_Size, this->mu32_Used, this->mu32_Reserved, u32_PercentageUsed,
                                     u32_PercentageReserved);

         // update the label
         if ((this->mu32_Reserved > this->mu32_Size) && (this->mu32_Reserved > this->mu32_Used))
         {
            c_Label = c_Label.arg(QString::number(u32_PercentageReserved), C_GtGetText::h_GetText("Reservation"));
         }
         else
         {
            c_Label = c_Label.arg(QString::number(u32_PercentageUsed), C_GtGetText::h_GetText("Usage"));
         }
         this->mpc_Ui->pc_LabelUsage->setText(c_Label);

         // udpate the tool tip
         c_TextUsage = "   " + QString("%1% %2 (%3 / %4)\n").arg(QString::number(u32_PercentageUsed),
                                                                 C_GtGetText::h_GetText("used by parameters"),
                                                                 C_Uti::h_GetByteCountAsString(this->mu32_Used),
                                                                 C_Uti::h_GetByteCountAsString(this->mu32_Size));
         c_TextReservation = "   " + QString("%1% %2 (%3 / %4)").arg(QString::number(u32_PercentageReserved),
                                                                     C_GtGetText::h_GetText("reserved by lists"),
                                                                     C_Uti::h_GetByteCountAsString(this->mu32_Reserved),
                                                                     C_Uti::h_GetByteCountAsString(this->mu32_Size));

         c_ToolTipText += "NVM Statistics:\n" + c_TextUsage + c_TextReservation;
      }

      // update the tool tip
      this->SetToolTipInformation(this->mc_Name, c_ToolTipText);

      this->m_UpdateLabel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget active or inactive

   Adapts the stylesheet

   \param[in]     oq_Active   Is widget active or not active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetActive(const bool oq_Active)
{
   if (oq_Active == true)
   {
      // adapt the styling with the stylesheet
      this->setStyleSheet("QFrame#pc_FrameSdNdeDpSelectorItem {"
                          "background-color: " + mc_STYLESHEET_GUIDE_COLOR_4 + ";"
                          "}"
                          "QTextEdit#pc_TextEditDpName {"
                          "color: " + mc_STYLESHEET_GUIDE_COLOR_0 + ";"
                          "}");
   }
   else
   {
      // reset stylesheet
      this->setStyleSheet("");
   }

   this->mq_Active = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets conflict state of the widget

   Enables or disables the conflict icon

   \param[in]     oq_Active   Is conflict active or not active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetStateConflict(const bool oq_Active)
{
   this->mq_StateConflict = oq_Active;
   this->m_UpdateLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the shared datapool flag

   \param[in]     oq_Shared       Flag if datapool is shared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetShareDatapool(const bool oq_Shared)
{
   this->mq_Shared = oq_Shared;
   this->m_UpdateLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current Datapool Id

   \return
   Datapool Id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolId & C_SdNdeDpSelectorItemWidget::GetDatapoolId(void) const
{
   return this->mc_DatapoolId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the datapool name

   \return
   Name of datapool
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpSelectorItemWidget::GetDatapoolName(void) const
{
   return this->mpc_Ui->pc_TextEditDpName->toPlainText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the datapool comment

   \return
   Comment of datapool
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpSelectorItemWidget::GetDatapoolComment(void) const
{
   return this->mpc_Ui->pc_TextEditDpComment->toPlainText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the safety state

   \return
   Safety state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetStateSafety(void) const
{
   return this->mq_StateSafety;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the conflict state

   \return
   Conflict state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetStateConflict(void) const
{
   return this->mq_StateConflict;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the shared flag of the datapool

   \return
   true     Datapool is shared
   false    Datapool is not shared
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetShareDatapool(void) const
{
   return this->mq_Shared;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the widget in maximized or minimized mode

   \param[in]     oq_Maximized   Flag for maximized
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetMaximized(const bool oq_Maximized)
{
   this->mpc_Ui->pc_TextEditDpComment->setVisible(oq_Maximized);

   if (this->mpc_UsageBar != NULL)
   {
      this->mpc_Ui->pc_WidgetDpItemUsage->setVisible(oq_Maximized);
      this->mpc_UsageBar->setVisible(oq_Maximized);
   }

   if (oq_Maximized == true)
   {
      this->mpc_Ui->pc_FrameSdNdeDpSelectorItem->setMaximumHeight(140);
      this->mpc_Ui->pc_FrameSdNdeDpSelectorItem->setMinimumHeight(140);
      this->setMaximumHeight(hc_MaximumSize.height());
      this->setMinimumHeight(hc_MaximumSize.height());
   }
   else
   {
      this->mpc_Ui->pc_FrameSdNdeDpSelectorItem->setMinimumHeight(50);
      this->mpc_Ui->pc_FrameSdNdeDpSelectorItem->setMaximumHeight(50);
      this->setMaximumHeight(hc_MinimumSize.height());
      this->setMinimumHeight(hc_MinimumSize.height());
      // the widget would not adapt the size correctly in the delegate instance if resize will not be called
      this->resize(hc_MinimumSize);
   }

   this->update();

   this->mq_Maximized = oq_Maximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the maximized state of the widget

   \return
   Flag for maximized
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetMaximized(void) const
{
   return this->mq_Maximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the active state of the widget

   \return
   Flag for active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetActive(void) const
{
   return this->mq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the usage view

   \return
   Flag for usage view active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetUsageViewActive() const
{
   return this->mq_UsageViewActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the size of the datapool

   \return
   Size of datapool
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDpSelectorItemWidget::GetDataPoolSize(void) const
{
   return this->mu32_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the size of the used datapool

   \return
   Size of used datapool
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDpSelectorItemWidget::GetDataPoolUsedSize(void) const
{
   return this->mu32_Used;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the size of the reserved datapool

   \return
   Size of reserved datapool
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDpSelectorItemWidget::GetDataPoolReservedSize(void) const
{
   return this->mu32_Reserved;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On Show Event

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::showEvent(QShowEvent * const opc_Event)
{
   QWidget::showEvent(opc_Event);

   this->InitWidget();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::m_UpdateLabel(void)
{
   // adapt the color of the percentage label
   if ((this->mu32_Used > this->mu32_Size) ||
       (this->mu32_Reserved > this->mu32_Size) ||
       (this->mu32_Used > this->mu32_Reserved))
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsage, "Conflict", true);
   }
   else
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsage, "Conflict", false);
   }

   // adapt the icon
   if (this->mpc_LabelStateImg != NULL)
   {
      if (this->mq_StateConflict == true)
      {
         this->mpc_LabelStateImg->setPixmap(this->mc_ConflictImg);
         this->mpc_LabelStateImg->setVisible(true);
         //Activate tool tip
         this->mpc_LabelStateImg->SetToolTipInformation("Not visible", "Not visible");
      }
      //safety icon deactivated so far (SSI, 13.03.18)
      // - Layout is awful
      // - The role of safety property not clear so far
      //   else if (this->mq_StateSafety == true)
      //   {
      //      this->mpc_LabelStateImg->setPixmap(this->mc_SafetyImg);
      //      this->mpc_LabelStateImg->setVisible(true);
      //   }
      else
      {
         // nothing to paint
         this->mpc_LabelStateImg->setVisible(false);
      }
   }

   // adapt the shared icon
   if (this->mpc_LabelShareImg != NULL)
   {
      this->mpc_LabelShareImg->setVisible(this->mq_Shared);
   }

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::m_UpdateName(void) const
{
   this->mpc_Ui->pc_TextEditDpName->setText(QString("#") + QString::number(this->mu32_Number) + QString(" - ") +
                                            this->mc_Name);
}
