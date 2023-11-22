//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Slider with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESLITOOLTIPBASE_HPP
#define C_OGESLITOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSlider>
#include "stwtypes.hpp"
#include "C_OgeToolTipBase.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_PuiSvDbDataElementDisplayFormatterConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSliToolTipBase :
   public QSlider,
   public C_OgeToolTipBase
{
public:
   C_OgeSliToolTipBase(QWidget * const opc_Parent = NULL);

   void SetValueCustom(const int32_t os32_Value);
   void SetToolTipParameters(const float64_t of64_ToolTipSliderRangeValueFactor,
                             const float64_t of64_ToolTipValueOffset,
                             const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_ToolTipUserScaling,
                             const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_RepresentationType,
                             const stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig);

   bool event(QEvent * const opc_Event) override;

protected:
   void m_OnValueChange(void);

private:
   float64_t mf64_ToolTipSliderRangeOffset;
   float64_t mf64_ToolTipSliderRangeValueFactor;
   float64_t mf64_ToolTipValueOffset;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling mc_ToolTipUserScaling;
   stw::opensyde_core::C_OscNodeDataPoolContent::E_Type me_RepresentationType;
   int32_t ms32_ToolTipCoordVertical;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatterConfig mc_FormatterConfig;

   void m_MoveToolTip(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
