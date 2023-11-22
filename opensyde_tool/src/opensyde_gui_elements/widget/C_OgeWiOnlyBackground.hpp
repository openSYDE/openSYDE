//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with a simple background (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIONLYBACKGROUND_HPP
#define C_OGEWIONLYBACKGROUND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OgeWiWithToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiOnlyBackground :
   public C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   C_OgeWiOnlyBackground(QWidget * const opc_Parent = NULL);
   void SetBackgroundColor(const int32_t os32_Value);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
