//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for boundary style setup to customize boundary optics

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GiSyBoundaryWidget.h"
#include "ui_C_GiSyBoundaryWidget.h"

#include "C_HeHandler.h"
#include "C_GtGetText.h"
#include "C_OgePubColorOnly.h"
#include "C_UtiStyleSheets.h"
#include "C_GiBiBoundary.h"
#include "C_SdTopologyScene.h"
#include "C_GiSyColorSelectWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

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
C_GiSyBoundaryWidget::C_GiSyBoundaryWidget(C_GiSyBaseWidget & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_GiSyBoundaryWidget),
   mpc_ParentDialog(&orc_Parent)
{
   QPixmap c_Pic;

   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   c_Pic.load(mc_PATH_BACKGROUND_IMG);

   mpc_Ui->setupUi(this);

   //border
   this->mpc_LabelBorder = new QLabel(dynamic_cast<QWidget *>(this->mpc_Ui->pc_BushButtonBorderColor->parent()));
   this->mpc_LabelBorder->setPixmap(c_Pic);
   this->mpc_LabelBorder->lower();

   //background
   this->mpc_LabelBackground =
      new QLabel(dynamic_cast<QWidget *>(this->mpc_Ui->pc_BushButtonBackgroundColor->parent()));
   this->mpc_LabelBackground->setPixmap(c_Pic);
   this->mpc_LabelBackground->lower();

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxWidth->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxWidth->SetMaximumCustom(20);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   this->mc_BorderColor = C_UtiStyleSheets::h_GetStyleSheetColor(
      this->mpc_Ui->pc_BushButtonBorderColor->styleSheet());
   this->mc_BackgroundColor = C_UtiStyleSheets::h_GetStyleSheetColor(
      this->mpc_Ui->pc_BushButtonBackgroundColor->styleSheet());

   this->m_UpdatePreview();

   InitStaticNames();

   connect(this->mpc_Ui->pc_BushButtonBorderColor, &C_OgePubColorOnly::clicked, this,
           &C_GiSyBoundaryWidget::m_BorderColorClicked);
   connect(this->mpc_Ui->pc_BushButtonBackgroundColor, &C_OgePubColorOnly::clicked,
           this, &C_GiSyBoundaryWidget::m_BackgroundColorClicked);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxWidth, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged),
           this, &C_GiSyBoundaryWidget::m_WidthChanged);
} //lint !e429  //no memory leak because of the parent of pc_Label-Background / Border and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because the ownership of these objects was never transfered to this class
// and Qt management takes care of the rest
C_GiSyBoundaryWidget::~C_GiSyBoundaryWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelWidth->setText(C_GtGetText::h_GetText("Border Width"));
   this->mpc_Ui->pc_LabelBorderColor->setText(C_GtGetText::h_GetText("Border Color"));
   this->mpc_Ui->pc_LabelBackgroundColor->setText(C_GtGetText::h_GetText("Background Color"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On Show Event

   \param[in,out]    opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::showEvent(QShowEvent * const opc_Event)
{
   //border
   this->mpc_LabelBorder->resize(this->mpc_Ui->pc_BushButtonBorderColor->size());
   this->mpc_LabelBorder->move(this->mpc_Ui->pc_BushButtonBorderColor->pos());

   //background
   this->mpc_LabelBackground->resize(this->mpc_Ui->pc_BushButtonBackgroundColor->size());
   this->mpc_LabelBackground->move(this->mpc_Ui->pc_BushButtonBackgroundColor->pos());

   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get actual line width

   \return  Actual line width
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sintn C_GiSyBoundaryWidget::GetBorderWidth(void) const
{
   return this->mpc_Ui->pc_SpinBoxWidth->value();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set line width

   \param[in]  osn_Value   New line width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::SetBorderWidth(const stw_types::sintn osn_Value) const
{
   this->mpc_Ui->pc_SpinBoxWidth->setValue(osn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get actual color of line

   \return  Actual line color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyBoundaryWidget::GetBorderColor(void) const
{
   return this->mc_BorderColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set line color

   \param[in]  orc_Value   New line color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::SetBorderColor(const QColor & orc_Value)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonBorderColor->styleSheet();

   this->mc_BorderColor = orc_Value;

   // set the color of the button
   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, orc_Value);
   this->mpc_Ui->pc_BushButtonBorderColor->setStyleSheet(c_Style);
   this->m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   GetInnerlineColor

   \return  Actual color of inner line
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyBoundaryWidget::GetBackgroundColor(void) const
{
   return this->mc_BackgroundColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set inner line color

   \param[in]  orc_Value   New inner line color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::SetBackgroundColor(const QColor & orc_Value)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonBackgroundColor->styleSheet();

   this->mc_BackgroundColor = orc_Value;

   // set the color of the button
   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, orc_Value);
   this->mpc_Ui->pc_BushButtonBackgroundColor->setStyleSheet(c_Style);
   m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::m_UpdatePreview(void)
{
   const QSize c_ViewSize = C_GiSyBaseWidget::h_GetSceneViewSize();
   C_GiBiBoundary * const pc_Item = new C_GiBiBoundary(0ULL, 200.0, 70.0);
   const QSizeF c_ItemSize = pc_Item->GetSize();

   pc_Item->setPos((static_cast<float64>(c_ViewSize.width()) - c_ItemSize.width()) / 2.0,
                   (static_cast<float64>(c_ViewSize.height()) - c_ItemSize.height()) / 2.0);

   pc_Item->SetBorderColor(this->mc_BorderColor);
   pc_Item->SetBackgroundColor(this->mc_BackgroundColor);
   pc_Item->SetBorderWidth(this->mpc_Ui->pc_SpinBoxWidth->value());

   // clear old scene
   this->mpc_ParentDialog->GetPreviewScene()->clear();

   // add item
   this->mpc_ParentDialog->GetPreviewScene()->addItem(pc_Item);
   this->mpc_ParentDialog->GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Color button

   Offers the possibility to choose a color for the line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::m_BorderColorClicked(void)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonBorderColor->styleSheet();
   // get the old color as initial color
   QColor c_Color = C_UtiStyleSheets::h_GetStyleSheetColor(c_Style);

   QPointer<C_OgePopUpDialog> const c_Popup = new C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, c_Color);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   if (c_Popup->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      c_Color = pc_ColorWidget->ChooseSelectedColor();

      if (c_Color.isValid() == true)
      {
         // save the color
         this->mc_BorderColor = c_Color;

         // update the button
         C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, c_Color);
         this->mpc_Ui->pc_BushButtonBorderColor->setStyleSheet(c_Style);

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
/*! \brief   Slot of inner color button

   Offers the possibility to choose a color for the inner line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::m_BackgroundColorClicked(void)
{
   QString c_Style = this->mpc_Ui->pc_BushButtonBackgroundColor->styleSheet();
   // get the old color as initial color
   QColor c_Color = C_UtiStyleSheets::h_GetStyleSheetColor(c_Style);

   QPointer<C_OgePopUpDialog> const c_Popup = new C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, c_Color);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   if (c_Popup->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      c_Color = pc_ColorWidget->ChooseSelectedColor();

      if (c_Color.isValid() == true)
      {
         // save the color
         this->mc_BackgroundColor = c_Color;

         // update the button
         C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, c_Color);
         this->mpc_Ui->pc_BushButtonBackgroundColor->setStyleSheet(c_Style);

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
/*! \brief   Slot of spin box changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBoundaryWidget::m_WidthChanged(void)
{
   this->m_UpdatePreview();
}
