//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool tip window with an image

   Tool tip window with an image

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "constants.hpp"

#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QGuiApplication>
#include <QIcon>

#include "C_OgeWiUtil.hpp"
#include "TglUtils.hpp"
#include "C_NagToolTipWithImage.hpp"
#include "ui_C_NagToolTipWithImage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
std::vector<QPointer<C_NagToolTipWithImage> > C_NagToolTipWithImage::mhc_ExistingToolTips;

const uint16_t C_NagToolTipWithImage::mhu16_IMAGE_WIDTH = 150;
const QString C_NagToolTipWithImage::mhc_DEFAULT_IMAGE = "Image_Grey.svg";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]  orc_Heading    Optional heading
   \param[in]  orc_Content    Optional content
   \param[in]  opc_Parent     Optional parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolTipWithImage::C_NagToolTipWithImage(const QString & orc_Heading, const QString & orc_Content,
                                             QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagToolTipWithImage)
{
   this->mpc_Ui->setupUi(this);

   SetHeading(orc_Heading);
   SetContent(orc_Content);
   SetType(eDEFAULT);

   this->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
   this->setAttribute(Qt::WA_TranslucentBackground);

   // Image is disabled by default
   HideImage();

   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_FrameVerticalDivider,
                                                                   "HasColor7Background", true);

   // Text alignment for tooltip content
   this->mpc_Ui->mpc_Content->setAlignment(Qt::AlignLeft | Qt::AlignTop);

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
C_NagToolTipWithImage::~C_NagToolTipWithImage()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new heading

   \param[in]  orc_Heading    New heading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTipWithImage::SetHeading(const QString & orc_Heading) const
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
void C_NagToolTipWithImage::SetContent(const QString & orc_Content) const
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
void C_NagToolTipWithImage::SetType(const C_NagToolTipWithImage::E_Type oe_Type) const
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
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->mpc_LabelCaption, "Type", c_NewType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move widget position

   \param[in]  orc_GlobalPos  Global target position of top left corner
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTipWithImage::DoMove(const QPoint & orc_GlobalPos)
{
   // get available space of monitor where tool-tip was requested (e.g. subtract windows task bar from whole monitor)
   const QScreen * const pc_Screen = QGuiApplication::screenAt(orc_GlobalPos);

   if (pc_Screen != NULL)
   {
      const QRect c_RectMonitor = pc_Screen->availableGeometry();

      int32_t s32_NewVertical = orc_GlobalPos.y();
      int32_t s32_NewHorizontal = orc_GlobalPos.x();
      const int32_t s32_DISTANCE_TO_BORDER = 78; //space between tooltip and border (ensures that the tooltip is
      // displayed exactly next to the right border of the screen)

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
         // which usually means the mouse is inside the tooltip which then will trigger the tooltip to instantly hide
         // Also include the distance to the border so the mouse does not even touch the tooltip
         s32_NewVertical = (orc_GlobalPos.y() - this->mpc_Ui->mpc_GroupBoxInklShadow->sizeHint().height()) -
                           s32_DISTANCE_TO_BORDER;
      }

      this->move(QPoint(s32_NewHorizontal, s32_NewVertical));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide the image part of the tooltip

*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTipWithImage::HideImage() const
{
   this->mpc_Ui->mpc_FrameVerticalDivider->setHidden(true);
   this->mpc_Ui->mpc_LabelImage->setVisible(false);
   this->mpc_Ui->mpc_LabelCaption->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show the image part of the tooltip
 *
 *  TODO: provide image size (mostly width) as parameter. Currently hard coded for Node.
 *
 *  \param[in]  orc_ImagePath  The path to the image that will be displayed
 *  \param[in]  orc_ImageText  The text to be displayed below the image
 *
 *
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolTipWithImage::ShowImage(const QString & orc_ImagePath, const QString & orc_ImageText) const
{
   QPixmap c_TooltipImage;

   // By default the image part of the tooltip not visible. Make all image gui elements visible
   this->mpc_Ui->mpc_LabelCaption->setText(orc_ImageText);
   this->mpc_Ui->mpc_LabelImage->setVisible(true);
   this->mpc_Ui->mpc_LabelCaption->setVisible(true);
   this->mpc_Ui->mpc_FrameVerticalDivider->setHidden(false);

   // If the image does not have any caption associated with it, hide the caption label
   if (orc_ImageText.isEmpty() == true)
   {
      this->mpc_Ui->mpc_LabelCaption->setHidden(false);
   }

   // Scaling is done differently for a device with "no image". In this case we display a gray icon
   // saying "no image available"
   if (orc_ImagePath.contains(mhc_DEFAULT_IMAGE) == true)
   {
      const uint16_t u16_DEFAULT_WIDTH = mhu16_IMAGE_WIDTH - 30;
      const QIcon c_Icon(orc_ImagePath);
      c_TooltipImage = c_Icon.pixmap(u16_DEFAULT_WIDTH, u16_DEFAULT_WIDTH);
   }
   else
   {
      c_TooltipImage.load(orc_ImagePath);

      c_TooltipImage = c_TooltipImage.scaled(mhu16_IMAGE_WIDTH,  c_TooltipImage.height(), //second parameter is not
                                                                                          // relevant
                                             Qt::KeepAspectRatio, Qt::SmoothTransformation);
   }

   this->mpc_Ui->mpc_LabelImage->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
   this->mpc_Ui->mpc_LabelImage->setPixmap(c_TooltipImage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide all existing tool tips

   \retval true   hide at least one tooltip
   \retval false  no existing visible tooltip found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagToolTipWithImage::h_HideAll(void)
{
   bool q_Retval = false;

   for (std::vector<QPointer<C_NagToolTipWithImage> >::iterator c_ItExisting =
           C_NagToolTipWithImage::mhc_ExistingToolTips.begin();
        c_ItExisting != C_NagToolTipWithImage::mhc_ExistingToolTips.end();)
   {
      QPointer<C_NagToolTipWithImage> & rc_ExistingToolTip = *c_ItExisting;
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
         c_ItExisting = C_NagToolTipWithImage::mhc_ExistingToolTips.erase(c_ItExisting);
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
void C_NagToolTipWithImage::showEvent(QShowEvent * const opc_Event)
{
   bool q_Found = false;

   //Clean up old ones
   for (std::vector<QPointer<C_NagToolTipWithImage> >::iterator c_ItExisting =
           C_NagToolTipWithImage::mhc_ExistingToolTips.begin();
        c_ItExisting != C_NagToolTipWithImage::mhc_ExistingToolTips.end();)
   {
      QPointer<C_NagToolTipWithImage> & rc_ExistingToolTip = *c_ItExisting;
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
         c_ItExisting = C_NagToolTipWithImage::mhc_ExistingToolTips.erase(c_ItExisting);
      }
   }
   //Register new one
   if (q_Found == false)
   {
      C_NagToolTipWithImage::mhc_ExistingToolTips.emplace_back(this);
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
void C_NagToolTipWithImage::leaveEvent(QEvent * const opc_Event)
{
   QWidget::leaveEvent(opc_Event);
   //At this point the tool-tip should not be displayed anymore
   this->hide();
}
