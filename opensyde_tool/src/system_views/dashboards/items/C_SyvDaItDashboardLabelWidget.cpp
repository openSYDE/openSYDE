//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard label widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_SyvDaItDashboardLabelWidget.h"
#include "ui_C_SyvDaItDashboardLabelWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItDashboardLabelWidget::C_SyvDaItDashboardLabelWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItDashboardLabelWidget),
   mq_ShowUnit(true)
{
   QFont c_Font;

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LabelValue->SetAllowAutomatedAdaptation(false);

   //Caption
   c_Font = this->mpc_Ui->pc_LabelCaption->font();
   c_Font.setFamily("Segoe UI");
   c_Font.setBold(true);
   this->mpc_Ui->pc_LabelCaption->setFont(c_Font);

   //Value
   c_Font = this->mpc_Ui->pc_LabelValue->font();
   c_Font.setFamily("Segoe UI");
   c_Font.setBold(false);
   this->mpc_Ui->pc_LabelValue->setFont(c_Font);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItDashboardLabelWidget::~C_SyvDaItDashboardLabelWidget(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reset font after stylesheet polish action

   \created     22.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::ResetFont(void) const
{
   this->mpc_Ui->pc_LabelValue->ResetFont();
   m_UpdateCaptionFont();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set caption label text

   \param[in] orc_Text New value

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::SetCaption(const QString & orc_Text) const
{
   this->mpc_Ui->pc_LabelCaption->setText(orc_Text.toUpper());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set value label text

   \param[in] orc_Text New value

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::SetValue(const QString & orc_Text) const
{
   if (this->mq_ShowUnit == true)
   {
      this->mpc_Ui->pc_LabelValue->setText(QString("%1 %2").arg(orc_Text).arg(this->mc_Unit));
   }
   else
   {
      this->mpc_Ui->pc_LabelValue->setText(orc_Text);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set unit

   \param[in] orc_Text New unit

   \created     04.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::SetUnit(const QString & orc_Text)
{
   this->mc_Unit = orc_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::AdjustFontToSize(void) const
{
   const QSize c_ExpectedSize(this->width(), (this->height() * 10) / 17);

   this->mpc_Ui->pc_LabelValue->AdjustFontToSpecificSize(c_ExpectedSize);
   this->m_UpdateCaptionFont();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set display style

   \param[in] oe_Type        Type
   \param[in] oq_ShowCaption Show caption
   \param[in] oq_ShowUnit    Show unit

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::SetDisplayStyle(const C_PuiSvDbLabel::E_Type oe_Type, const bool oq_ShowCaption,
                                                    const bool oq_ShowUnit)
{
   switch (oe_Type)
   {
   case C_PuiSvDbLabel::eTRANSPARENT:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Type", "TRANSPARENT");
      break;
   default:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Type", "DEFAULT");
      break;
   }
   this->mpc_Ui->pc_LabelCaption->setVisible(oq_ShowCaption);
   this->mq_ShowUnit = oq_ShowUnit;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Adapt caption based on value label

   \param[in,out] opc_Event Event identification and information

   \created     11.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   this->AdjustFontToSize();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Adapt caption based on value label

   \param[in,out] opc_Event Event identification and information

   \created     11.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::showEvent(QShowEvent * const opc_Event)
{
   QWidget::showEvent(opc_Event);
   this->AdjustFontToSize();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt caption relative to value label

   \created     11.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardLabelWidget::m_UpdateCaptionFont(void) const
{
   QFont c_FontCaption;
   QFont c_FontValue;

   c_FontValue = this->mpc_Ui->pc_LabelValue->font();
   c_FontCaption = this->mpc_Ui->pc_LabelCaption->font();
   c_FontCaption.setPointSize(std::max((c_FontValue.pointSize() * 7) / 17, 1));
   this->mpc_Ui->pc_LabelCaption->setFont(c_FontCaption);
}
