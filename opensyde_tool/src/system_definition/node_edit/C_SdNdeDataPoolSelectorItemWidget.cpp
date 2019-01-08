//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       A datapool item widget for the list widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include <QPainter>

#include "constants.h"

#include "C_SdNdeDataPoolSelectorItemWidget.h"
#include "ui_C_SdNdeDataPoolSelectorItemWidget.h"

#include "C_Uti.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const QSize C_SdNdeDataPoolSelectorItemWidget::hc_MaximumSize = QSize(145, 151);
const QSize C_SdNdeDataPoolSelectorItemWidget::hc_MinimumSize = QSize(145, 61);

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     oq_UsageViewActive   Flag for showing usage bars for datapool
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolSelectorItemWidget::C_SdNdeDataPoolSelectorItemWidget(const bool oq_UsageViewActive,
                                                                     QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDataPoolSelectorItemWidget),
   mpc_UsageBar(NULL),
   mq_StateSafety(false),
   mq_StateConflict(false),
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
   this->mc_SafetyImg.load(":images/SafetyIcon24px.png");

   this->mpc_LabelStateImg = new C_OgeLabToolTipBase(this);
   this->mpc_LabelStateImg->raise();
   this->mpc_LabelStateImg->move(114, 0);
   this->mpc_LabelStateImg->setVisible(false);

   if (this->mq_UsageViewActive == false)
   {
      this->mpc_Ui->pc_WidgetDpItemUsage->setVisible(false);
   }
   else
   {
      this->mpc_UsageBar = new C_SdNdeDataPoolSelectorItemUsageWidget(this);
   }

   connect(this->mpc_LabelStateImg, &C_OgeLabToolTipBase::SigLastChanceToUpdateToolTip, this,
           &C_SdNdeDataPoolSelectorItemWidget::SigUpdateErrorToolTip);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolSelectorItemWidget::~C_SdNdeDataPoolSelectorItemWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_LabelStateImg and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialization of the widget

   It is only necessary if the showEvent will not be triggered.
   For example in a delegate class.

   \created     24.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::InitWidget(void)
{
   if (this->mpc_UsageBar != NULL)
   {
      // adapt the size
      this->mpc_UsageBar->setGeometry(3, this->height() - 6, 138, 5);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Error tool tip

   \param[in] orc_Heading Error tool tip heading
   \param[in] orc_Content Error tool tip content

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content)
{
   if (this->mpc_LabelStateImg != NULL)
   {
      this->mpc_LabelStateImg->SetToolTipInformation(orc_Heading, orc_Content, C_NagToolTip::eERROR);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the number of the item

   \param[in]     ou32_Number          Actual number

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetNumber(const uint32 ou32_Number)
{
   this->mu32_Number = ou32_Number;
   this->m_UpdateName();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the shown datapool data

   \param[in]     orc_OSCDataPool          Actual datapool with its informations

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetData(const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool)
{
   this->mc_Name = orc_OSCDataPool.c_Name.c_str();
   this->m_UpdateName();
   this->mpc_Ui->pc_TextEditDpComment->setText(orc_OSCDataPool.c_Comment.c_str());
   this->mq_StateSafety = orc_OSCDataPool.q_IsSafety;

   if (this->mpc_UsageBar != NULL)
   {
      uint32 u32_PercentageUsed;
      uint32 u32_PercentageReserved;
      QString c_TextUsage;
      QString c_TextReservation;
      QString c_ToolTipText;
      QString c_Label("%1% %2");

      this->mu32_Size = orc_OSCDataPool.u32_NvMSize;
      this->mu32_Used = orc_OSCDataPool.GetNumBytesUsed();
      this->mu32_Reserved = orc_OSCDataPool.GetListsSize();

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
      c_TextUsage = QString("%1% %2 (%3 / %4)\n").arg(QString::number(u32_PercentageUsed),
                                                      C_GtGetText::h_GetText("used by parameters"),
                                                      C_Uti::h_GetByteCountAsString(this->mu32_Used),
                                                      C_Uti::h_GetByteCountAsString(this->mu32_Size));
      c_TextReservation = QString("%1% %2 (%3 / %4)").arg(QString::number(u32_PercentageReserved),
                                                          C_GtGetText::h_GetText("reserved by lists"),
                                                          C_Uti::h_GetByteCountAsString(this->mu32_Reserved),
                                                          C_Uti::h_GetByteCountAsString(this->mu32_Size));

      c_ToolTipText = this->mpc_Ui->pc_TextEditDpComment->toPlainText();
      if (c_ToolTipText != "")
      {
         c_ToolTipText += "\n\n";
      }
      c_ToolTipText += "Statistics:\n" + c_TextUsage + c_TextReservation;

      this->SetToolTipInformation(this->mc_Name, c_ToolTipText);
   }
   else
   {
      // update the tool tip
      this->SetToolTipInformation(this->mc_Name, this->mpc_Ui->pc_TextEditDpComment->toPlainText());
   }

   this->m_UpdateLabel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the shown datapool data

   \param[in]     orc_DatapoolName         Datapool name
   \param[in]     orc_DatapoolComment      Datapool comment
   \param[in]     oq_Safety                Is the datapool safety relevant
   \param[in]     ou32_Size                Size of the datapool
   \param[in]     ou32_Used                Used size of the datapool
   \param[in]     ou32_Reserved            Reserved size of the datapool

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetData(const QString & orc_DatapoolName, const QString & orc_DatapoolComment,
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the widget active or inactive

   Adapts the stylesheet

   \param[in]     oq_Active   Is widget active or not active

   \created     06.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetActive(const bool oq_Active)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sets conflict state of the widget

   Enables or disables the conflict icon

   \param[in]     oq_Active   Is conflict active or not active

   \created     22.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetStateConflict(const bool oq_Active)
{
   this->mq_StateConflict = oq_Active;
   this->m_UpdateLabel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the datapool name

   \return
   Name of datapool

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolSelectorItemWidget::GetDatapoolName(void) const
{
   return this->mpc_Ui->pc_TextEditDpName->toPlainText();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the datapool comment

   \return
   Comment of datapool

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_SdNdeDataPoolSelectorItemWidget::GetDatapoolComment(void) const
{
   return this->mpc_Ui->pc_TextEditDpComment->toPlainText();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the safety state

   \return
   Safety state

   \created     07.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorItemWidget::GetStateSafety(void) const
{
   return this->mq_StateSafety;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the conflict state

   \return
   Conflict state

   \created     07.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorItemWidget::GetStateConflict(void) const
{
   return this->mq_StateConflict;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the widget in maximized or minimized mode

   \param[in]     oq_Maximized   Flag for maximized

   \created     07.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::SetMaximized(const bool oq_Maximized)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the maximized state of the widget

   \return
   Flag for maximized

   \created     08.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorItemWidget::GetMaximized(void) const
{
   return this->mq_Maximized;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the active state of the widget

   \return
   Flag for active

   \created     21.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorItemWidget::GetActive(void) const
{
   return this->mq_Active;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the state of the usage view

   \return
   Flag for usage view active

   \created     23.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorItemWidget::GetUsageViewActive() const
{
   return this->mq_UsageViewActive;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the size of the datapool

   \return
   Size of datapool

   \created     23.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDataPoolSelectorItemWidget::GetDataPoolSize(void) const
{
   return this->mu32_Size;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the size of the used datapool

   \return
   Size of used datapool

   \created     23.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDataPoolSelectorItemWidget::GetDataPoolUsedSize(void) const
{
   return this->mu32_Used;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the size of the reserved datapool

   \return
   Size of reserved datapool

   \created     29.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_SdNdeDataPoolSelectorItemWidget::GetDataPoolReservedSize(void) const
{
   return this->mu32_Reserved;
}

//-----------------------------------------------------------------------------
/*!
   \brief   On Show Event

   \param[in,out] opc_Event  Pointer to paint event

   \created     23.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::showEvent(QShowEvent * const opc_Event)
{
   QWidget::showEvent(opc_Event);

   this->InitWidget();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::m_UpdateLabel(void)
{
   // adapt the color of the percentage label
   if ((this->mu32_Used > this->mu32_Size) ||
       (this->mu32_Reserved > this->mu32_Size) ||
       (this->mu32_Used > this->mu32_Reserved))
   {
      //Deactivate error tool tip on interaction item
      //const QString c_Heading = C_GtGetText::h_GetText("Invalid list content");
      //QString c_Content;
      //if (this->mu32_Reserved > this->mu32_Size)
      //{
      //   c_Content += C_GtGetText::h_GetText("- Lists use too much space");
      //}
      //if (this->mu32_Used > this->mu32_Reserved)
      //{
      //   c_Content += C_GtGetText::h_GetText("- Parameters use more space than available in the lists");
      //}
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsage, "Conflict", true);
      //this->mpc_Ui->pc_LabelUsage->SetToolTipInformation(c_Heading, c_Content, true);
      //if (this->mpc_UsageBar != NULL)
      //{
      //   this->mpc_UsageBar->SetToolTipInformation(c_Heading, c_Content, true);
      //}
   }
   else
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelUsage, "Conflict", false);
      //Deactivate error tool tip on interaction item
      //this->mpc_Ui->pc_LabelUsage->SetToolTipInformation("", "", false);
      //if (this->mpc_UsageBar != NULL)
      //{
      //   this->mpc_UsageBar->SetToolTipInformation("", "", false);
      //}
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
   this->update();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorItemWidget::m_UpdateName(void) const
{
   this->mpc_Ui->pc_TextEditDpName->setText(QString("#") + QString::number(this->mu32_Number) + QString(" - ") +
                                            this->mc_Name);
}
