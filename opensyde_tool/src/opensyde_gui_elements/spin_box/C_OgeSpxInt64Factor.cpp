//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Int64 spin box which should avoid a zero value (implementation)

   Int64 spin box which should avoid a zero value

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OgeSpxInt64Factor.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent     Optional pointer to parent
   \param[in]     orq_IsUnsigned Indicator if this spin box should handle unsigned 64 bit or signed 64 bit

   \created     08.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxInt64Factor::C_OgeSpxInt64Factor(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   C_OgeSpxInt64ToolTipBase(opc_Parent, orq_IsUnsigned)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Interpret current value and fix if necessary

   \created     08.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten step by event slot

   Here: Skip 0

   \param[in] osn_Steps Steps

   \created     08.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64Factor::stepBy(const sintn osn_Steps)
{
   C_OgeSpxInt64::stepBy(osn_Steps);
   if (this->GetIsUnsigned() == true)
   {
      if (this->GetValue().toULongLong() == 0ULL)
      {
         if (osn_Steps != 0)
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
         if (osn_Steps > 0)
         {
            this->SetValue(1LL, true);
         }
         else if (osn_Steps < 0)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten validate event slot

   Here: Skip 0

   \param[in] orc_Text Text
   \param[in] orsn_Pos Pos

   \created     08.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QValidator::State C_OgeSpxInt64Factor::validate(QString & orc_Text, sintn & orsn_Pos) const
{
   QValidator::State e_Retval = C_OgeSpxInt64::validate(orc_Text, orsn_Pos);
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
