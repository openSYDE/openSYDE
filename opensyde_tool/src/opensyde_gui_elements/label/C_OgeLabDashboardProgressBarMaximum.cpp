//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard progress bar item value, scaled to maximum size (implementation)

   Label for dashboard progress bar item value, scaled to maximum size.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeLabDashboardProgressBarMaximum.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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
C_OgeLabDashboardProgressBarMaximum::C_OgeLabDashboardProgressBarMaximum(QWidget * const opc_Parent) :
   C_OgeLabAdaptiveSize(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabDashboardProgressBarMaximum::AdjustFontToSize(void)
{
   const sintn sn_NewPointSize = C_OgeWiUtil::h_UpdateFontSize(this, this->text(), 1.0F, true);

   if (sn_NewPointSize > 0)
   {
      this->setStyleSheet(static_cast<QString>("QLabel{\n"
                                               "font-size:%1pt;\n"
                                               "margin-bottom:-%2px;\n"
                                               "}").arg(sn_NewPointSize).arg((sn_NewPointSize * 30) / 100));
   }
}
