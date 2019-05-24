//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box which should avoid a zero value (implementation)

   Spin box which should avoid a zero value

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <limits>
#include "constants.h"
#include "C_OSCUtils.h"
#include "C_OgeSpxFactor.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::float64 C_OgeSpxFactor::mhf64_FACTOR_MIN = std::pow(10, -msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT);

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
C_OgeSpxFactor::C_OgeSpxFactor(QWidget * const opc_Parent) :
   C_OgeSpxDoubleToolTipBase(opc_Parent)
{
   //lint -e{10,530,747,1013,1015,1960}  c++11 feature
   this->SetMinimumCustom(std::numeric_limits<float64>::lowest());
   this->SetMaximumCustom(std::numeric_limits<float64>::max());
   this->setValue(1.0); // default value
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten step by event slot

   Here: Skip 0

   \param[in] osn_Steps Steps
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxFactor::stepBy(const sintn osn_Steps)
{
   //Only allow step if resulting value is ABOVE zero
   float64 f64_CurValue = this->value();

   if (osn_Steps > 0)
   {
      C_OgeSpxDoubleToolTipBase::stepBy(osn_Steps);
   }
   else
   {
      const sintn sn_StepsPos = osn_Steps * -1;
      if (f64_CurValue > (static_cast<float64>(sn_StepsPos) * 1.0))
      {
         C_OgeSpxDoubleToolTipBase::stepBy(osn_Steps);
      }
      else
      {
         //Convert to max allowed number of steps
         sintn sn_MaxStepsPossible = 0;
         while (f64_CurValue > 0.0)
         {
            f64_CurValue -= 1.0;
            ++sn_MaxStepsPossible;
         }
         C_OgeSpxDoubleToolTipBase::stepBy(sn_MaxStepsPossible);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten validate event slot

   Here: Skip 0

   \param[in] orc_Text Text
   \param[in] orsn_Pos Pos
*/
//----------------------------------------------------------------------------------------------------------------------
QValidator::State C_OgeSpxFactor::validate(QString & orc_Text, sintn & orsn_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxDoubleToolTipBase::validate(orc_Text, orsn_Pos);
   // 0 would return acceptable, but we do not allow zero
   if ((e_Retval == QValidator::Acceptable) &&
       (C_OSCUtils::h_IsFloat64NearlyEqual(valueFromText(orc_Text), 0.0) == true))
   {
      e_Retval = QValidator::State::Intermediate;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return allowed step actions

   \return
   StepDownEnabled                 Down action allowed
   StepUpEnabled                   Up action allowed
   StepUpEnabled | StepDownEnabled Up and down actions allowed
*/
//----------------------------------------------------------------------------------------------------------------------
QAbstractSpinBox::StepEnabled C_OgeSpxFactor::stepEnabled(void) const
{
   QAbstractSpinBox::StepEnabled c_Retval;
   if (this->value() > 1.0)
   {
      c_Retval = StepUpEnabled | StepDownEnabled;
   }
   else
   {
      c_Retval = StepUpEnabled;
   }
   return c_Retval;
}
