//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Slider with tool tip (implementation)

   Slider with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEvent>
#include <QHelpEvent>
#include <QStyle>
#include "C_OgeSliToolTipBase.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSliToolTipBase::C_OgeSliToolTipBase(QWidget * const opc_Parent) :
   QSlider(opc_Parent),
   mf64_ToolTipSliderRangeOffset(0.0),
   mf64_ToolTipSliderRangeValueFactor(1.0),
   mf64_ToolTipValueOffset(0.0),
   me_RepresentationType(C_OscNodeDataPoolContent::eSINT32),
   ms32_ToolTipCoordVertical(0)
{
   connect(this, &C_OgeSliToolTipBase::valueChanged, this, &C_OgeSliToolTipBase::m_OnValueChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   \param[in]  os32_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliToolTipBase::SetValueCustom(const int32_t os32_Value)
{
   this->setValue(os32_Value);
   m_OnValueChange();
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
void C_OgeSliToolTipBase::SetToolTipParameters(const float64_t of64_ToolTipSliderRangeValueFactor,
                                               const float64_t of64_ToolTipValueOffset,
                                               const C_PuiSvDbDataElementScaling & orc_ToolTipUserScaling,
                                               const C_OscNodeDataPoolContent::E_Type oe_RepresentationType,
                                               const C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig)
{
   this->mf64_ToolTipSliderRangeOffset = static_cast<float64_t>(this->minimum());
   this->mf64_ToolTipSliderRangeValueFactor = of64_ToolTipSliderRangeValueFactor;
   this->mf64_ToolTipValueOffset = of64_ToolTipValueOffset;
   this->mc_ToolTipUserScaling = orc_ToolTipUserScaling;
   this->me_RepresentationType = oe_RepresentationType;
   this->mc_FormatterConfig = orc_FormatterConfig;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSliToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (this->mq_ToolTipActive == true)
   {
      if (opc_Event->type() == QEvent::ToolTip)
      {
         //show tooltip
         if (this->m_GetToolTip()->isVisible() == false)
         {
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

            if (pc_HelpEvent != NULL)
            {
               this->setMouseTracking(true);

               this->m_GetToolTip()->SetHeading(this->mc_ToolTipHeading);
               this->m_GetToolTip()->SetContent(this->mc_ToolTipContent);
               this->m_GetToolTip()->SetType(this->me_ToolTipType);
               this->m_GetToolTip()->show();
               this->ms32_ToolTipCoordVertical = pc_HelpEvent->pos().y();
               this->m_MoveToolTip();
            }
         }
         //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this widget
         // already
         opc_Event->accept();
         q_Return = true;
      }
      else if (opc_Event->type() == QEvent::Leave)
      {
         //hide on leave
         this->m_HideToolTip();

         this->setMouseTracking(false);

         opc_Event->accept();
         q_Return = true;
      }
      else
      {
         q_Return = QSlider::event(opc_Event);
      }
   }
   else
   {
      q_Return = QSlider::event(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle slider value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliToolTipBase::m_OnValueChange(void)
{
   QString c_Content;
   C_OscNodeDataPoolContent c_Tmp;

   c_Tmp.SetArray(false);
   c_Tmp.SetType(this->me_RepresentationType);

   //set slider value in content (c_Tmp)
   C_OscNodeDataPoolContentUtil::h_SetValueInContent(
      ((static_cast<float64_t>(this->value()) - this->mf64_ToolTipSliderRangeOffset) *
       this->mf64_ToolTipSliderRangeValueFactor) + this->mf64_ToolTipValueOffset,
      c_Tmp);

   //apply factor and offset
   c_Content = this->mc_FormatterConfig.GetSingleValueContentFormatted(c_Tmp, 0UL, this->mc_ToolTipUserScaling, NULL);

   this->SetToolTipInformation(C_GtGetText::h_GetText("Value"), c_Content);
   //Update directly
   if (this->m_GetToolTip() != NULL)
   {
      this->m_GetToolTip()->SetContent(c_Content);
   }

   // Update tooltip position
   this->m_MoveToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move tooltip to slider-handle position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSliToolTipBase::m_MoveToolTip(void)
{
   if (this->m_GetToolTip() != NULL)
   {
      QPoint c_ToolTipPos;
      c_ToolTipPos.setX(QStyle::sliderPositionFromValue(this->minimum(), this->maximum(), this->value(),
                                                        this->width() - 12)); // 2x border width + 2x margin
      c_ToolTipPos.setY(ms32_ToolTipCoordVertical);
      this->m_GetToolTip()->DoMove(this->mapToGlobal(c_ToolTipPos));
   }
}
