//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for navigation bar tab (implementation)

   Widget for navigation bar tab

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGroupBox>
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_OgeWiNavigationTab.h"
#include "ui_C_OgeWiNavigationTab.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeWiNavigationTab::C_OgeWiNavigationTab(QWidget * const opc_Parent) :
   QPushButton(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiNavigationTab),
   mq_Active(false)
{
   this->mpc_Ui->setupUi(this);

   //Remove debug text
   this->mpc_Ui->pc_GroupBoxStyle->setTitle("");

   //Here left border is always necessary
   this->mpc_Ui->pc_GroupBoxStyle->SetIncludeLeftBorder(true);

   this->mpc_Ui->pc_LabelText->setAlignment(Qt::AlignCenter);
   this->mpc_Ui->pc_LabelText->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelText->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelText->SetFontPixel(14, true);
   //Remove debug text
   this->mpc_Ui->pc_LabelIcon->setText("");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeWiNavigationTab::~C_OgeWiNavigationTab(void)
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set include left border flag

   \param[in] oq_Include Include left border flag

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiNavigationTab::SetIncludeLeftBorder(const bool oq_Include)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IncludeLeftBorder", oq_Include);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new active state

   \param[in] oq_Active New active state

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiNavigationTab::SetActive(const bool oq_Active)
{
   QColor c_Color = mc_STYLE_GUIDE_COLOR_21;

   if (oq_Active == false)
   {
      c_Color.setAlpha(0);
   }
   this->mq_Active = oq_Active;
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", oq_Active);
   this->mpc_Ui->pc_FrameBottomLine->setStyleSheet(QString("background-color:rgba(%1,%2,%3,%4);").arg(
                                                      c_Color.red()).arg(c_Color.green()).arg(c_Color.blue()).arg(
                                                      c_Color.alpha()));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new SVG icon

   \param[in] orc_IconSvg New icon

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiNavigationTab::SetIconSvg(const QString & orc_IconSvg) const
{
   this->mpc_Ui->pc_LabelIcon->SetSvg(orc_IconSvg);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new text

   \param[in] orc_NewText New text

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiNavigationTab::SetText(const QString & orc_NewText) const
{
   this->mpc_Ui->pc_LabelText->setText(orc_NewText);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeWiNavigationTab::paintEvent(QPaintEvent * const opc_Event)
{
   C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten event slot

   Here: Check for hover event

   \param[in,out] opc_Event Event identification and information

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeWiNavigationTab::event(QEvent * const opc_Event)
{
   if ((opc_Event->type() == QEvent::HoverMove) && (this->mq_Active == false))
   {
      this->mpc_Ui->pc_LabelText->SetForegroundColor(13);
   }
   else if (opc_Event->type() == QEvent::Leave)
   {
      this->mpc_Ui->pc_LabelText->SetForegroundColor(0);
   }
   else
   {
      //No special handling necessary
   }
   return QPushButton::event(opc_Event);
}
