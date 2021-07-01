//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard slider properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "TGLUtils.h"
#include "C_SyvDaPeSlider.h"
#include "ui_C_SyvDaPeSlider.h"

#include "C_GiSvDaSliderBase.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE1 = 0;
const sintn C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Parent widget
   \param[in]      oq_DarkMode   Flag for dark mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeSlider::C_SyvDaPeSlider(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeSlider),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_UpdatePreview();

   InitStaticNames();

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxMinMax->setText("");

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTypeColor,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeSlider::m_UpdatePreview);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTypeHandle,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeSlider::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxMinMax, &QCheckBox::toggled, this, &C_SyvDaPeSlider::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeSlider::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeSlider::~C_SyvDaPeSlider(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSlider::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelShowMinMax->setText(C_GtGetText::h_GetText("Show Min / Max"));
   this->mpc_Ui->pc_LabelStyleColor->setText(C_GtGetText::h_GetText("Handle Style"));
   this->mpc_Ui->pc_LabelStyleHandle->setText(C_GtGetText::h_GetText("Line Style"));

   this->mpc_Ui->pc_ComboBoxTypeColor->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxTypeColor->addItem(C_GtGetText::h_GetText("Type 2"));
   this->mpc_Ui->pc_ComboBoxTypeHandle->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxTypeHandle->addItem(C_GtGetText::h_GetText("Type 2"));

   //Tool tips
   this->mpc_Ui->pc_LabelStyleColor->SetToolTipInformation(C_GtGetText::h_GetText("Line Style"),
                                                           C_GtGetText::h_GetText(
                                                              "Different options for styles of the slider line"));
   this->mpc_Ui->pc_LabelStyleHandle->SetToolTipInformation(C_GtGetText::h_GetText("Handle Style"),
                                                            C_GtGetText::h_GetText(
                                                               "Different options for styles of the slider handle"));
   this->mpc_Ui->pc_LabelShowMinMax->SetToolTipInformation(C_GtGetText::h_GetText("Show Min / Max"),
                                                           C_GtGetText::h_GetText(
                                                              "Option to display the minimum and maximum value next to the slider or hide them entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show minimum and maximum flag

   \return
   Current show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeSlider::GetShowMinMax(void) const
{
   return this->mpc_Ui->pc_CheckBoxMinMax->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show minimum and maximum flag

   \param[in]  oq_Value    New show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSlider::SetShowMinMax(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxMinMax->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbSlider::E_Type C_SyvDaPeSlider::GetType(void) const
{
   C_PuiSvDbSlider::E_Type e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxTypeColor->currentIndex())
   {
   case C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2:
      switch (this->mpc_Ui->pc_ComboBoxTypeHandle->currentIndex())
      {
      case C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2:
         e_Retval = C_PuiSvDbSlider::eTYPE_BIG_COLOR_2;
         break;
      default:
         e_Retval = C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2;
         break;
      }
      break;
   default:
      switch (this->mpc_Ui->pc_ComboBoxTypeHandle->currentIndex())
      {
      case C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2:
         e_Retval = C_PuiSvDbSlider::eTYPE_BIG_COLOR_1;
         break;
      default:
         e_Retval = C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1;
         break;
      }
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSlider::SetType(const C_PuiSvDbSlider::E_Type oe_Type) const
{
   switch (oe_Type)
   {
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1:
      this->mpc_Ui->pc_ComboBoxTypeColor->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE1);
      this->mpc_Ui->pc_ComboBoxTypeHandle->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE1);
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_1:
      this->mpc_Ui->pc_ComboBoxTypeColor->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE1);
      this->mpc_Ui->pc_ComboBoxTypeHandle->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2);
      break;
   case C_PuiSvDbSlider::eTYPE_BIG_COLOR_2:
      this->mpc_Ui->pc_ComboBoxTypeColor->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2);
      this->mpc_Ui->pc_ComboBoxTypeHandle->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2);
      break;
   case C_PuiSvDbSlider::eTYPE_SMALL_COLOR_2:
      this->mpc_Ui->pc_ComboBoxTypeColor->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE2);
      this->mpc_Ui->pc_ComboBoxTypeHandle->setCurrentIndex(C_SyvDaPeSlider::mhsn_INDEX_STYLE_TYPE1);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSlider::m_UpdatePreview(void)
{
   const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
   //Also include the fix offset to the right
   const float64 f64_IconOffset = C_SyvDaPeBase::h_GetFixIconOffset();
   C_GiSvDaSliderBase * const pc_Item = new C_GiSvDaSliderBase(0UL, 0UL, -1L, 0ULL, NULL);
   //Do not use view center
   const QSizeF c_ItemSize((static_cast<float64>(c_ViewSize.width()) / 1.2) + f64_IconOffset,
                           static_cast<float64>(c_ViewSize.height()) / 5.0);
   const QPointF c_ItemPos(
      ((static_cast<float64>(c_ViewSize.width()) - c_ItemSize.width()) / 2.0) + (f64_IconOffset / 2.0),
      (static_cast<float64>(c_ViewSize.height()) - c_ItemSize.height()) / 2.0);

   pc_Item->ApplySizeChange(c_ItemPos, c_ItemSize);
   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetType(), this->GetShowMinMax());
   pc_Item->SetValuePe(75, 0, 100);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management
