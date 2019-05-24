//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with auto fix to nearest raw value (implementation)

   Double spin box with auto fix to nearest raw value

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <QLineEdit>
#include "C_Uti.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_OgeSpxDoubleAutoFix.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
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
C_OgeSpxDoubleAutoFix::C_OgeSpxDoubleAutoFix(QWidget * const opc_Parent) :
   C_OgeSpxDoubleToolTipBase(opc_Parent),
   C_OgeSpxAutoFixBase(),
   mf64_ScaledMin(0.0),
   mf64_ScaledMax(0.0),
   mf64_StepWidth(1.0)
{
   this->setCorrectionMode(QDoubleSpinBox::CorrectToNearestValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current line edit width

   \return
   Current line edit width
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeSpxDoubleAutoFix::GetLineEditWidth(void) const
{
   return this->lineEdit()->width();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix intermediate string

   \param[in,out] orc_String Intermediate string to fix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleAutoFix::fixup(QString & orc_String) const
{
   sintn sn_Pos = 0;

   if ((validate(orc_String, sn_Pos) == QValidator::Intermediate) && (this->mu64_NumberOfStepsAvailable > 0))
   {
      float64 f64_Value;
      if (C_OgeSpxDoubleAutoFix::mh_GetValue(orc_String, f64_Value) == C_NO_ERR)
      {
         //Get the value in the data type range
         const float64 f64_RangeValue = f64_Value - this->mf64_ScaledMin;
         //Check if the value fits in the step width and round if necessary
         //lint -e{18,762,830,1055}  C++ 11 feature
         const float64 f64_Steps = std::round(f64_RangeValue / this->mf64_StepWidth);
         //Apply improved value
         orc_String = this->textFromValue(this->mf64_ScaledMin + (f64_Steps * this->mf64_StepWidth));
      }
      else
      {
         C_OgeSpxDoubleToolTipBase::fixup(orc_String);
      }
   }
   else
   {
      C_OgeSpxDoubleToolTipBase::fixup(orc_String);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validate current input string

   \param[in,out] orc_Input Input string
   \param[in,out] orc_Pos   Position

   \return
   Invalid      Unusable
   Intermediate Might be valid on next user input
   Acceptable   Completely valid
*/
//----------------------------------------------------------------------------------------------------------------------
QValidator::State C_OgeSpxDoubleAutoFix::validate(QString & orc_Input, sintn & orc_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxDoubleToolTipBase::validate(orc_Input, orc_Pos);
   if ((e_Retval == QValidator::Acceptable) && (this->mu64_NumberOfStepsAvailable > 0))
   {
      float64 f64_Value;
      if (C_OgeSpxDoubleAutoFix::mh_GetValue(orc_Input, f64_Value) == C_NO_ERR)
      {
         const float64 f64_RangeValue = f64_Value - this->mf64_ScaledMin;
         const float64 f64_Steps = f64_RangeValue / this->mf64_StepWidth;
         if (C_Uti::h_CheckFloatHasNoFractionPart(f64_Steps) == true)
         {
            e_Retval = QValidator::Acceptable;
         }
         else
         {
            e_Retval = QValidator::Intermediate;
         }
      }
      else
      {
         e_Retval = QValidator::Intermediate;
      }
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init static values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleAutoFix::m_Init(void)
{
   std::vector<float64> c_Tmp;
   sint32 s32_DecimalsFactor;
   sint32 s32_DecimalsOffset;

   C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(this->mc_UnscaledMin, c_Tmp);
   if (this->mu32_Index < c_Tmp.size())
   {
      this->mf64_ScaledMin = C_OSCUtils::h_GetValueScaled(c_Tmp[this->mu32_Index], this->mf64_Factor,
                                                          this->mf64_Offset);
   }
   else
   {
      this->mf64_ScaledMin = 0.0;
      tgl_assert(false);
   }
   C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(this->mc_UnscaledMax, c_Tmp);
   if (this->mu32_Index < c_Tmp.size())
   {
      this->mf64_ScaledMax = C_OSCUtils::h_GetValueScaled(c_Tmp[this->mu32_Index], this->mf64_Factor,
                                                          this->mf64_Offset);
   }
   else
   {
      this->mf64_ScaledMax = 0.0;
      tgl_assert(false);
   }
   //Range check
   if (this->mf64_ScaledMin > this->mf64_ScaledMax)
   {
      this->mf64_ScaledMin = this->mf64_ScaledMax;
   }
   //Min max
   this->SetMinimumCustom(this->mf64_ScaledMin);
   this->SetMaximumCustom(this->mf64_ScaledMax);
   //Decimals
   //Check factor and offset and use maximum number of decimals if both have a defined number of decimals
   // Small addition: only do this if unerlying type DOES define a discrete number of steps
   s32_DecimalsFactor = C_Uti::h_GetNumberOfDecimals(this->mf64_Factor);
   s32_DecimalsOffset = C_Uti::h_GetNumberOfDecimals(this->mf64_Offset);
   if (((s32_DecimalsFactor < 0) || (s32_DecimalsOffset < 0)) || (this->mu64_NumberOfStepsAvailable == 0ULL))
   {
      this->setDecimals(msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT);
   }
   else
   {
      this->setDecimals(std::max(std::max(s32_DecimalsFactor, s32_DecimalsOffset), 1L));
   }
   //Step width
   if (this->mu64_NumberOfStepsAvailable > 0)
   {
      this->mf64_StepWidth = (this->mf64_ScaledMax - this->mf64_ScaledMin) /
                             static_cast<float64>(this->mu64_NumberOfStepsAvailable);
      if (this->mf64_StepWidth > 0.0)
      {
         //Fine
      }
      else
      {
         //Just set some default
         this->mf64_StepWidth = 1.0;
      }
   }
   else
   {
      this->mf64_StepWidth = 1.0;
   }
   this->setSingleStep(this->mf64_StepWidth);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value from string if any

   \param[in]  orc_Value    Input string
   \param[out] orf64_Output Output value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OgeSpxDoubleAutoFix::mh_GetValue(const QString & orc_Input, float64 & orf64_Output)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_Ok;

   orf64_Output = orc_Input.toDouble(&q_Ok);
   if (q_Ok == false)
   {
      //Attempt replace of ','
      QString c_Tmp = orc_Input;
      c_Tmp.replace(',', '.');
      orf64_Output = c_Tmp.toDouble(&q_Ok);
      if (q_Ok == false)
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}
