//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool tip window (implementation)

   Tool tip window

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScreen>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsDropShadowEffect>
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "constants.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_NagToolTip.hpp"
#include "ui_C_NagToolTip.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
std::vector<QPointer<C_NagToolTip> > C_NagToolTip::mhc_ExistingToolTips;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Heading    Optional heading
   \param[in]  orc_Content    Optional content
   \param[in]  opc_Parent     Optional parent
*/
//----------------------------------------------------------------------------------------------------------------------
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
      pc_Shadow->setOffset(4.0);
      c_Color = mc_STYLE_GUIDE_COLOR_31;
      c_Color.setAlpha(110);
      pc_Shadow->setColor(c_Color);
      this->mpc_Ui->mpc_GroupBox->setGraphicsEffect(pc_Shadow);
   } //lint !e429  //no memory leak because of the parent of pc_Shadow and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolTip::~C_NagToolTip()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new heading

   \param[in]  orc_Heading    New heading
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new content

   \param[in]  orc_Content    New content
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tooltip type

   \param[in]  oe_Type  Tooltip type
*/
//----------------------------------------------------------------------------------------------------------------------
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
   default:
      tgl_assert(false);
      break;
   }
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_GroupBox, "Type", c_NewType);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_Heading, "Type", c_NewType);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_Content, "Type", c_NewType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move widget position

   \param[in]  orc_GlobalPos  Global target position of top left corner
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTip::DoMove(const QPoint & orc_GlobalPos)
{
   // get available space of monitor where tool-tip was requested (e.g. subtract windows task bar from whole monitor)
   const QScreen * const pc_Screen = QGuiApplication::screenAt(orc_GlobalPos);

   if (pc_Screen != NULL)
   {
      const QRect c_RectMonitor = pc_Screen->availableGeometry();

      int32_t s32_NewVertical = orc_GlobalPos.y();
      int32_t s32_NewHorizontal = orc_GlobalPos.x();
      const int32_t s32_DISTANCE_TO_BORDER = 20; //space between tooltip and boarder

      this->updateGeometry();

      // x coordinate out of monitor
      if ((s32_NewHorizontal + this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().width()) >
          (c_RectMonitor.x() + c_RectMonitor.width()))
      {
         // max of left border of monitor and right border of monitor minus tool-tip-width
         s32_NewHorizontal = std::max(c_RectMonitor.x() + s32_DISTANCE_TO_BORDER,
                                      (c_RectMonitor.x() + c_RectMonitor.width()) -
                                      (this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().width() +
                                       s32_DISTANCE_TO_BORDER));
      }

      // y coordinate out of monitor
      if ((s32_NewVertical + this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().height()) >
          (c_RectMonitor.y() + c_RectMonitor.height()))
      {
         //display tooltip above global pos (otheriwse the global pos is inside the tooltip
         // which usally means the mouse is inside the tooltip which then will trigger the tooltip to instantly hide
         // Also include the distance to the border so the mouse does not even touch the tooltip
         s32_NewVertical = (orc_GlobalPos.y() - this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().height()) -
                           s32_DISTANCE_TO_BORDER;
      }

      this->move(QPoint(s32_NewHorizontal, s32_NewVertical));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide all existing tool tips

   \retval true   hided at least one tooltip
   \retval false  no existing visible tooltip found
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Handle all existing tooltips if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
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
      C_NagToolTip::mhc_ExistingToolTips.emplace_back(this);
   }
   //Show new one
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten leave event slot

   Here: Hide tool-tip

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTip::leaveEvent(QEvent * const opc_Event)
{
   QWidget::leaveEvent(opc_Event);
   //At this point the tool-tip should not be displayed anymore
   this->hide();
}
