//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic line edit for table edit (implementation)

   Generic line edit for table edit

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_TblEditLineEditBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblEditLineEditBase::C_TblEditLineEditBase(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   me_Type(QVariant::Invalid)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize from value

   Warning: this also prepares the output value type

   \param[in] orc_DisplayValue Display value
   \param[in] orc_EditValue    Edit value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblEditLineEditBase::SetFromVariant(const QVariant & orc_DisplayValue, const QVariant & orc_EditValue)
{
   this->me_Type = orc_EditValue.type();
   this->setText(orc_DisplayValue.toString());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set minimum value from variant

   \param[in] orc_Value Edit value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblEditLineEditBase::SetMinFromVariant(const QVariant & orc_Value)
{
   this->mc_MinValue = orc_Value.toString();
   m_UpdateToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set maximum value from variant

   \param[in] orc_Value Edit value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblEditLineEditBase::SetMaxFromVariant(const QVariant & orc_Value)
{
   this->mc_MaxValue = orc_Value.toString();
   m_UpdateToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get value as variant

   Expected: same value type as input

   \param[out] orc_Value            Current value (invalid if error)
   \param[out] orc_ErrorDescription Error description

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_TblEditLineEditBase::GetValueAsVariant(QVariant & orc_Value, QString & orc_ErrorDescription) const
{
   const QString c_ErrorDescriptionInt = C_GtGetText::h_GetText(
      "Only integers and hexadecimal numbers (leading \"0x\") are allowed.");
   const QString c_ErrorDescriptionFloat = C_GtGetText::h_GetText(
      "Only floating point numbers and integers are allowed.");
   int32_t s32_Retval = C_NO_ERR;
   bool q_Worked = false;
   uint32_t u32_Base = 10;

   // handle base (hex/dec) by hand because QString::toInt(&q_Worked, 0) and Co. also handles octal what we do not want
   if (this->text().startsWith("0x"))
   {
      u32_Base = 16;
   }

   switch (this->me_Type) //lint !e788 //not all cases handled here explicitly
   {
   case QVariant::Int:
      orc_Value = this->text().toInt(&q_Worked, u32_Base);
      orc_ErrorDescription = c_ErrorDescriptionInt;
      break;
   case QVariant::UInt:
      orc_Value = this->text().toUInt(&q_Worked, u32_Base);
      orc_ErrorDescription = c_ErrorDescriptionInt;
      break;
   case QVariant::LongLong:
      orc_Value = this->text().toLongLong(&q_Worked, u32_Base);
      orc_ErrorDescription = c_ErrorDescriptionInt;
      break;
   case QVariant::ULongLong:
      orc_Value = this->text().toULongLong(&q_Worked, u32_Base);
      orc_ErrorDescription = c_ErrorDescriptionInt;
      break;
   case QVariant::Double:
      orc_Value = this->text().toDouble(&q_Worked);
      orc_ErrorDescription = c_ErrorDescriptionFloat;
      break;
   case QVariant::String:
      orc_Value = this->text();
      q_Worked = true;
      break;
   default:
      //Not supported
      break;
   }
   if (q_Worked == false)
   {
      s32_Retval = C_CONFIG;
   }
   else
   {
      //Range checks
      //for range checks only the floating point conversion should be relevant
      const float64_t f64_Value = C_TblEditLineEditBase::mh_GetStringAsFloat(this->text(), q_Worked);
      if (q_Worked == true)
      {
         if (this->mc_MinValue.isEmpty() == false)
         {
            const float64_t f64_MinValue = C_TblEditLineEditBase::mh_GetStringAsFloat(this->mc_MinValue, q_Worked);
            if (q_Worked == true)
            {
               if (f64_Value < f64_MinValue)
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
         if (this->mc_MaxValue.isEmpty() == false)
         {
            const float64_t f64_MaxValue = C_TblEditLineEditBase::mh_GetStringAsFloat(this->mc_MaxValue, q_Worked);
            if (q_Worked == true)
            {
               if (f64_MaxValue < f64_Value)
               {
                  s32_Retval = C_CONFIG;
               }
            }
         }
      }
   }
   //Range info
   if (this->mc_MinValue.isEmpty() == false)
   {
      if (orc_ErrorDescription.isEmpty() == false)
      {
         orc_ErrorDescription += "\n";
      }
      orc_ErrorDescription += static_cast<QString>(C_GtGetText::h_GetText("Minimum: %1")).arg(this->mc_MinValue);
   }
   if (this->mc_MaxValue.isEmpty() == false)
   {
      if (orc_ErrorDescription.isEmpty() == false)
      {
         orc_ErrorDescription += "\n";
      }
      orc_ErrorDescription += static_cast<QString>(C_GtGetText::h_GetText("Maximum: %1")).arg(this->mc_MaxValue);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update tooltip (for min max only!)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblEditLineEditBase::m_UpdateToolTip(void)
{
   if ((this->mc_MinValue.isEmpty() == false) || (this->mc_MaxValue.isEmpty() == false))
   {
      const QString c_Heading = C_GtGetText::h_GetText("Value Range");
      QString c_Content = "";
      if (this->mc_MinValue.isEmpty() == false)
      {
         c_Content += static_cast<QString>(C_GtGetText::h_GetText("Minimum: %1")).arg(this->mc_MinValue);
      }
      if (this->mc_MaxValue.isEmpty() == false)
      {
         if (c_Content.isEmpty() == false)
         {
            c_Content += "\n";
         }
         c_Content += static_cast<QString>(C_GtGetText::h_GetText("Maximum: %1")).arg(this->mc_MaxValue);
      }
      this->SetToolTipInformation(c_Heading, c_Content);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get string as float value (if possible)

   \param[in]  orc_Value  Value to transform
   \param[out] orq_Worked Output for error reporting

   \return
   Value as float if it worked
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_TblEditLineEditBase::mh_GetStringAsFloat(const QString & orc_Value, bool & orq_Worked)
{
   float64_t f64_Retval = orc_Value.toDouble(&orq_Worked);

   if (orq_Worked == false)
   {
      f64_Retval = static_cast<float64_t>(orc_Value.toULongLong(&orq_Worked, 0));
      if (orq_Worked == false)
      {
         f64_Retval = static_cast<float64_t>(orc_Value.toLongLong(&orq_Worked, 0));
      }
   }
   return f64_Retval;
}
