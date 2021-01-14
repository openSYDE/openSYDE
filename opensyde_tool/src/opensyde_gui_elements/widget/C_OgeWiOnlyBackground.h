//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with a simple background (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIONLYBACKGROUND_H
#define C_OGEWIONLYBACKGROUND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiOnlyBackground :
   public C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   C_OgeWiOnlyBackground(QWidget * const opc_Parent = NULL);
   void SetBackgroundColor(const stw_types::sintn osn_Value);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
