//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard spin box properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_SyvDaPeSpinBox.h"
#include "ui_C_SyvDaPeSpinBox.h"

#include "C_GtGetText.h"
#include "C_GiSvDaSpinBoxBase.h"
#include "C_OgeCbxText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SyvDaPeSpinBox::mhsn_INDEX_STYLE_TYPE1 = 0;
const sintn C_SyvDaPeSpinBox::mhsn_INDEX_STYLE_TYPE2 = 1;

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
C_SyvDaPeSpinBox::C_SyvDaPeSpinBox(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeSpinBox),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_UpdatePreview();

   InitStaticNames();

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxUnit->setText("");

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeSpinBox::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxUnit, &QCheckBox::toggled, this, &C_SyvDaPeSpinBox::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeSpinBox::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeSpinBox::~C_SyvDaPeSpinBox(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSpinBox::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelShowUnit->setText(C_GtGetText::h_GetText("Show Unit"));
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 2"));

   //Tool tips
   this->mpc_Ui->pc_LabelStyle->SetToolTipInformation(C_GtGetText::h_GetText("Style"),
                                                      C_GtGetText::h_GetText(
                                                         "Different options for styles specific to this widget"));
   this->mpc_Ui->pc_LabelShowUnit->SetToolTipInformation(C_GtGetText::h_GetText("Show Unit"),
                                                         C_GtGetText::h_GetText(
                                                            "Option to display the unit after the value or hide the unit entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show unit flag

   \return
   Current show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeSpinBox::GetShowUnit(void) const
{
   return this->mpc_Ui->pc_CheckBoxUnit->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show unit flag

   \param[in]  oq_Value    New show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSpinBox::SetShowUnit(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxUnit->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbSpinBox::E_Type C_SyvDaPeSpinBox::GetType(void) const
{
   C_PuiSvDbSpinBox::E_Type e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeSpinBox::mhsn_INDEX_STYLE_TYPE2:
      e_Retval = C_PuiSvDbSpinBox::eTYPE2;
      break;
   default:
      e_Retval = C_PuiSvDbSpinBox::eTYPE1;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSpinBox::SetType(const C_PuiSvDbSpinBox::E_Type oe_Type) const
{
   switch (oe_Type)
   {
   case C_PuiSvDbSpinBox::eTYPE2:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeSpinBox::mhsn_INDEX_STYLE_TYPE2);
      break;
   default:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeSpinBox::mhsn_INDEX_STYLE_TYPE1);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeSpinBox::m_UpdatePreview(void)
{
   //Also include the fix offset to the right
   const float64 f64_IconOffset = C_SyvDaPeBase::h_GetFixIconOffset();
   const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
   C_GiSvDaSpinBoxBase * pc_Item = new C_GiSvDaSpinBoxBase(0UL, 0UL, -1L, 0ULL, NULL);
   const QSizeF c_ItemSize(static_cast<float64>(c_ViewSize.width()) / 1.5,
                           static_cast<float64>(c_ViewSize.height()) / 3.0);
   // 10 =  Vertical center adjustment
   const QPointF c_ItemPos(
      ((static_cast<float64>(c_ViewSize.width()) - c_ItemSize.width()) / 2.0) + (f64_IconOffset / 2.0),
      ((static_cast<float64>(c_ViewSize.height()) - c_ItemSize.height()) / 2.0) - 10.0);

   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetType(), this->GetShowUnit());
   pc_Item->SetUnitPe("°C");
   pc_Item->SetValuePe(25);

   pc_Item->ApplySizeChange(c_ItemPos, c_ItemSize);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();

   //lint -e{429}  no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management
}
