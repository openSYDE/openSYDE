//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard pie chart properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"

#include "C_SyvDaPePieChart.hpp"
#include "ui_C_SyvDaPePieChart.h"

#include "C_GtGetText.hpp"
#include "C_GiSvDaPieChartBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_SyvDaPePieChart::C_SyvDaPePieChart(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPePieChart),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_UpdatePreview();

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxUnit->setText("");
   this->mpc_Ui->pc_CheckBoxValue->setText("");

   this->InitStaticNames();

   connect(this->mpc_Ui->pc_CheckBoxUnit, &QCheckBox::toggled, this, &C_SyvDaPePieChart::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxValue, &QCheckBox::toggled, this, &C_SyvDaPePieChart::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPePieChart::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPePieChart::~C_SyvDaPePieChart(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPePieChart::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelShowUnit->setText(C_GtGetText::h_GetText("Show Unit"));
   this->mpc_Ui->pc_LabelShowValue->setText(C_GtGetText::h_GetText("Show Value"));

   //Tool tips
   this->mpc_Ui->pc_LabelShowUnit->SetToolTipInformation(C_GtGetText::h_GetText("Show Unit"),
                                                         C_GtGetText::h_GetText(
                                                            "Option to display the unit after the value or hide the unit entirely"));
   this->mpc_Ui->pc_LabelShowValue->SetToolTipInformation(C_GtGetText::h_GetText("Show Value"),
                                                          C_GtGetText::h_GetText(
                                                             "Option to display the value inside the pie chart or hide the value entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show value flag

   \return
   Current show value flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPePieChart::GetShowValue(void) const
{
   return this->mpc_Ui->pc_CheckBoxValue->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show unit flag

   \return
   Current show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPePieChart::GetShowUnit(void) const
{
   return this->mpc_Ui->pc_CheckBoxUnit->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show valid flag

   \param[in]  oq_Value    New show value flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPePieChart::SetShowValue(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxValue->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show unit flag

   \param[in]  oq_Value    New show unit flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPePieChart::SetShowUnit(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxUnit->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPePieChart::m_UpdatePreview(void)
{
   const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
   C_GiSvDaPieChartBase * const pc_Item = new C_GiSvDaPieChartBase(0UL, 0UL, -1L, 0ULL, NULL);
   const QSizeF c_ItemSize(static_cast<float64_t>(c_ViewSize.height()),
                           static_cast<float64_t>(c_ViewSize.height()));
   //20.0 to center the chart
   const QPointF c_ItemPos(
      ((static_cast<float64_t>(c_ViewSize.width()) - static_cast<float64_t>(c_ItemSize.width())) / 2.0) + 20.0,
      (static_cast<float64_t>(c_ViewSize.height()) - static_cast<float64_t>(c_ItemSize.height())) / 2.0);

   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetShowUnit(), this->GetShowValue());
   pc_Item->SetUnitPe("%");
   pc_Item->SetValuePe("75", 1500000);

   pc_Item->ApplySizeChange(c_ItemPos, c_ItemSize);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management
