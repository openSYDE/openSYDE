//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box with auto fix to nearest raw value (implementation)

   Int 64 spin box with auto fix to nearest raw value

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QLineEdit>
#include "C_Uti.hpp"
#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeSpxInt64AutoFix.hpp"
#include "C_SdNdeDpContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
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
C_OgeSpxInt64AutoFix::C_OgeSpxInt64AutoFix(QWidget * const opc_Parent) :
   C_OgeSpxInt64ToolTipBase(opc_Parent),
   C_OgeSpxAutoFixBase()
{
   this->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current line edit width

   \return
   Current line edit width
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeSpxInt64AutoFix::GetLineEditWidth(void) const
{
   return this->lineEdit()->width();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix string after user confirm

   \param[in,out]  orc_Input  Current string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64AutoFix::fixup(QString & orc_Input) const
{
   int32_t s32_Pos = orc_Input.length();

   if (this->validate(orc_Input, s32_Pos) == QValidator::Intermediate)
   {
      const QString c_InputValue = this->m_ExtractSpinBoxValue(orc_Input);
      //Restrict to min & max
      if (this->GetIsUnsigned() == true)
      {
         uint64_t u64_Value;
         if (C_OgeSpxInt64AutoFix::mh_GetUnsignedValue(c_InputValue, u64_Value) == C_NO_ERR)
         {
            const uint64_t u64_Min = this->GetMinimum().toULongLong();
            const uint64_t u64_Max = this->GetMaximum().toULongLong();
            if (u64_Value < u64_Min)
            {
               u64_Value = u64_Min;
            }
            else if (u64_Value > u64_Max)
            {
               u64_Value = u64_Max;
            }
            else
            {
               const uint64_t u64_RangeValue = u64_Value - u64_Min;
               const float64_t f64_Steps =
                  std::round(static_cast<float64_t>(u64_RangeValue) / static_cast<float64_t>(this->GetStepWidth()));
               u64_Value = u64_Min + (static_cast<uint64_t>(f64_Steps) * this->GetStepWidth());
            }
            orc_Input = this->m_PrepareSpinBoxValue(QString::number(u64_Value));
         }
         else
         {
            C_OgeSpxInt64::fixup(orc_Input);
         }
      }
      else
      {
         int64_t s64_Value;
         if (C_OgeSpxInt64AutoFix::mh_GetSignedValue(c_InputValue, s64_Value) == C_NO_ERR)
         {
            const int64_t s64_Min = this->GetMinimum().toLongLong();
            const int64_t s64_Max = this->GetMaximum().toLongLong();
            if (s64_Value < s64_Min)
            {
               s64_Value = s64_Min;
            }
            else if (s64_Value > s64_Max)
            {
               s64_Value = s64_Max;
            }
            else
            {
               const uint64_t u64_RangeValue = static_cast<uint64_t>(s64_Value - s64_Min);
               const float64_t f64_Steps =
                  std::round(static_cast<float64_t>(u64_RangeValue) / static_cast<float64_t>(this->GetStepWidth()));
               const float64_t f64_RangeOffset = f64_Steps * static_cast<float64_t>(this->GetStepWidth());
               s64_Value = s64_Min + static_cast<int64_t>(f64_RangeOffset);
            }
            orc_Input = this->m_PrepareSpinBoxValue(QString::number(s64_Value));
         }
         else
         {
            C_OgeSpxInt64::fixup(orc_Input);
         }
      }
   }
   else
   {
      C_OgeSpxInt64::fixup(orc_Input);
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
QValidator::State C_OgeSpxInt64AutoFix::validate(QString & orc_Input, int32_t & ors32_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxInt64::validate(orc_Input, ors32_Pos);
   if (e_Retval == QValidator::Acceptable)
   {
      const QString c_ValueOnly = this->m_ExtractSpinBoxValue(orc_Input);
      if (this->GetIsUnsigned() == true)
      {
         uint64_t u64_Value;
         if (C_OgeSpxInt64AutoFix::mh_GetUnsignedValue(c_ValueOnly, u64_Value) == C_NO_ERR)
         {
            const uint64_t u64_Min = this->GetMinimum().toULongLong();
            const uint64_t u64_RangeValue = u64_Value - u64_Min;
            const float64_t f64_Steps = static_cast<float64_t>(u64_RangeValue) /
                                        static_cast<float64_t>(this->GetStepWidth());
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
      else
      {
         int64_t s64_Value;
         if (C_OgeSpxInt64AutoFix::mh_GetSignedValue(c_ValueOnly, s64_Value) == C_NO_ERR)
         {
            const int64_t s64_Min = this->GetMinimum().toLongLong();
            const uint64_t u64_RangeValue = static_cast<uint64_t>(s64_Value - s64_Min);
            const float64_t f64_Steps = static_cast<float64_t>(u64_RangeValue) /
                                        static_cast<float64_t>(this->GetStepWidth());
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
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init static values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64AutoFix::m_Init(void)
{
   tgl_assert(this->mc_UnscaledMin.GetType() == this->mc_UnscaledMax.GetType());
   if (this->mc_UnscaledMin.GetType() == this->mc_UnscaledMax.GetType())
   {
      QVariant c_ScaledMin;
      QVariant c_ScaledMax;
      uint64_t u64_ScaledMin = 0;
      uint64_t u64_ScaledMax = 0;
      int64_t s64_ScaledMin = 0;
      int64_t s64_ScaledMax = 0;
      uint64_t u64_StepWidth;
      float64_t f64_Tmp;

      if (C_OscUtils::h_IsScalingActive(this->mf64_Factor, this->mf64_Offset) == true)
      {
         this->mc_UnscaledMin.GetAnyValueAsFloat64(f64_Tmp, this->mu32_Index);
         f64_Tmp = C_OscUtils::h_GetValueScaled(f64_Tmp, this->mf64_Factor, this->mf64_Offset);
         if (this->GetIsUnsigned() == true)
         {
            u64_ScaledMin = static_cast<uint64_t>(f64_Tmp);
            c_ScaledMin = static_cast<QVariant>(u64_ScaledMin);
         }
         else
         {
            s64_ScaledMin = static_cast<int64_t>(f64_Tmp);
            c_ScaledMin = static_cast<QVariant>(s64_ScaledMin);
         }
         this->mc_UnscaledMax.GetAnyValueAsFloat64(f64_Tmp, this->mu32_Index);
         f64_Tmp = C_OscUtils::h_GetValueScaled(f64_Tmp, this->mf64_Factor, this->mf64_Offset);
         if (this->GetIsUnsigned() == true)
         {
            u64_ScaledMax = static_cast<uint64_t>(f64_Tmp);
            c_ScaledMax = static_cast<QVariant>(u64_ScaledMax);
         }
         else
         {
            s64_ScaledMax = static_cast<int64_t>(f64_Tmp);
            c_ScaledMax = static_cast<QVariant>(s64_ScaledMax);
         }
      }
      else
      {
         if (this->GetIsUnsigned() == true)
         {
            C_SdNdeDpContentUtil::h_GetAnyValueAsUint64(this->mc_UnscaledMin, u64_ScaledMin, this->mu32_Index);
            C_SdNdeDpContentUtil::h_GetAnyValueAsUint64(this->mc_UnscaledMax, u64_ScaledMax, this->mu32_Index);
            c_ScaledMin = static_cast<QVariant>(u64_ScaledMin);
            c_ScaledMax = static_cast<QVariant>(u64_ScaledMax);
         }
         else
         {
            C_SdNdeDpContentUtil::h_GetAnyValueAsSint64(this->mc_UnscaledMin, s64_ScaledMin, this->mu32_Index);
            C_SdNdeDpContentUtil::h_GetAnyValueAsSint64(this->mc_UnscaledMax, s64_ScaledMax, this->mu32_Index);
            c_ScaledMin = static_cast<QVariant>(s64_ScaledMin);
            c_ScaledMax = static_cast<QVariant>(s64_ScaledMax);
         }
      }
      this->SetMinimum(c_ScaledMin);
      this->SetMaximum(c_ScaledMax);
      if (this->mu64_NumberOfStepsAvailable > 0)
      {
         if (this->GetIsUnsigned() == true)
         {
            u64_StepWidth = (u64_ScaledMax - u64_ScaledMin) / this->mu64_NumberOfStepsAvailable;
         }
         else
         {
            u64_StepWidth = static_cast<uint64_t>(s64_ScaledMax - s64_ScaledMin) /
                            this->mu64_NumberOfStepsAvailable;
         }
         //Just set some default
         if (u64_StepWidth == 0)
         {
            u64_StepWidth = 1;
         }
      }
      else
      {
         u64_StepWidth = 1;
      }
      this->SetStepWidth(u64_StepWidth);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value from string if any

   \param[in]   orc_Input     Input string
   \param[out]  oru64_Output  Output value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeSpxInt64AutoFix::mh_GetUnsignedValue(const QString & orc_Input, uint64_t & oru64_Output)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_Ok;

   oru64_Output = orc_Input.toULongLong(&q_Ok);
   if (q_Ok == false)
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value from string if any

   \param[in]   orc_Input     Input string
   \param[out]  ors64_Output  Output value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeSpxInt64AutoFix::mh_GetSignedValue(const QString & orc_Input, int64_t & ors64_Output)
{
   int32_t s32_Retval = C_NO_ERR;
   bool q_Ok;

   ors64_Output = orc_Input.toLongLong(&q_Ok);
   if (q_Ok == false)
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
