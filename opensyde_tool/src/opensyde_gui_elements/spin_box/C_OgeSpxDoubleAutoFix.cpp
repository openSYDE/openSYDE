//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with auto fix to nearest raw value (implementation)

   Double spin box with auto fix to nearest raw value

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QLineEdit>
#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeSpxInt64.hpp"
#include "C_OgeSpxDoubleAutoFix.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
int32_t C_OgeSpxDoubleAutoFix::GetLineEditWidth(void) const
{
   return this->lineEdit()->width();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix intermediate string

   \param[in,out]  orc_String    Intermediate string to fix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleAutoFix::fixup(QString & orc_String) const
{
   int32_t s32_Pos = 0;

   if ((validate(orc_String, s32_Pos) == QValidator::Intermediate) && (this->mu64_NumberOfStepsAvailable > 0))
   {
      float64_t f64_Value;
      const QString c_ValueOnly = this->m_ExtractSpinBoxValue(orc_String);
      if (C_OgeSpxDoubleAutoFix::mh_GetValue(c_ValueOnly, f64_Value) == C_NO_ERR)
      {
         //Get the value in the data type range
         const float64_t f64_RangeValue = f64_Value - this->mf64_ScaledMin;
         //Check if the value fits in the step width and round if necessary
         const float64_t f64_Steps = std::round(f64_RangeValue / this->mf64_StepWidth);
         //Apply improved value
         orc_String =
            this->m_PrepareSpinBoxValue(this->textFromValue(this->mf64_ScaledMin + (f64_Steps * this->mf64_StepWidth)));
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

   Q_UNUSED(s32_Pos) // last value is irrelevant
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validate current input string

   \param[in,out]  orc_Input  Input string
   \param[in,out]  ors32_Pos   Position

   \return
   Invalid      Unusable
   Intermediate Might be valid on next user input
   Acceptable   Completely valid
*/
//----------------------------------------------------------------------------------------------------------------------
QValidator::State C_OgeSpxDoubleAutoFix::validate(QString & orc_Input, int32_t & ors32_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxDoubleToolTipBase::validate(orc_Input, ors32_Pos);
   if ((e_Retval == QValidator::Acceptable) && (this->mu64_NumberOfStepsAvailable > 0))
   {
      float64_t f64_Value;
      const QString c_ValueOnly = this->m_ExtractSpinBoxValue(orc_Input);
      if (C_OgeSpxDoubleAutoFix::mh_GetValue(c_ValueOnly, f64_Value) == C_NO_ERR)
      {
         const float64_t f64_RangeValue = f64_Value - this->mf64_ScaledMin;
         const float64_t f64_Steps = f64_RangeValue / this->mf64_StepWidth;
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
   std::vector<float64_t> c_Tmp;
   int32_t s32_DecimalsFactor;
   int32_t s32_DecimalsOffset;

   C_SdNdeDpContentUtil::h_GetValuesAsFloat64(this->mc_UnscaledMin, c_Tmp);
   if (this->mu32_Index < c_Tmp.size())
   {
      this->mf64_ScaledMin = C_OscUtils::h_GetValueScaled(c_Tmp[this->mu32_Index], this->mf64_Factor,
                                                          this->mf64_Offset);
   }
   else
   {
      this->mf64_ScaledMin = 0.0;
      tgl_assert(false);
   }
   C_SdNdeDpContentUtil::h_GetValuesAsFloat64(this->mc_UnscaledMax, c_Tmp);
   if (this->mu32_Index < c_Tmp.size())
   {
      this->mf64_ScaledMax = C_OscUtils::h_GetValueScaled(c_Tmp[this->mu32_Index], this->mf64_Factor,
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
   // Small addition: only do this if underlying type DOES define a discrete number of steps
   s32_DecimalsFactor = C_Uti::h_GetNumberOfDecimals(this->mf64_Factor);
   s32_DecimalsOffset = C_Uti::h_GetNumberOfDecimals(this->mf64_Offset);
   if (((s32_DecimalsFactor < 0) || (s32_DecimalsOffset < 0)) || (this->mu64_NumberOfStepsAvailable == 0ULL))
   {
      this->setDecimals(ms32_DOUBLE_SPIN_BOX_DECIMAL_COUNT);
   }
   else
   {
      this->setDecimals(std::max(std::max(s32_DecimalsFactor, s32_DecimalsOffset), static_cast<int32_t>(1L)));
   }
   //Step width
   if (this->mu64_NumberOfStepsAvailable > 0)
   {
      this->mf64_StepWidth = (this->mf64_ScaledMax - this->mf64_ScaledMin) /
                             static_cast<float64_t>(this->mu64_NumberOfStepsAvailable);
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
/*! \brief  Prepare spin box value

   \param[in]  orc_Text    Text

   \return
   String with suffix
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxDoubleAutoFix::m_PrepareSpinBoxValue(const QString & orc_Text) const
{
   QString c_Retval;

   if (this->suffix().compare("") == 0)
   {
      c_Retval = orc_Text;
   }
   else
   {
      c_Retval = static_cast<QString>("%1 %2").arg(orc_Text, this->suffix());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract spin box value

   \param[in]  orc_Text    Text

   \return
   String without suffix
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxDoubleAutoFix::m_ExtractSpinBoxValue(const QString & orc_Text) const
{
   const QString c_Retval = C_OgeSpxInt64::h_ExtractSpinBoxValue(orc_Text, this->suffix());

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value from string if any

   \param[in]   orc_Input     Input
   \param[out]  orf64_Output  Output value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeSpxDoubleAutoFix::mh_GetValue(const QString & orc_Input, float64_t & orf64_Output)
{
   int32_t s32_Retval = C_NO_ERR;
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
