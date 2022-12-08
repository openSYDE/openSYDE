//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A datapool item widget for the list widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>

#include "constants.hpp"

#include "C_SdNdeDpSelectorItemWidget.hpp"
#include "ui_C_SdNdeDpSelectorItemWidget.h"

#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "TglUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OscNode.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
   mc_DatapoolId(C_OscNodeDataPoolId()),
   mq_StateSafety(false),
   mq_StateConflict(false),
   mq_Shared(false),
   mq_Selected(false),
   mq_UsageViewActive(oq_UsageViewActive),
   mu32_Size(0U),
   mu32_Used(0U),
   mu32_Reserved(0U),
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
   this->mpc_LabelShareImg->move(119, 33);
   this->mpc_LabelShareImg->setVisible(false);
   this->mpc_LabelShareImg->setPixmap(QIcon("://images/system_definition/IconShare.svg").pixmap(mc_ICON_SIZE_16));

   this->mpc_LabelShareImgHovered = new QLabel(this);
   this->mpc_LabelShareImgHovered->raise();
   this->mpc_LabelShareImgHovered->move(119, 33);
   this->mpc_LabelShareImgHovered->setVisible(false);
   this->mpc_LabelShareImgHovered->setPixmap(QIcon("://images/system_definition/IconShareHover.svg").
                                             pixmap(mc_ICON_SIZE_16));
   this->mpc_LabelShareImgHovered->setVisible(false);

   if (this->mq_UsageViewActive == false)
   {
      this->mpc_Ui->pc_WidgetDpItemUsage->setVisible(false);
   }
   else
   {
      this->mpc_UsageBar = new C_SdNdeDpSelectorItemUsageWidget(this);
   }

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this->mpc_LabelStateImg, &C_OgeLabToolTipBase::SigLastChanceToUpdateToolTip, this,
           &C_SdNdeDpSelectorItemWidget::m_UpdateErrorToolTip);
   connect(this, &C_SdNdeDpSelectorItemWidget::customContextMenuRequested, this,
           &C_SdNdeDpSelectorItemWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorItemWidget::~C_SdNdeDpSelectorItemWidget()
{
   delete mpc_Ui;

   //Qt element destruction handled by Qt engine; only NULL here
   mpc_LabelStateImg = NULL;
   mpc_LabelShareImg = NULL;
   mpc_LabelShareImgHovered = NULL;
   mpc_UsageBar = NULL;
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
void C_SdNdeDpSelectorItemWidget::SetNumber(const uint32_t ou32_Number)
{
   this->mu32_Number = ou32_Number;
   this->m_UpdateName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the shown datapool data

   \param[in]     orc_OscDataPoolId        Actual datapool ID with its information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetData(const stw::opensyde_core::C_OscNodeDataPoolId & orc_OscDataPoolId)
{
   this->mc_DatapoolId = orc_OscDataPoolId;

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
                                          const bool oq_Safety, const uint32_t ou32_Size, const uint32_t ou32_Used,
                                          const uint32_t ou32_Reserved)
{
   this->mpc_Ui->pc_TextEditDpName->setText(orc_DatapoolName);
   this->mpc_Ui->pc_TextEditDpComment->setText(orc_DatapoolComment);
   this->mq_StateSafety = oq_Safety;
   if (this->mpc_UsageBar != NULL)
   {
      uint32_t u32_PercentageUsed;
      uint32_t u32_PercentageReserved;
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
   const C_OscNodeDataPool * const pc_OscDataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
      this->mc_DatapoolId.u32_NodeIndex, this->mc_DatapoolId.u32_DataPoolIndex);

   tgl_assert(pc_OscDataPool != NULL);
   if (pc_OscDataPool != NULL)
   {
      QString c_ToolTipText;
      const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
      uint32_t u32_SharedGroup = 0U;

      this->mc_Name = pc_OscDataPool->c_Name.c_str();
      this->m_UpdateName();
      this->mpc_Ui->pc_TextEditDpComment->setText(pc_OscDataPool->c_Comment.c_str());
      this->mq_StateSafety = pc_OscDataPool->q_IsSafety;
      this->mq_Shared = rc_SharedDatapools.IsSharedDatapool(this->mc_DatapoolId, &u32_SharedGroup);

      // Tooltip
      c_ToolTipText = this->mpc_Ui->pc_TextEditDpComment->toPlainText();
      c_ToolTipText += C_GtGetText::h_GetText("\n\nVersion: ");
      c_ToolTipText += static_cast<QString>("v%1.%2r%3").
                       arg(pc_OscDataPool->au8_Version[0], 2, 10, QChar('0')).
                       arg(pc_OscDataPool->au8_Version[1], 2, 10, QChar('0')).
                       arg(pc_OscDataPool->au8_Version[2], 2, 10, QChar('0'));

      c_ToolTipText += C_GtGetText::h_GetText("\n\nConfiguration:\n");

      // Protocol type for COMM datapools
      if (pc_OscDataPool->e_Type == C_OscNodeDataPool::eCOM)
      {
         c_ToolTipText += C_GtGetText::h_GetText("   Communication Protocol: ");
         c_ToolTipText +=
            C_PuiSdUtil::h_ConvertProtocolTypeToString(
               C_PuiSdUtil::h_GetRelatedCanProtocolType(this->mc_DatapoolId.u32_NodeIndex,
                                                        this->mc_DatapoolId.u32_DataPoolIndex));
         c_ToolTipText += "\n";
      }

      // Scope flag
      c_ToolTipText += C_GtGetText::h_GetText("   Scope of Content: ");
      if (pc_OscDataPool->q_ScopeIsPrivate == true)
      {
         c_ToolTipText += C_GtGetText::h_GetText("Private");
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("Public");
      }

      // Safety flag
      c_ToolTipText += C_GtGetText::h_GetText("\n   Safety Relevant Content: ");
      if (pc_OscDataPool->q_IsSafety == true)
      {
         c_ToolTipText += C_GtGetText::h_GetText("Yes");
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("No");
      }

      // Application
      c_ToolTipText += C_GtGetText::h_GetText("\n   Related Data Block: ");
      if (pc_OscDataPool->s32_RelatedDataBlockIndex >= 0)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
            this->mc_DatapoolId.u32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            tgl_assert(static_cast<uint32_t>(pc_OscDataPool->s32_RelatedDataBlockIndex) <
                       pc_Node->c_Applications.size());
            if (static_cast<uint32_t>(pc_OscDataPool->s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size())
            {
               c_ToolTipText += pc_Node->c_Applications[pc_OscDataPool->s32_RelatedDataBlockIndex].c_Name.c_str();
            }
         }
      }
      else
      {
         c_ToolTipText += C_GtGetText::h_GetText("<not assigned>");
      }
      c_ToolTipText += "\n\n";

      // Share state
      if (this->mq_Shared == true)
      {
         std::vector<QString> c_SharedDatapoolGroup;
         uint32_t u32_DatapoolCounter;

         c_ToolTipText += C_GtGetText::h_GetText("Share configuration with:\n");

         C_SdNdeDpUtil::h_GetSharedDatapoolGroup(u32_SharedGroup, this->mc_DatapoolId,
                                                 this->mc_DatapoolId.u32_NodeIndex, c_SharedDatapoolGroup);

         for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < c_SharedDatapoolGroup.size(); ++u32_DatapoolCounter)
         {
            c_ToolTipText += "   " + c_SharedDatapoolGroup[u32_DatapoolCounter] + "\n";
         }
         c_ToolTipText += "\n";
      }

      if (this->mpc_UsageBar != NULL)
      {
         uint32_t u32_PercentageUsed;
         uint32_t u32_PercentageReserved;
         QString c_StartAddress;
         QString c_TextUsage;
         QString c_TextReservation;
         QString c_Label("%1% %2");

         this->mu32_Size = pc_OscDataPool->u32_NvmSize;
         this->mu32_Used = pc_OscDataPool->GetNumBytesUsed();
         this->mu32_Reserved = pc_OscDataPool->GetListsSize();

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
         c_StartAddress = static_cast<QString>("   %1: %2\n").arg(C_GtGetText::h_GetText("Start Address"),
                                                                  QString::number(pc_OscDataPool->u32_NvmStartAddress));

         c_TextUsage = "   " + static_cast<QString>("%1% %2 (%3 / %4)\n").arg(QString::number(u32_PercentageUsed),

                                                                              C_GtGetText::h_GetText(
                                                                                 "used by parameters"),
                                                                              C_Uti::h_GetByteCountAsString(this->
                                                                                                            mu32_Used),
                                                                              C_Uti::h_GetByteCountAsString(this->
                                                                                                            mu32_Size));
         c_TextReservation = "   " + static_cast<QString>("%1% %2 (%3 / %4)").arg(QString::number(
                                                                                     u32_PercentageReserved),
                                                                                  C_GtGetText::h_GetText(
                                                                                     "reserved by lists"),
                                                                                  C_Uti::h_GetByteCountAsString(this->
                                                                                                                mu32_Reserved),
                                                                                  C_Uti::h_GetByteCountAsString(this->
                                                                                                                mu32_Size));

         c_ToolTipText += "Memory Usage:\n" + c_StartAddress + c_TextUsage + c_TextReservation;
      }

      // update the tool tip
      this->SetToolTipInformation(this->mc_Name, c_ToolTipText);

      this->m_UpdateLabel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the selected state

   \param[in]  oq_Selected    Flag if selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::SetSelected(const bool oq_Selected)
{
   this->m_SetSelectColor(oq_Selected);

   this->mq_Selected = oq_Selected;
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
const C_OscNodeDataPoolId & C_SdNdeDpSelectorItemWidget::GetDatapoolId(void) const
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
/*! \brief   Returns the active state of the widget

   \return
   Flag for active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::GetSelected(void) const
{
   return this->mq_Selected;
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
uint32_t C_SdNdeDpSelectorItemWidget::GetDataPoolSize(void) const
{
   return this->mu32_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the size of the used datapool

   \return
   Size of used datapool
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDpSelectorItemWidget::GetDataPoolUsedSize(void) const
{
   return this->mu32_Used;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the size of the reserved datapool

   \return
   Size of reserved datapool
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDpSelectorItemWidget::GetDataPoolReservedSize(void) const
{
   return this->mu32_Reserved;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden mouse press event

   Selecting or unselecting the widget

   \param[in,out]  opc_Event  Pointer to mouse event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (opc_Event->buttons() == static_cast<int32_t>(Qt::LeftButton))
   {
      Q_EMIT (this->SigClicked(this->mu32_Number - 1U));
   }

   QWidget::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten double click event

   Open version edit on double click

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   if (opc_Event->buttons() == static_cast<int32_t>(Qt::LeftButton))
   {
      Q_EMIT (this->SigDoubleClicked(this->mu32_Number - 1U));
   }

   QWidget::mouseDoubleClickEvent(opc_Event);
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
/*! \brief   Overwritten default event slot

   Here: Necessary that the event function leaveEvent is called. Reason: Unknown

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorItemWidget::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::Leave)
   {
      if ((this->mq_Selected == false) &&
          (this->mpc_LabelShareImg != NULL) &&
          (this->mpc_LabelShareImgHovered != NULL))
      {
         this->mpc_LabelShareImg->setVisible(this->mq_Shared);
         this->mpc_LabelShareImgHovered->setVisible(false);
      }
      Q_EMIT (this->SigHoverStateChanged(this->mc_DatapoolId.u32_DataPoolIndex, false));
   }
   else if (opc_Event->type() == QEvent::Enter)
   {
      if ((this->mq_Selected == false) &&
          (this->mpc_LabelShareImg != NULL) &&
          (this->mpc_LabelShareImgHovered != NULL))
      {
         this->mpc_LabelShareImgHovered->setVisible(this->mq_Shared);
         this->mpc_LabelShareImg->setVisible(false);
      }
      Q_EMIT (this->SigHoverStateChanged(this->mc_DatapoolId.u32_DataPoolIndex, true));
   }
   else
   {
      // Nothing to do
   }

   return C_OgeWiWithToolTip::event(opc_Event);
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
         this->mpc_LabelStateImg->SetToolTipInformation(C_GtGetText::h_GetText("Not visible"),
                                                        C_GtGetText::h_GetText("Not visible"));
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
   this->mpc_Ui->pc_TextEditDpName->setText(static_cast<QString>("#") + QString::number(
                                               this->mu32_Number) + static_cast<QString>(" - ") +
                                            this->mc_Name);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color of the item in case of selection or hover event

   \param[in]       oq_Active     Flag if the color will be set or removed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::m_SetSelectColor(const bool oq_Active)
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends a signal for updating the error tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::m_UpdateErrorToolTip(void)
{
   Q_EMIT (this->SigUpdateErrorToolTip(this->mu32_Number - 1U));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   Q_EMIT (this->SigContextMenuRequested(this->mapToParent(orc_Pos), this->mu32_Number - 1U));
}
