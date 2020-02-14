//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box for int64 values (implementation)

   Spin box for int64 values

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QLineEdit>
#include "TGLUtils.h"
#include "C_OgeSpxInt64.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
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

   \param[in,out]  opc_Parent       Optional pointer to parent
   \param[in]      orq_IsUnsigned   Indicator if this spin box should handle unsigned 64 bit or signed 64 bit
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxInt64::C_OgeSpxInt64(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   QAbstractSpinBox(opc_Parent),
   mq_IsUnsigned(orq_IsUnsigned),
   mu64_StepWidth(1)
{
   //Init default min max and default value
   m_ResetMinMax();
   if (this->mq_IsUnsigned == true)
   {
      this->SetValue(QVariant(0ULL), false);
   }
   else
   {
      this->SetValue(QVariant(0LL), false);
   }
   connect(this->lineEdit(), &QLineEdit::editingFinished, this, &C_OgeSpxInt64::SigValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get spin box internal used variable state

   \return
   true:  Internal variable is unsigned
   false: Internal variable is signed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxInt64::GetIsUnsigned(void) const
{
   return mq_IsUnsigned;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new internal value

   \param[in]  orc_Value            New value
   \param[in]  oq_AllowValueUpdate  Flag to suppress or request value update signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetValue(const QVariant & orc_Value, const bool oq_AllowValueUpdate)
{
   QString c_ValueStr;
   QVariant c_Value = m_PrepareValue(orc_Value);
   QLineEdit * const pc_LineEdit = this->lineEdit();
   bool q_Change;

   if (this->GetValue() == c_Value)
   {
      q_Change = false;
   }
   else
   {
      q_Change = true;
   }

   m_ApplyMinMax(c_Value);
   if (mq_IsUnsigned == true)
   {
      if (c_Value.type() == QVariant::ULongLong)
      {
         c_ValueStr = QString::number(c_Value.toULongLong());
      }
      else if (c_Value.type() == QVariant::UInt)
      {
         c_ValueStr = QString::number(c_Value.toUInt());
      }
      else
      {
         tgl_assert(false);
      }
   }
   else
   {
      if (c_Value.type() == QVariant::LongLong)
      {
         c_ValueStr = QString::number(c_Value.toLongLong());
      }
      else if (c_Value.type() == QVariant::Int)
      {
         c_ValueStr = QString::number(c_Value.toInt());
      }
      else
      {
         tgl_assert(false);
      }
   }
   if (this->mc_Suffix.compare("") == 0)
   {
      pc_LineEdit->setText(QString("%1").arg(c_ValueStr));
   }
   else
   {
      pc_LineEdit->setText(QString("%1 %2").arg(c_ValueStr, this->mc_Suffix));
   }
   //Behave as double spin box, only send changes if any
   if ((q_Change == true) && (oq_AllowValueUpdate))
   {
      Q_EMIT this->SigValueChanged();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set single step width

   \param[in]  ou64_Step   Single step width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetStepWidth(const uint64 ou64_Step)
{
   if (ou64_Step > 0)
   {
      this->mu64_StepWidth = ou64_Step;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new minimum value

   Warning: No action if new value over maximum

   \param[in]  orc_Minimum    New minimum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetMinimum(const QVariant & orc_Minimum)
{
   this->mc_Minimum = m_PrepareValue(orc_Minimum);
   Q_EMIT this->SigMinMaxChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new maximum value

   Warning: No action if new value below minimum

   \param[in]  orc_Maximum    New maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetMaximum(const QVariant & orc_Maximum)
{
   this->mc_Maximum = m_PrepareValue(orc_Maximum);
   Q_EMIT this->SigMinMaxChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set suffix

   \param[in]  orc_Value   New suffix
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetSuffix(const QString & orc_Value)
{
   this->mc_Suffix = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get internal value

   \return
   Internal value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_OgeSpxInt64::GetValue(void) const
{
   QVariant c_Retval;
   const QLineEdit * const pc_LineEdit = this->lineEdit();
   QString c_Text = pc_LineEdit->text();

   //Eliminate suffix
   c_Text = c_Text.remove(this->mc_Suffix);

   //Change invalid value if necessary
   this->fixup(c_Text);

   if (mq_IsUnsigned == true)
   {
      c_Retval = c_Text.toULongLong();
   }
   else
   {
      c_Retval = c_Text.toLongLong();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimum value

   \return
   Current minimum value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_OgeSpxInt64::GetMinimum(void) const
{
   return this->mc_Minimum;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get maximum value

   \return
   Current maximum value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_OgeSpxInt64::GetMaximum(void) const
{
   return this->mc_Maximum;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get suffix

   \return
   Current suffix
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxInt64::GetSuffix(void) const
{
   return this->mc_Suffix;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get step width

   \return
   Current step width
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_OgeSpxInt64::GetStepWidth(void) const
{
   return this->mu64_StepWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do x steps

   \param[in]  osn_Steps   Step count to do
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::stepBy(const sintn osn_Steps)
{
   QVariant c_Tmp = this->GetValue();

   if (osn_Steps > 0)
   {
      //Step by step (avoid stepping over min/max)
      for (sint32 s32_Step = 0L; s32_Step < osn_Steps; ++s32_Step)
      {
         if (mq_IsUnsigned == true)
         {
            const uint64 u64_Max = this->mc_Maximum.toULongLong();
            uint64 u64_Tmp = c_Tmp.toULongLong();
            const uint64 u64_AllowedSteps = u64_Max - u64_Tmp;
            if ((u64_Tmp < u64_Max) && (u64_AllowedSteps >= this->mu64_StepWidth))
            {
               u64_Tmp += this->mu64_StepWidth;
               if (u64_Tmp < u64_Max)
               {
                  c_Tmp = u64_Tmp;
               }
               else
               {
                  c_Tmp = u64_Max;
               }
            }
            else
            {
               c_Tmp = u64_Max;
            }
         }
         else
         {
            const sint64 s64_Max = this->mc_Maximum.toLongLong();
            sint64 s64_Tmp = c_Tmp.toLongLong();
            const uint64 u64_AllowedSteps = static_cast<uint64>(s64_Max - s64_Tmp);
            if ((s64_Tmp < s64_Max) && (u64_AllowedSteps >= this->mu64_StepWidth))
            {
               s64_Tmp += static_cast<sint64>(this->mu64_StepWidth);
               if (s64_Tmp < s64_Max)
               {
                  c_Tmp = s64_Tmp;
               }
               else
               {
                  c_Tmp = s64_Max;
               }
            }
            else
            {
               c_Tmp = s64_Max;
            }
         }
      }
   }
   else if (osn_Steps < 0)
   {
      //Step by step (avoid stepping over min/max)
      for (sint32 s32_Step = 0L; s32_Step > osn_Steps; --s32_Step)
      {
         if (mq_IsUnsigned == true)
         {
            const uint64 u64_Min = this->mc_Minimum.toULongLong();
            uint64 u64_Tmp = c_Tmp.toULongLong();
            const uint64 u64_AllowedSteps = u64_Tmp - u64_Min;
            if ((u64_Tmp > u64_Min) && (u64_AllowedSteps >= this->mu64_StepWidth))
            {
               u64_Tmp -= this->mu64_StepWidth;
               if (u64_Tmp > u64_Min)
               {
                  c_Tmp = u64_Tmp;
               }
               else
               {
                  c_Tmp = u64_Min;
               }
            }
            else
            {
               c_Tmp = u64_Min;
            }
         }
         else
         {
            const sint64 s64_Min = this->mc_Minimum.toLongLong();
            sint64 s64_Tmp = c_Tmp.toLongLong();
            const uint64 u64_AllowedSteps = static_cast<uint64>(s64_Tmp - s64_Min);
            if ((s64_Tmp > s64_Min) && (u64_AllowedSteps >= this->mu64_StepWidth))
            {
               s64_Tmp -= static_cast<sint64>(this->mu64_StepWidth);
               if (s64_Tmp > s64_Min)
               {
                  c_Tmp = s64_Tmp;
               }
               else
               {
                  c_Tmp = s64_Min;
               }
            }
            else
            {
               c_Tmp = s64_Min;
            }
         }
      }
   }
   else
   {
      //No change
   }
   this->SetValue(c_Tmp, true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validate current input string

   \param[in,out]  orc_Input  Input string
   \param[in,out]  orc_Pos    Position

   \return
   Invalid      Unusable
   Intermediate Might be valid on next user input
   Acceptable   Completely valid
*/
//----------------------------------------------------------------------------------------------------------------------
QValidator::State C_OgeSpxInt64::validate(QString & orc_Input, sintn & orc_Pos) const
{
   QValidator::State e_Retval;
   if (((orc_Input.compare("") == 0) || (orc_Input.compare("+") == 0)) || (orc_Input.compare("-") == 0))
   {
      e_Retval = QValidator::Intermediate;
   }
   else
   {
      bool q_Result = true;
      bool * const pc_Result = &q_Result;
      bool q_IsUnderMinimum;
      bool q_IsOverMaximum;

      Q_UNUSED(orc_Pos)
      if (this->mq_IsUnsigned == true)
      {
         const uint64 u64_Test = orc_Input.toULongLong(pc_Result);
         if (q_Result == true)
         {
            m_CheckMinMax(u64_Test, q_IsUnderMinimum, q_IsOverMaximum);
            if ((q_IsUnderMinimum == true) || (q_IsOverMaximum == true))
            {
               e_Retval = QValidator::Intermediate;
            }
            else
            {
               e_Retval = QValidator::Acceptable;
            }
         }
         else
         {
            e_Retval = QValidator::Invalid;
         }
      }
      else
      {
         const sint64 s64_Test = orc_Input.toLongLong(pc_Result);
         if (q_Result == true)
         {
            m_CheckMinMax(s64_Test, q_IsUnderMinimum, q_IsOverMaximum);
            if ((q_IsUnderMinimum == true) || (q_IsOverMaximum == true))
            {
               e_Retval = QValidator::Intermediate;
            }
            else
            {
               e_Retval = QValidator::Acceptable;
            }
         }
         else
         {
            e_Retval = QValidator::Invalid;
         }
      }
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
QAbstractSpinBox::StepEnabled C_OgeSpxInt64::stepEnabled(void) const
{
   QAbstractSpinBox::StepEnabled c_Retval;
   QVariant c_Tmp = this->GetValue();
   if (mq_IsUnsigned == true)
   {
      uint64 u64_Tmp = c_Tmp.toULongLong();
      if (u64_Tmp == this->mc_Minimum.toULongLong())
      {
         c_Retval = StepUpEnabled;
      }
      else if (u64_Tmp == this->mc_Maximum.toULongLong())
      {
         c_Retval = StepDownEnabled;
      }
      else
      {
         c_Retval = StepUpEnabled | StepDownEnabled;
      }
   }
   else
   {
      sint64 s64_Tmp = c_Tmp.toLongLong();
      if (s64_Tmp == this->mc_Minimum.toLongLong())
      {
         c_Retval = StepUpEnabled;
      }
      else if (s64_Tmp == this->mc_Maximum.toLongLong())
      {
         c_Retval = StepDownEnabled;
      }
      else
      {
         c_Retval = StepUpEnabled | StepDownEnabled;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unsigned or signed state

   \param[in]  orq_Value   Flag if is unsigned
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::SetIsUnsigned(const bool & orq_Value)
{
   this->mq_IsUnsigned = orq_Value;
   m_ResetMinMax();
   //Reset value
   this->SetValue(this->GetValue(), true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt value to range

   \param[in,out]  orc_Value  Value to check and adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::m_ApplyMinMax(QVariant & orc_Value) const
{
   bool q_IsUnderMinimum;
   bool q_IsOverMaximum;

   m_CheckMinMax(orc_Value, q_IsUnderMinimum, q_IsOverMaximum);
   if (q_IsUnderMinimum == true)
   {
      orc_Value = this->mc_Minimum;
   }
   else if (q_IsOverMaximum == true)
   {
      orc_Value = this->mc_Maximum;
   }
   else
   {
      //No change
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if value in range

   \param[in]   orc_Value           Value to check
   \param[out]  orq_IsUnderMinimum  Indicator if value under allowed range
   \param[out]  orq_IsOverMaximum   Indicator if value over allowed range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::m_CheckMinMax(const QVariant & orc_Value, bool & orq_IsUnderMinimum, bool & orq_IsOverMaximum) const
{
   orq_IsUnderMinimum = false;
   orq_IsOverMaximum = false;
   if (mq_IsUnsigned == true)
   {
      if (orc_Value.toULongLong() < this->mc_Minimum.toULongLong())
      {
         orq_IsUnderMinimum = true;
      }
      if (orc_Value.toULongLong() > this->mc_Maximum.toULongLong())
      {
         orq_IsOverMaximum = true;
      }
   }
   else
   {
      if (orc_Value.toLongLong() < this->mc_Minimum.toLongLong())
      {
         orq_IsUnderMinimum = true;
      }
      if (orc_Value.toLongLong() > this->mc_Maximum.toLongLong())
      {
         orq_IsOverMaximum = true;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset min and max value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64::m_ResetMinMax(void)
{
   //Init default min max and default value
   if (this->mq_IsUnsigned == true)
   {
      //lint -e{530,10,1015,1013}  c++11 feature
      this->mc_Minimum = std::numeric_limits<uint64>::lowest();
      this->mc_Maximum = std::numeric_limits<uint64>::max();
   }
   else
   {
      //lint -e{530,10,1015,1013}  c++11 feature
      this->mc_Minimum = std::numeric_limits<sint64>::lowest();
      this->mc_Maximum = std::numeric_limits<sint64>::max();
   }
   Q_EMIT this->SigMinMaxChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value in expected internal storage format

   \param[in]  orc_Value   Numeric QVariant

   \return
   QVariant in expected format
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_OgeSpxInt64::m_PrepareValue(const QVariant & orc_Value) const
{
   QVariant c_Retval;

   switch (orc_Value.type())
   {
   case QVariant::ULongLong:
      if (this->mq_IsUnsigned == true)
      {
         c_Retval = static_cast<uint64>(orc_Value.toULongLong());
      }
      else
      {
         c_Retval = static_cast<sint64>(orc_Value.toULongLong());
      }
      break;
   case QVariant::UInt:
      if (this->mq_IsUnsigned == true)
      {
         c_Retval = static_cast<uint64>(orc_Value.toUInt());
      }
      else
      {
         c_Retval = static_cast<sint64>(orc_Value.toUInt());
      }
      break;
   case QVariant::LongLong:
      if (this->mq_IsUnsigned == true)
      {
         c_Retval = static_cast<uint64>(orc_Value.toLongLong());
      }
      else
      {
         c_Retval = static_cast<sint64>(orc_Value.toLongLong());
      }
      break;
   case QVariant::Int:
      if (this->mq_IsUnsigned == true)
      {
         c_Retval = static_cast<uint64>(static_cast<sint64>(orc_Value.toInt()));
      }
      else
      {
         c_Retval = static_cast<sint64>(orc_Value.toInt());
      }
      break;
   case QVariant::Double:
      if (this->mq_IsUnsigned == true)
      {
         c_Retval = static_cast<uint64>(orc_Value.toDouble());
      }
      else
      {
         c_Retval = static_cast<sint64>(orc_Value.toDouble());
      }
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_Retval;
}
