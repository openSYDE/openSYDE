//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard toggle properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_SyvDaPeToggle.h"
#include "ui_C_SyvDaPeToggle.h"

#include "C_GtGetText.h"
#include "C_GiSvDaToggleBase.h"
#include "C_PuiSvDbWidgetBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE1 = 0;
const sintn C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE2 = 1;
const sintn C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE3 = 2;

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
C_SyvDaPeToggle::C_SyvDaPeToggle(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeToggle),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_UpdatePreview();

   this->InitStaticNames();

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeToggle::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeToggle::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeToggle::~C_SyvDaPeToggle(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeToggle::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 2"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 3"));

   //Tool tips
   this->mpc_Ui->pc_LabelStyle->SetToolTipInformation(C_GtGetText::h_GetText("Style"),
                                                      C_GtGetText::h_GetText(
                                                         "Different options for styles specific to this widget"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbToggle::E_Type C_SyvDaPeToggle::GetType(void) const
{
   C_PuiSvDbToggle::E_Type e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE2:
      e_Retval = C_PuiSvDbToggle::eTYPE2;
      break;
   case C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE3:
      e_Retval = C_PuiSvDbToggle::eTYPE3;
      break;
   default:
      e_Retval = C_PuiSvDbToggle::eTYPE1;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeToggle::SetType(const C_PuiSvDbToggle::E_Type oe_Type) const
{
   switch (oe_Type)
   {
   case C_PuiSvDbToggle::eTYPE2:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE2);
      break;
   case C_PuiSvDbToggle::eTYPE3:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE3);
      break;
   default:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeToggle::mhsn_INDEX_STYLE_TYPE1);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeToggle::m_UpdatePreview(void)
{
   //Also include the fix offset to the right
   const float64 f64_IconOffset = C_SyvDaPeBase::h_GetFixIconOffset();
   const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
   C_GiSvDaToggleBase * pc_Item = new C_GiSvDaToggleBase(0UL, 0UL, -1L, 0ULL, NULL);
   const QSizeF c_ItemSize(150.0, 100.0);
   // 20 =  Vertical center adjustment
   const QPointF c_ItemPos(
      ((static_cast<float64>(c_ViewSize.width()) - c_ItemSize.width()) / 2.0) + (f64_IconOffset / 2.0),
      ((static_cast<float64>(c_ViewSize.height()) - c_ItemSize.height()) / 2.0) + 20.0);

   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateType(this->GetType());

   pc_Item->ApplySizeChange(c_ItemPos, c_ItemSize);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();

} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management
