//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard slider widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaItUtil.h"
#include "C_SyvDaItDashboardSliderWidget.h"
#include "ui_C_SyvDaItDashboardSliderWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItDashboardSliderWidget::C_SyvDaItDashboardSliderWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItDashboardSliderWidget),
   mc_Min("0"),
   mc_Max("99"),
   mq_ShowMinMax(true)
{
   mpc_Ui->setupUi(this);

   //Deactivate automated adaption: reason syncing those if each is independent is hard to do
   this->mpc_Ui->pc_LabelMin->SetAllowAutomatedAdaptation(false);
   this->mpc_Ui->pc_LabelMax->SetAllowAutomatedAdaptation(false);

   connect(this->mpc_Ui->pc_HorizontalSlider, &stw_opensyde_gui_elements::C_OgeSliDashboard::valueChanged, this,
           &C_SyvDaItDashboardSliderWidget::SigOnChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItDashboardSliderWidget::~C_SyvDaItDashboardSliderWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current style

   \param[in] oe_Style  Widget style
   \param[in] oq_IsDark Dark mode active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetCurrentStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style,
                                                     const bool oq_IsDark) const
{
   this->mpc_Ui->pc_HorizontalSlider->SetCurrentStyle(oe_Style, oq_IsDark);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style New style type
   \param[in] oe_Type  New item type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type)
const
{
   QFont c_Font;

   this->mpc_Ui->pc_HorizontalSlider->SetDisplayStyle(oe_Type);

   //Min
   this->mpc_Ui->pc_LabelMin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
   c_Font = this->mpc_Ui->pc_LabelMin->font();
   c_Font.setFamily("Segoe UI");
   this->mpc_Ui->pc_LabelMin->setFont(c_Font);

   //Max
   this->mpc_Ui->pc_LabelMax->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
   c_Font = this->mpc_Ui->pc_LabelMax->font();
   c_Font.setFamily("Segoe UI");
   this->mpc_Ui->pc_LabelMax->setFont(c_Font);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::AdjustFontToSize(void) const
{
   const sintn sn_Margins = 2 * 6;
   const sintn sn_AdaptedWidth = this->width() - sn_Margins;
   const float32 f32_WidthLabels = static_cast<float32>(sn_AdaptedWidth) * (2.0F / ((25.0F * 2.0F) / 3.0F));
   const sintn sn_WidthLabels = static_cast<sintn>(f32_WidthLabels);

   //Manual resize
   this->mpc_Ui->pc_HorizontalSlider->setFixedHeight(this->height());
   this->mpc_Ui->pc_LabelMin->setFixedSize(sn_WidthLabels, this->height());
   this->mpc_Ui->pc_LabelMax->setFixedSize(sn_WidthLabels, this->height());

   this->mpc_Ui->pc_HorizontalSlider->HandleResize();
   this->mpc_Ui->pc_LabelMin->AdjustFontToSize();
   this->mpc_Ui->pc_LabelMax->AdjustFontToSize();

   //Sync font
   C_SyvDaItUtil::h_SyncFontSize(this->mpc_Ui->pc_LabelMin, this->mpc_Ui->pc_LabelMax);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set minimum

   \param[in] osn_Value  New minimum
   \param[in] orc_String New displayed minimum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetMin(const sintn osn_Value, const QString & orc_String)
{
   this->mpc_Ui->pc_HorizontalSlider->setMinimum(osn_Value);
   this->mc_Min = orc_String;

   this->m_UpdateLabels();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set maximum

   \param[in] osn_Value  New maximum
   \param[in] orc_String New displayed maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetMax(const sintn osn_Value, const QString & orc_String)
{
   this->mpc_Ui->pc_HorizontalSlider->setMaximum(osn_Value);
   this->mc_Max = orc_String;

   this->m_UpdateLabels();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for showing minimum and maximum

   \param[in]     oq_Value       Flag for showing minimum and maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetShowMinMax(const bool oq_Value)
{
   this->mq_ShowMinMax = oq_Value;

   this->m_UpdateLabels();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetValue(const sintn osn_Value) const
{
   this->mpc_Ui->pc_HorizontalSlider->SetValueCustom(osn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value

   \return
   Actual value
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SyvDaItDashboardSliderWidget::GetValue(void) const
{
   return this->mpc_Ui->pc_HorizontalSlider->value();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameter for tooltip display format

   Formula: ((Value - of64_ToolTipRangeOffset) * of64_ToolTipFactor) + of64_ToolTipOffset

   \param[in] of64_ToolTipRangeOffset See formula
   \param[in] of64_ToolTipOffset      See formula
   \param[in] of64_ToolTipFactor      See formula
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetToolTipParameters(const float64 of64_ToolTipRangeOffset,
                                                          const float64 of64_ToolTipOffset,
                                                          const float64 of64_ToolTipFactor,
                                                          const C_OSCNodeDataPoolContent::E_Type oe_RepresentationType)
const
{
   this->mpc_Ui->pc_HorizontalSlider->SetToolTipParameters(of64_ToolTipRangeOffset, of64_ToolTipOffset,
                                                           of64_ToolTipFactor, oe_RepresentationType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Handle resize specific update

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   AdjustFontToSize();
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvDaItDashboardSliderWidget::m_UpdateLabels(void) const
{
   if (this->mq_ShowMinMax == true)
   {
      this->mpc_Ui->pc_LabelMax->setText(this->mc_Max);
      this->mpc_Ui->pc_LabelMin->setText(this->mc_Min);
   }
   else
   {
      this->mpc_Ui->pc_LabelMax->setText("");
      this->mpc_Ui->pc_LabelMin->setText("");
   }
}
