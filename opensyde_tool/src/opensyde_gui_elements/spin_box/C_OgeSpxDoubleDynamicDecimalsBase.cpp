//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with dynamic decimal input (implementation)

   Base class of double spin boxes, where the number of shown decimals
   dynamically changes during user input.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QLineEdit>
#include <QKeyEvent>
#include "stwtypes.h"
#include "C_Uti.h"
#include "constants.h"
#include "C_OgeSpxDoubleDynamicDecimalsBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxDoubleDynamicDecimalsBase::C_OgeSpxDoubleDynamicDecimalsBase(QWidget * const opc_Parent) :
   QDoubleSpinBox(opc_Parent)
{
   this->setDecimals(msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten textFromValue method

   This virtual function is used by the spin box whenever it needs to display the given value.
   Cut annoying trailing zeros and give the possibility to show 0,00000...0123.

   \param[in]  of64_Value  Value

   \return
   Spinbox text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxDoubleDynamicDecimalsBase::textFromValue(const float64 of64_Value) const
{
   QString c_StringFromValue;
   QChar c_DecimalSeparator;

   c_StringFromValue = C_Uti::h_GetStringFromDouble(of64_Value);

   // do not show more decimals than the number of QDoubleSpinbox::decimals()
   // which is per default msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT but could be customized
   if (c_StringFromValue.split(".").at(1).length() > this->decimals())
   {
      c_StringFromValue = QDoubleSpinBox::textFromValue(of64_Value);
   }

   // show the string with local decimal separator (1,423 vs 1.423)
   c_DecimalSeparator = this->locale().decimalPoint();
   c_StringFromValue.replace(".", c_DecimalSeparator);

   return c_StringFromValue;
}
