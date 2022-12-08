//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard progress bar properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"

#include "TglUtils.hpp"
#include "C_SyvDaPeProgressBar.hpp"
#include "ui_C_SyvDaPeProgressBar.h"

#include "C_GiSvDaProgressBarBase.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1 = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2 = 1;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3 = 2;

const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_RIGHT = 1;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_TOP = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM = 1;

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
C_SyvDaPeProgressBar::C_SyvDaPeProgressBar(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeProgressBar),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();

   this->m_TypeChanged();

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxMinMax->setText("");

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeProgressBar::m_TypeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxAlignment,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeProgressBar::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxMinMax, &QCheckBox::toggled, this, &C_SyvDaPeProgressBar::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeProgressBar::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeProgressBar::~C_SyvDaPeProgressBar(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelShowMinMax->setText(C_GtGetText::h_GetText("Show Min / Max"));
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));
   this->mpc_Ui->pc_LabelAlignment->setText(C_GtGetText::h_GetText("Alignment"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 2"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 3"));

   //Tool tips
   this->mpc_Ui->pc_LabelStyle->SetToolTipInformation(C_GtGetText::h_GetText("Style"),
                                                      C_GtGetText::h_GetText(
                                                         "Different options for styles specific to this widget"));
   this->mpc_Ui->pc_LabelAlignment->SetToolTipInformation(C_GtGetText::h_GetText("Alignment"),
                                                          C_GtGetText::h_GetText(
                                                             "Option for different alignments of the value label"));
   this->mpc_Ui->pc_LabelShowMinMax->SetToolTipInformation(C_GtGetText::h_GetText("Show Min / Max"),
                                                           C_GtGetText::h_GetText(
                                                              "Option to display the minimum and maximum value next to the progress bar or hide them entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show minimum and maximum flag

   \return
   Current show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeProgressBar::GetShowMinMax(void) const
{
   return this->mpc_Ui->pc_CheckBoxMinMax->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbProgressBar::E_Type C_SyvDaPeProgressBar::GetType(void) const
{
   C_PuiSvDbProgressBar::E_Type e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_2;
      break;
   case C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_3;
      break;
   default:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_1;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get alignment

   \return
   Current alginment
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbProgressBar::E_Alignment C_SyvDaPeProgressBar::GetAlignment(void) const
{
   C_PuiSvDbProgressBar::E_Alignment e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1:
      switch (this->mpc_Ui->pc_ComboBoxAlignment->currentIndex())
      {
      case C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM:
         e_Retval = C_PuiSvDbProgressBar::eBOTTOM;
         break;
      default:
         e_Retval = C_PuiSvDbProgressBar::eTOP;
         break;
      }
      break;
   default:
      switch (this->mpc_Ui->pc_ComboBoxAlignment->currentIndex())
      {
      case C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT:
         e_Retval = C_PuiSvDbProgressBar::eLEFT;
         break;
      default:
         e_Retval = C_PuiSvDbProgressBar::eRIGHT;
         break;
      }
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show minimum and maximum flag

   \param[in]  oq_Value    New show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetShowMinMax(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxMinMax->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetType(const C_PuiSvDbProgressBar::E_Type oe_Type)
{
   switch (oe_Type)
   {
   case C_PuiSvDbProgressBar::eTYPE_1:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1);
      break;
   case C_PuiSvDbProgressBar::eTYPE_2:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2);
      break;
   case C_PuiSvDbProgressBar::eTYPE_3:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3);
      break;
   default:
      tgl_assert(false);
      break;
   }

   this->m_TypeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set alignment

   \param[in]  oe_Alignment   New alignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetAlignment(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment) const
{
   switch (oe_Alignment)
   {
   case C_PuiSvDbProgressBar::eTOP:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_TOP);
      break;
   case C_PuiSvDbProgressBar::eLEFT:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT);
      break;
   case C_PuiSvDbProgressBar::eRIGHT:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_RIGHT);
      break;
   case C_PuiSvDbProgressBar::eBOTTOM:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_TypeChanged(void)
{
   // Special case: Options for alignment depends of selection of type

   this->m_FillAlignmentComboBox(this->mpc_Ui->pc_ComboBoxType->currentIndex());
   this->m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_FillAlignmentComboBox(const int32_t os32_Type) const
{
   this->mpc_Ui->pc_ComboBoxAlignment->clear();

   if (os32_Type == mhs32_INDEX_STYLE_TYPE1)
   {
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Top"));
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Bottom"));
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Left"));
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Right"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_UpdatePreview(void)
{
   C_GiSvDaProgressBarBase * const pc_Item = new C_GiSvDaProgressBarBase(0UL, 0UL, -1L, 0ULL, NULL);
   const QSizeF c_ITEM_SIZE(240.0, 100.0);
   //Do not use view center
   const QPointF c_ITEM_POS(6.0, 30.0);

   pc_Item->ApplySizeChange(c_ITEM_POS, c_ITEM_SIZE);
   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetType(), this->GetAlignment(), this->GetShowMinMax());
   pc_Item->SetValuePe(50);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management
