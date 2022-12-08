//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for boundary style setup to customize text element optics

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFontDialog>

#include "C_GiSyTextElementWidget.hpp"
#include "ui_C_GiSyTextElementWidget.h"

#include "C_GtGetText.hpp"
#include "C_OgePubColorOnly.hpp"
#include "C_UtiStyleSheets.hpp"
#include "C_GiBiTextElement.hpp"
#include "C_SdTopologyScene.hpp"
#include "C_UtiStyleSheets.hpp"
#include "C_GiCustomFunctions.hpp"
#include "C_Uti.hpp"
#include "C_GiSyColorSelectWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString mc_PATH_BACKGROUND_IMG = ":/images/graphic_items/TransparentBtnBackground.png";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent  Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyTextElementWidget::C_GiSyTextElementWidget(C_GiSyBaseWidget & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_GiSyTextElementWidget),
   mpc_ParentDialog(&orc_Parent)
{
   QPixmap c_Pic;

   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   c_Pic.load(mc_PATH_BACKGROUND_IMG);

   mpc_Ui->setupUi(this);

   //font color

   this->mpc_LabelColorFont = new QLabel(dynamic_cast<QWidget *>(this->mpc_Ui->pc_BushButtonFontColor->parent()));
   this->mpc_LabelColorFont->setPixmap(c_Pic);
   this->mpc_LabelColorFont->lower();

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   this->mc_FontColor = C_UtiStyleSheets::h_GetStyleSheetColor(this->mpc_Ui->pc_BushButtonFontColor->styleSheet());

   this->m_UpdatePreview();

   InitStaticNames();

   connect(this->mpc_Ui->pc_BushButtonFontColor, &C_OgePubColorOnly::clicked,
           this, &C_GiSyTextElementWidget::m_FontColorClicked);

   connect(this->mpc_Ui->pc_WidgetFont, &C_OgeWiFontConfig::SigFontUpdate, this,
           &C_GiSyTextElementWidget::m_FontStyleUpdated);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because the ownership of these objects was never transfered to this class
// and Qt management takes care of the rest
C_GiSyTextElementWidget::~C_GiSyTextElementWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelFontColor->setText(C_GtGetText::h_GetText("Font Color"));
   this->mpc_Ui->pc_LabelFontStyle->setText(C_GtGetText::h_GetText("Font"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On Show Event

   \param[in,out]    opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::showEvent(QShowEvent * const opc_Event)
{
   //font color
   this->mpc_LabelColorFont->resize(this->mpc_Ui->pc_BushButtonFontColor->size());
   this->mpc_LabelColorFont->move(this->mpc_Ui->pc_BushButtonFontColor->pos());

   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get actual font style

   \return  Actual font style
*/
//----------------------------------------------------------------------------------------------------------------------
QFont C_GiSyTextElementWidget::GetFontStyle(void) const
{
   return this->mc_FontStyle;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set actual font style

   \param[in]  orc_Font    New font style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::SetFontStyle(const QFont & orc_Font)
{
   this->mc_FontStyle = orc_Font;
   // update the labels
   this->mpc_Ui->pc_WidgetFont->SetFont(this->mc_FontStyle);
   this->m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get actual color of line

   \return  Actual line color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyTextElementWidget::GetFontColor(void) const
{
   return this->mc_FontColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set line color

   \param[in]  orc_Value   New line color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::SetFontColor(const QColor & orc_Value)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonFontColor->styleSheet();

   this->mc_FontColor = orc_Value;

   // set the color of the button
   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, orc_Value);
   this->mpc_Ui->pc_BushButtonFontColor->setStyleSheet(c_Style);
   this->m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::m_UpdatePreview(void)
{
   const QSize c_ViewSize = C_GiSyBaseWidget::h_GetSceneViewSize();
   C_GiBiTextElement * const pc_Item = new C_GiBiTextElement(0ULL);
   QSizeF c_ItemSize;
   bool q_SizeInvalid = true;
   QFont c_Tmp = this->mc_FontStyle;
   int32_t s32_CurPixelSize = this->mc_FontStyle.pixelSize();

   if (s32_CurPixelSize <= 0)
   {
      s32_CurPixelSize = this->mc_FontStyle.pointSize();
   }

   //Apply color
   pc_Item->SetFontColor(this->mc_FontColor);
   //dynamic adaptation of point size as there are a wide variety of font families with vast differences in text length
   for (int32_t s32_PixelSize = s32_CurPixelSize; (s32_PixelSize > 1) && (q_SizeInvalid == true);
        --s32_PixelSize)
   {
      //Apply current point size
      c_Tmp.setPixelSize(s32_PixelSize);
      //If someone wonders, this line somehow triggers a qt exceptoin
      pc_Item->SetFontStyle(c_Tmp);
      //Set fix text each time as this will trigger a size adaptation
      pc_Item->SetText("Text Element");

      c_ItemSize = pc_Item->GetSize();

      //Set centered position
      pc_Item->setPos((static_cast<float64_t>(c_ViewSize.width()) - c_ItemSize.width()) / 2.0,
                      (static_cast<float64_t>(c_ViewSize.height()) - c_ItemSize.height()) / 2.0);
      //Check item size valid
      if ((static_cast<float64_t>(pc_Item->boundingRect().width()) +
           (2.0 * C_GiCustomFunctions::hf64_SCENE_MIN_BORDER_SIZE)) > static_cast<float64_t>(c_ViewSize.width()))
      {
         q_SizeInvalid = true;
      }
      else
      {
         q_SizeInvalid = false;
      }
   }

   // clear old scene
   this->mpc_ParentDialog->GetPreviewScene()->clear();

   // add item
   this->mpc_ParentDialog->GetPreviewScene()->addItem(pc_Item);
   this->mpc_ParentDialog->GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of adding the item to the scene and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of inner color button

   Offers the possibility to choose a color for the inner line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::m_FontColorClicked(void)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonFontColor->styleSheet();
   // get the old color as initial color
   QColor c_Color = C_UtiStyleSheets::h_GetStyleSheetColor(c_Style);

   const QPointer<C_OgePopUpDialog> c_Popup = new C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, c_Color);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   if (c_Popup->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      c_Color = pc_ColorWidget->ChooseSelectedColor();

      if (c_Color.isValid() == true)
      {
         // save the color
         this->mc_FontColor = c_Color;

         // update the button
         C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, c_Color);
         this->mpc_Ui->pc_BushButtonFontColor->setStyleSheet(c_Style);

         this->m_UpdatePreview();
      }
   }

   if (c_Popup != NULL)
   {
      c_Popup->HideOverlay();
      c_Popup->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle font change

   \param[in] orc_Font New font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyTextElementWidget::m_FontStyleUpdated(const QFont & orc_Font)
{
   this->SetFontStyle(orc_Font);
}
