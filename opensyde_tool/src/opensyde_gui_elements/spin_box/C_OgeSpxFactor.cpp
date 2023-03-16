//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box which should avoid a zero value (implementation)

   Spin box which should avoid a zero value

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <limits>
#include "constants.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeSpxFactor.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64_t C_OgeSpxFactor::mhf64_FACTOR_MIN = std::pow(10, -ms32_DOUBLE_SPIN_BOX_DECIMAL_COUNT);

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
C_OgeSpxFactor::C_OgeSpxFactor(QWidget * const opc_Parent) :
   C_OgeSpxDoubleToolTipBase(opc_Parent)
{
   this->SetMinimumCustom(std::numeric_limits<float64_t>::lowest());
   this->SetMaximumCustom(std::numeric_limits<float64_t>::max());
   this->setValue(1.0); // default value
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten step by event slot

   Here: Skip 0

   \param[in]  os32_Steps   Steps
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxFactor::stepBy(const int32_t os32_Steps)
{
   //Only allow step if resulting value is ABOVE zero
   float64_t f64_CurValue = this->value();

   if (os32_Steps > 0)
   {
      C_OgeSpxDoubleToolTipBase::stepBy(os32_Steps);
   }
   else
   {
      const int32_t s32_StepsPos = std::abs(os32_Steps);
      if (f64_CurValue > (static_cast<float64_t>(s32_StepsPos) * 1.0))
      {
         C_OgeSpxDoubleToolTipBase::stepBy(os32_Steps);
      }
      else
      {
         //Convert to max allowed number of steps
         int32_t s32_MaxStepsPossible = 0;
         while (f64_CurValue > 0.0)
         {
            f64_CurValue -= 1.0;
            ++s32_MaxStepsPossible;
         }
         C_OgeSpxDoubleToolTipBase::stepBy(s32_MaxStepsPossible);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten validate event slot

   Here: Skip 0

   \param[in]  orc_Text    Text
   \param[in]  ors32_Pos    Position

   \return
   Validation state (invalid, intermediate, valid)
*/
//----------------------------------------------------------------------------------------------------------------------
QValidator::State C_OgeSpxFactor::validate(QString & orc_Text, int32_t & ors32_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxDoubleToolTipBase::validate(orc_Text, ors32_Pos);
   // 0 would return acceptable, but we do not allow zero
   if ((e_Retval == QValidator::Acceptable) &&
       (C_OscUtils::h_IsFloat64NearlyEqual(valueFromText(orc_Text), 0.0) == true))
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
