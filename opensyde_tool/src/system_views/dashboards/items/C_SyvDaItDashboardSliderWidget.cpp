//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard slider widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QtGlobal>

#include "stwtypes.hpp"
#include "C_SyvDaItUtil.hpp"
#include "C_SyvDaItDashboardSliderWidget.hpp"
#include "ui_C_SyvDaItDashboardSliderWidget.h"
#include "C_OgeLabDashboardDefault.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
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

   connect(this->mpc_Ui->pc_HorizontalSlider, &stw::opensyde_gui_elements::C_OgeSliDashboard::valueChanged, this,
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

   \param[in]  oe_Style    Widget style
   \param[in]  oq_IsDark   Dark mode active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetCurrentStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style,
                                                     const bool oq_IsDark) const
{
   this->mpc_Ui->pc_HorizontalSlider->SetCurrentStyle(oe_Style, oq_IsDark);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Type  New item type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type)
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
void C_SyvDaItDashboardSliderWidget::AdjustFontToSize(void)
{
   // Constants
   const int32_t s32_WidgetHeight = this->height();
   const float32_t f32_FONT_MODIFIER = 0.3F;

   const float32_t f32_MaxLabelFontSize = static_cast<float32_t>(s32_WidgetHeight) * f32_FONT_MODIFIER;
   const float32_t f32_MinLabelFontSize = static_cast<float32_t>(s32_WidgetHeight) * f32_FONT_MODIFIER;

   const uint32_t u32_MaxLabelFontSize = static_cast<uint32_t>(f32_MaxLabelFontSize);
   const uint32_t u32_MinLabelFontSize = static_cast<uint32_t>(f32_MinLabelFontSize);

   m_SetLabelSize(this->mpc_Ui->pc_LabelMin, u32_MaxLabelFontSize);
   m_SetLabelSize(this->mpc_Ui->pc_LabelMax, u32_MinLabelFontSize);

   //Manual resize
   this->mpc_Ui->pc_HorizontalSlider->setFixedHeight(this->height());
   this->mpc_Ui->pc_HorizontalSlider->HandleResize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum and maximum

   \param[in]  os32_MinValue  New minimum
   \param[in]  orc_MinString  New displayed minimum
   \param[in]  os32_MaxValue  New maximum
   \param[in]  orc_MaxString  New displayed maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetMinMax(const int32_t os32_MinValue, const QString & orc_MinString,
                                               const int32_t os32_MaxValue, const QString & orc_MaxString)
{
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 2)
#warning Check if this bug fix is still necessary in new Qt version
#endif
   // Bug fix for qt issue in QStyle::sliderPositionFromValue (https://bugreports.qt.io/browse/QTBUG-29764)
   // Can be reproduced in any call of:
   // QStyle::sliderPositionFromValue(std::numeric_limits<int>::min(), 0, 0, 100,false);
   // leads to crashes in C_OgeSliDashboard::paintEvent and
   // in C_OgeSliToolTipBase::m_MoveToolTip (Systemtest Project View "Bus0" Tab "Single" and Tab "WIN-PC2")
   const int64_t s64_Range = static_cast<int64_t>(os32_MaxValue) - static_cast<int64_t>(os32_MinValue);

   if (s64_Range == std::abs(static_cast<int64_t>(std::numeric_limits<int32_t>::min())))
   {
      this->mpc_Ui->pc_HorizontalSlider->setMinimum(os32_MinValue + 1);
   }
   else
   {
      this->mpc_Ui->pc_HorizontalSlider->setMinimum(os32_MinValue);
   }
   this->mc_Min = orc_MinString;
   this->mpc_Ui->pc_HorizontalSlider->setMaximum(os32_MaxValue);
   this->mc_Max = orc_MaxString;

   this->m_UpdateLabels();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the flag for showing minimum and maximum

   \param[in]  oq_Value    Flag for showing minimum and maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetShowMinMax(const bool oq_Value)
{
   this->mq_ShowMinMax = oq_Value;

   this->m_UpdateLabels();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   \param[in]  os32_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetValue(const int32_t os32_Value) const
{
   this->mpc_Ui->pc_HorizontalSlider->SetValueCustom(os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get min value

   \return
   Min value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItDashboardSliderWidget::GetMinValue() const
{
   return this->mpc_Ui->pc_HorizontalSlider->minimum();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value

   \return
   Actual value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItDashboardSliderWidget::GetValue(void) const
{
   return this->mpc_Ui->pc_HorizontalSlider->value();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set parameter for tooltip display format

   Formula: (((Value - slider_min) * of64_ToolTipSliderRangeValueFactor) + of64_ToolTipValueOffset)
            then apply orc_ToolTipUserScaling

   \param[in]  of64_ToolTipSliderRangeValueFactor  See formula
   \param[in]  of64_ToolTipValueOffset             See formula
   \param[in]  orc_ToolTipUserScaling              See formula
   \param[in]  oe_RepresentationType               Representation type
   \param[in]  orc_FormatterConfig                 Formatter config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::SetToolTipParameters(const float64_t of64_ToolTipSliderRangeValueFactor,
                                                          const float64_t of64_ToolTipValueOffset,
                                                          const C_PuiSvDbDataElementScaling & orc_ToolTipUserScaling,
                                                          const C_OscNodeDataPoolContent::E_Type oe_RepresentationType,
                                                          const C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig)
const
{
   this->mpc_Ui->pc_HorizontalSlider->SetToolTipParameters(of64_ToolTipSliderRangeValueFactor,
                                                           of64_ToolTipValueOffset,
                                                           orc_ToolTipUserScaling, oe_RepresentationType,
                                                           orc_FormatterConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Handle resize specific update

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   AdjustFontToSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the pixel size of the label.

   This also includes setting the width of the label and the height

   \param[in]   opc_Label              Label that will be modified
   \param[in]   oru32_LabelFontSize    Label font size
   \param[in]   ou32_MinimumFontSize   Minimum font size (default value is 15)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::m_SetLabelSize(opensyde_gui_elements::C_OgeLabDashboardDefault * const opc_Label,
                                                    const uint32_t & oru32_LabelFontSize,
                                                    const uint32_t ou32_MinimumFontSize)
{
   // Constants
   const QString c_Text = opc_Label->text();

   // Font variables
   QFont c_Font = opc_Label->font();

   //Check if font size is too small
   const uint32_t u32_LabelFontSize = (ou32_MinimumFontSize >= oru32_LabelFontSize) ?
                                      (ou32_MinimumFontSize) :
                                      (oru32_LabelFontSize);

   // set font size if needed
   if (static_cast<uint32_t>(c_Font.pixelSize()) != u32_LabelFontSize)
   {
      m_SetLabelFontSize(opc_Label, u32_LabelFontSize, c_Font, c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set label font size

   \param[in]       opc_Label              Label that will be modified
   \param[in]       oru32_LabelFontSize      New label font size
   \param[in]       orc_Font                 Font to set the QFontMetrics and with that set the font size
   \param[in]       orc_Text                 Text that is necessary to calculate the font size by width and height of
                                             the text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItDashboardSliderWidget::m_SetLabelFontSize(
   opensyde_gui_elements::C_OgeLabDashboardDefault * const opc_Label, const uint32_t & oru32_LabelFontSize,
   QFont & orc_Font, const QString & orc_Text)
{
   // Constants
   const int32_t s32_WidgetHeight = this->height();

   // Font variables
   uint32_t u32_LabelWidth;

   const QFontMetrics c_TextFontMetric = m_GetNewLabelFontMetrics(opc_Label, orc_Font, oru32_LabelFontSize);

   // set label width
   u32_LabelWidth = c_TextFontMetric.horizontalAdvance(orc_Text);

   // set label pixel size
   opc_Label->setFixedSize(static_cast<int32_t>(u32_LabelWidth), s32_WidgetHeight);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the labels according to users preferences in visible min and max value.
*/
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new font metrics

   \param[in]       opc_Label                Label that will be modified
   \param[in]       oru32_LabelFontSize      New label font size
   \param[in]       orc_Font                 Font to set the QFontMetrics and with that set the font size

   \return
   QFontMetrics
*/
//----------------------------------------------------------------------------------------------------------------------
QFontMetrics C_SyvDaItDashboardSliderWidget::m_GetNewLabelFontMetrics(
   stw::opensyde_gui_elements::C_OgeLabDashboardDefault * const opc_Label, QFont & orc_Font,
   const uint32_t & oru32_NewFontSize)
{
   QFontMetrics c_TextFontMetric(orc_Font);

   if (oru32_NewFontSize != static_cast<uint32_t>(orc_Font.pixelSize()))
   {
      orc_Font.setPointSize(oru32_NewFontSize);
      opc_Label->setFont(orc_Font);

      c_TextFontMetric = QFontMetrics(orc_Font);
   }

   return c_TextFontMetric;
}
