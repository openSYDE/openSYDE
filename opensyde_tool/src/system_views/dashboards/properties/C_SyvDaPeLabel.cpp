//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard widget properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_GiSvDaLabelBase.hpp"
#include "C_SyvDaPeLabel.hpp"
#include "ui_C_SyvDaPeLabel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaPeLabel::mhs32_INDEX_STYLE_TYPE_DEFAULT = 0;
const int32_t C_SyvDaPeLabel::mhs32_INDEX_STYLE_TYPE_TRANSPARENT = 1;

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
C_SyvDaPeLabel::C_SyvDaPeLabel(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeLabel),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_UpdatePreview();

   InitStaticNames();

   //Line edit uppercase

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxUnit->setText("");
   this->mpc_Ui->pc_CheckBoxCaption->setText("");

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                         int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeLabel::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxCaption, &QCheckBox::toggled, this, &C_SyvDaPeLabel::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxUnit, &QCheckBox::toggled, this, &C_SyvDaPeLabel::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeLabel::m_UpdatePreview);
   connect(this->mpc_Ui->pc_LineEditCaption, &C_OgeLeProperties::editingFinished, this, &C_SyvDaPeLabel::m_ToUpper);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeLabel::~C_SyvDaPeLabel(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelCaption->setText(C_GtGetText::h_GetText("Caption"));
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));
   this->mpc_Ui->pc_LabelShowUnit->setText(C_GtGetText::h_GetText("Show Unit"));
   this->mpc_Ui->pc_LabelShowCaption->setText(C_GtGetText::h_GetText("Show Caption"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Default"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Transparent"));

   //Tool tips
   this->mpc_Ui->pc_LabelCaption->SetToolTipInformation(C_GtGetText::h_GetText("Caption"),
                                                        C_GtGetText::h_GetText(
                                                           "Caption, displayed above the value"));
   this->mpc_Ui->pc_LabelStyle->SetToolTipInformation(C_GtGetText::h_GetText("Style"),
                                                      C_GtGetText::h_GetText(
                                                         "Different options for styles specific to this widget"));
   this->mpc_Ui->pc_LabelShowUnit->SetToolTipInformation(C_GtGetText::h_GetText("Show Unit"),
                                                         C_GtGetText::h_GetText(
                                                            "Option to display the unit after the value or hide the unit entirely"));
   this->mpc_Ui->pc_LabelShowCaption->SetToolTipInformation(C_GtGetText::h_GetText("Show Caption"),
                                                            C_GtGetText::h_GetText(
                                                               "Option to display the caption above the value or hide the caption entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbLabel::E_Type C_SyvDaPeLabel::GetType(void) const
{
   C_PuiSvDbLabel::E_Type e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeLabel::mhs32_INDEX_STYLE_TYPE_TRANSPARENT:
      e_Retval = C_PuiSvDbLabel::eTRANSPARENT;
      break;
   default:
      e_Retval = C_PuiSvDbLabel::eDEFAULT;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show unit flag

   \return
   Current show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeLabel::GetShowUnit(void) const
{
   return this->mpc_Ui->pc_CheckBoxUnit->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show caption flag

   \return
   Current show caption flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeLabel::GetShowCaption(void) const
{
   return this->mpc_Ui->pc_CheckBoxCaption->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get caption

   \return
   Current caption
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaPeLabel::GetCaption(void) const
{
   return this->mpc_Ui->pc_LineEditCaption->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::SetType(const C_PuiSvDbLabel::E_Type oe_Type) const
{
   switch (oe_Type)
   {
   case C_PuiSvDbLabel::eTRANSPARENT:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeLabel::mhs32_INDEX_STYLE_TYPE_TRANSPARENT);
      break;
   case C_PuiSvDbLabel::eDEFAULT:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeLabel::mhs32_INDEX_STYLE_TYPE_DEFAULT);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show unit flag

   \param[in]  oq_Value    New show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::SetShowUnit(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxUnit->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show caption flag

   \param[in]  oq_Value    New show caption flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::SetShowCaption(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxCaption->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set caption

   \param[in]  orc_Value   New caption
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::SetCaption(const QString & orc_Value) const
{
   this->mpc_Ui->pc_LineEditCaption->setText(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::m_UpdatePreview(void)
{
   C_GiSvDaLabelBase * const pc_Item = new C_GiSvDaLabelBase(0UL, 0UL, -1L, 0ULL, NULL);
   QSizeF c_ItemSize;
   QPointF c_ItemPos;

   C_SyvDaPeBase::h_GetIdealItemRect(c_ItemPos, c_ItemSize, pc_Item);

   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetType(), this->GetShowCaption(), this->GetShowUnit());
   pc_Item->SetCaptionPe("Temperature");
   pc_Item->SetUnitPe("°C");
   pc_Item->SetValuePe("25");

   pc_Item->ApplySizeChange(c_ItemPos, c_ItemSize);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set line edit text always upper case
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeLabel::m_ToUpper(void) const
{
   this->mpc_Ui->pc_LineEditCaption->setText(this->mpc_Ui->pc_LineEditCaption->text().toUpper());
}
