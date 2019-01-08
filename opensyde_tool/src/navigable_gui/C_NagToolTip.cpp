//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tool tip window (implementation)

   Tool tip window

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsDropShadowEffect>
#include "stwtypes.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_NagToolTip.h"
#include "ui_C_NagToolTip.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
std::vector<QPointer<C_NagToolTip> > C_NagToolTip::mhc_ExistingToolTips;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] orc_Heading Optional heading
   \param[in] orc_Content Optional content
   \param[in] opc_Parent  Optional parent

   \created     25.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagToolTip::C_NagToolTip(const QString & orc_Heading, const QString & orc_Content, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagToolTip)
{
   mpc_Ui->setupUi(this);

   SetHeading(orc_Heading);
   SetContent(orc_Content);
   SetType(eDEFAULT);

   this->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
   this->setAttribute(Qt::WA_TranslucentBackground);

   //shadow
   {
      QColor c_Color;
      QGraphicsDropShadowEffect * pc_Shadow;

      pc_Shadow = new QGraphicsDropShadowEffect(this->mpc_Ui->mpc_GroupBox);
      pc_Shadow->setBlurRadius(10.0);
      pc_Shadow->setOffset(3.0);
      c_Color = mc_STYLE_GUIDE_COLOR_34;
      c_Color.setAlpha(128);
      pc_Shadow->setColor(c_Color);
      this->mpc_Ui->mpc_GroupBox->setGraphicsEffect(pc_Shadow);
      //lint -e{429}  no memory leak because of the parent of pc_Shadow and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     25.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagToolTip::~C_NagToolTip()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new heading

   \param[in] orc_Heading New heading

   \created     25.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::SetHeading(const QString & orc_Heading) const
{
   this->mpc_Ui->mpc_Heading->setText(orc_Heading);
   if (orc_Heading.compare("") == 0)
   {
      this->mpc_Ui->mpc_Heading->setVisible(false);
   }
   else
   {
      this->mpc_Ui->mpc_Heading->setVisible(true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new content

   \param[in] orc_Content New content

   \created     25.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::SetContent(const QString & orc_Content) const
{
   this->mpc_Ui->mpc_Content->setText(orc_Content.trimmed()); // remove trailing new line chars with QString::trimmed()
   if (orc_Content.compare("") == 0)
   {
      this->mpc_Ui->mpc_Content->setVisible(false);
   }
   else
   {
      this->mpc_Ui->mpc_Content->setVisible(true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set tooltip type

   \param[in] oe_Type Tooltip type

   \created     16.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::SetType(const C_NagToolTip::E_Type oe_Type) const
{
   QString c_NewType;

   switch (oe_Type)
   {
   case eDEFAULT:
      c_NewType = "DEFAULT";
      break;
   case eWARNING:
      c_NewType = "WARNING";
      break;
   case eERROR:
      c_NewType = "ERROR";
      break;
   }
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_GroupBox, "Type", c_NewType);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_Heading, "Type", c_NewType);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_Content, "Type", c_NewType);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move widget position

   \param[in] orc_GlobalPos Global target position of top left corner

   \created     01.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::DoMove(const QPoint & orc_GlobalPos)
{
   // get available space of monitor where tool-tip was requested (e.g. subtract windows task bar from whole monitor)
   const QDesktopWidget * const pc_Desktop = QApplication::desktop();
   const QRect c_RectMonitor = pc_Desktop->availableGeometry(orc_GlobalPos);

   stw_types::sintn sn_NewY = orc_GlobalPos.y();
   stw_types::sintn sn_NewX = orc_GlobalPos.x();
   sintn sn_DistanceToBoarder = 20; //space between tooltip and boarder

   this->updateGeometry();

   // x coordinate out of monitor
   if ((sn_NewX + this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().width()) >
       (c_RectMonitor.x() + c_RectMonitor.width()))
   {
      // max of left border of monitor and right border of monitor minus tool-tip-width
      sn_NewX = std::max(c_RectMonitor.x() + sn_DistanceToBoarder,
                         (c_RectMonitor.x() + c_RectMonitor.width()) -
                         (this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().width() + sn_DistanceToBoarder));
   }

   // y coordinate out of monitor
   if ((sn_NewY + this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().height()) >
       (c_RectMonitor.y() + c_RectMonitor.height()))
   {
      // max of bottom border of monitor and bottom border of monitor minus tool-tip-height
      sn_NewY = std::max(c_RectMonitor.y() + sn_DistanceToBoarder,
                         (c_RectMonitor.y() + c_RectMonitor.height()) -
                         (this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().height() + sn_DistanceToBoarder));
   }

   this->move(QPoint(sn_NewX, sn_NewY));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Hide all existing tool tips

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_NagToolTip::h_HideAll(void)
{
   bool q_Retval = false;

   for (std::vector<QPointer<C_NagToolTip> >::iterator c_ItExisting = C_NagToolTip::mhc_ExistingToolTips.begin();
        c_ItExisting != C_NagToolTip::mhc_ExistingToolTips.end();)
   {
      QPointer<C_NagToolTip> & rc_ExistingToolTip = *c_ItExisting;
      if (rc_ExistingToolTip.isNull() == false)
      {
         if (rc_ExistingToolTip->isVisible() == true)
         {
            q_Retval = true;
         }
         rc_ExistingToolTip->hide();
         //Important iterator step
         ++c_ItExisting;
      }
      else
      {
         //Clean up
         c_ItExisting = C_NagToolTip::mhc_ExistingToolTips.erase(c_ItExisting);
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Handle all existing tooltips if necessary

   \param[in,out] opc_Event Event identification and information

   \created     04.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::showEvent(QShowEvent * const opc_Event)
{
   bool q_Found = false;

   //Clean up old ones
   for (std::vector<QPointer<C_NagToolTip> >::iterator c_ItExisting = C_NagToolTip::mhc_ExistingToolTips.begin();
        c_ItExisting != C_NagToolTip::mhc_ExistingToolTips.end();)
   {
      QPointer<C_NagToolTip> & rc_ExistingToolTip = *c_ItExisting;
      if (rc_ExistingToolTip.isNull() == false)
      {
         //Hide ourself in a show event seems to be something we want to avoid
         if (rc_ExistingToolTip.data() != this)
         {
            rc_ExistingToolTip->hide();
         }
         else
         {
            q_Found = true;
         }
         //Important iterator step
         ++c_ItExisting;
      }
      else
      {
         //Clean up
         c_ItExisting = C_NagToolTip::mhc_ExistingToolTips.erase(c_ItExisting);
      }
   }
   //Register new one
   if (q_Found == false)
   {
      C_NagToolTip::mhc_ExistingToolTips.push_back(this);
   }
   //Show new one
   QWidget::showEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten leave event slot

   Here: Hide tool-tip

   \param[in,out] opc_Event Event identification and information

   \created     04.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolTip::leaveEvent(QEvent * const opc_Event)
{
   QWidget::leaveEvent(opc_Event);
   //At this point the tool-tip should not be displayed anymore
   this->hide();
}
