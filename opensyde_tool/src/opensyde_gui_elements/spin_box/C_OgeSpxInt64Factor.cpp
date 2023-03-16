//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int64 spin box which should avoid a zero value (implementation)

   Int64 spin box which should avoid a zero value

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OgeSpxInt64Factor.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out]  opc_Parent       Optional pointer to parent
   \param[in]      orq_IsUnsigned   Indicator if this spin box should handle unsigned 64 bit or signed 64 bit
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxInt64Factor::C_OgeSpxInt64Factor(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   C_OgeSpxInt64ToolTipBase(opc_Parent, orq_IsUnsigned)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interpret current value and fix if necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64Factor::InterpretValue(void)
{
   if (this->GetIsUnsigned() == true)
   {
      if (this->GetValue().toULongLong() == 0ULL)
      {
         this->SetValue(1ULL, true);
      }
   }
   else
   {
      if (this->GetValue().toLongLong() == 0LL)
      {
         this->SetValue(1ULL, true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten step by event slot

   Here: Skip 0

   \param[in]  os32_Steps   Steps
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxInt64Factor::stepBy(const int32_t os32_Steps)
{
   C_OgeSpxInt64::stepBy(os32_Steps);
   if (this->GetIsUnsigned() == true)
   {
      if (this->GetValue().toULongLong() == 0ULL)
      {
         if (os32_Steps != 0)
         {
            this->SetValue(1ULL, true);
         }
         else
         {
            //No step
         }
      }
   }
   else
   {
      if (this->GetValue().toLongLong() == 0LL)
      {
         if (os32_Steps > 0)
         {
            this->SetValue(1LL, true);
         }
         else if (os32_Steps < 0)
         {
            this->SetValue(-1LL, true);
         }
         else
         {
            //No step
         }
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
QValidator::State C_OgeSpxInt64Factor::validate(QString & orc_Text, int32_t & ors32_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxInt64::validate(orc_Text, ors32_Pos);
   if (this->GetIsUnsigned() == true)
   {
      if (this->GetValue().toULongLong() == 0ULL)
      {
         e_Retval = QValidator::State::Intermediate;
      }
   }
   else
   {
      if (this->GetValue().toLongLong() == 0LL)
      {
         e_Retval = QValidator::State::Intermediate;
      }
   }
   return e_Retval;
}
