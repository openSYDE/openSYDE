//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle custom trigger for custom spin box

   Handle custom trigger for custom spin box

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeSpxInt64.h"
#include "C_OgeSpxDoubleAutoFixCustomTrigger.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxDoubleAutoFixCustomTrigger::C_OgeSpxDoubleAutoFixCustomTrigger(QWidget * const opc_Parent) :
   C_OgeSpxDoubleAutoFix(opc_Parent)
{
   connect(this, &C_OgeSpxDoubleAutoFixCustomTrigger::editingFinished, this,
           &C_OgeSpxDoubleAutoFixCustomTrigger::SigValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set suffix

   \param[in]  orc_Suffix  Suffix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleAutoFixCustomTrigger::SetSuffix(const QString & orc_Suffix)
{
   if (orc_Suffix.isEmpty())
   {
      this->setSuffix(orc_Suffix);
   }
   else
   {
      this->setSuffix(C_OgeSpxInt64::hc_UNIT_SEPERATION_CHARACTER + orc_Suffix);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do x steps

   \param[in]  osn_Steps   Step count to do
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleAutoFixCustomTrigger::stepBy(const sintn osn_Steps)
{
   C_OgeSpxDoubleAutoFix::stepBy(osn_Steps);
   Q_EMIT (this->SigValueChanged());
}
