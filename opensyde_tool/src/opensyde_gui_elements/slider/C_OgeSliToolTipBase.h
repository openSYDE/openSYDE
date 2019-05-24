//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Slider with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESLITOOLTIPBASE_H
#define C_OGESLITOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSlider>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSliToolTipBase :
   public QSlider,
   public C_OgeToolTipBase
{
public:
   C_OgeSliToolTipBase(QWidget * const opc_Parent = NULL);

   void SetValueCustom(const stw_types::sintn osn_Value);
   void SetToolTipParameters(const stw_types::float64 of64_ToolTipRangeOffset,
                             const stw_types::float64 of64_ToolTipOffset, const stw_types::float64 of64_ToolTipFactor,
                             const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_RepresentationType);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

protected:
   void m_OnValueChange(void);

private:
   stw_types::float64 mf64_ToolTipRangeOffset;
   stw_types::float64 mf64_ToolTipOffset;
   stw_types::float64 mf64_ToolTipFactor;
   stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type me_RepresentationType;
   stw_types::sint32 ms32_ToolTipCoordY;
   void m_MoveToolTip(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
