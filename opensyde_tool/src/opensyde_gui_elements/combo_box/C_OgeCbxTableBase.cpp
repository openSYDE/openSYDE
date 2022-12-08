//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with base functionality for table usage

   Combo box with base functionality for table usage

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cmath>
#include <QLineEdit>

#include "stwerrors.hpp"
#include "C_GtGetText.hpp"

#include "C_OscUtils.hpp"
#include "C_OgeCbxTableBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgeCbxTableBase::C_OgeCbxTableBase(QWidget * const opc_Parent) :
   C_OgeCbxResizingView(opc_Parent),
   mf64_Factor(1.0),
   mf64_Offset(0.0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize the values from two string lists

   \param[in]  orc_Strings    Strings to display
   \param[in]  orc_Values     Values to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTableBase::InitFromStringList(const QStringList & orc_Strings, const QStringList & orc_Values)
{
   this->mc_Values = orc_Values;
   this->mc_Strings = orc_Strings;
   for (int32_t s32_It = 0; s32_It < orc_Strings.size(); ++s32_It)
   {
      this->addItem(orc_Strings.at(s32_It));
   }
   //Default fallback
   if (this->mc_Values.size() != this->mc_Strings.size())
   {
      this->mc_Values.clear();
      this->mc_Values.reserve(this->mc_Strings.size());
      for (int32_t s32_It = 0; s32_It < this->mc_Strings.size(); ++s32_It)
      {
         this->mc_Values.push_back(QString::number(s32_It));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize minimum, maximum, factor and offset

   Only necessary if combobox is editable; else index-based handling does not need scaled value.

   \param[in]  orc_Min        Minimum
   \param[in]  orc_Max        Maximum
   \param[in]  of64_Factor    Factor
   \param[in]  of64_Offset    Offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTableBase::InitMinMaxAndScaling(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                             const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                             const float64_t of64_Factor, const float64_t of64_Offset)
{
   this->mc_UnscaledMin = orc_Min;
   this->mc_UnscaledMax = orc_Max;
   this->mf64_Factor = of64_Factor;
   this->mf64_Offset = of64_Offset;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current value

   \param[out]  ors64_Value            Current value (not the index but the value corresponding to current text)
   \param[out]  orc_ErrorDescription   Error description

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: text is no valid value. See orc_ErrorDescription for further details.
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeCbxTableBase::GetValue(int64_t & ors64_Value, QString & orc_ErrorDescription) const
{
   int32_t s32_Retval = C_CONFIG;
   QString c_CurText = this->currentText();
   bool q_Ok;
   float64_t f64_Tmp;

   // default
   ors64_Value = -1;

   // try to convert current text to number
   c_CurText.replace(QLocale::system().decimalPoint(), QLocale::c().decimalPoint(),  Qt::CaseInsensitive);
   f64_Tmp = c_CurText.toDouble(&q_Ok);

   if (q_Ok == true)
   {
      ors64_Value =
         static_cast<int64_t>(std::round(C_OscUtils::h_GetValueUnscaled(f64_Tmp, this->mf64_Factor,
                                                                        this->mf64_Offset)));
      s32_Retval = C_NO_ERR;
   }
   else
   {
      // try to find current text in combobox selection
      const int64_t s64_StringIndex = this->mc_Strings.indexOf(c_CurText);
      if ((s64_StringIndex >= 0) && (s64_StringIndex < this->mc_Values.size()))
      {
         const QString & rc_CurVal = this->mc_Values.at(static_cast<int32_t>(s64_StringIndex));
         const int64_t s64_Tmp = rc_CurVal.toLongLong(&q_Ok);
         if (q_Ok == true)
         {
            ors64_Value = s64_Tmp;
            s32_Retval = C_NO_ERR;
         }
      }
   }

   if (s32_Retval == C_CONFIG)
   {
      orc_ErrorDescription = C_GtGetText::h_GetText("Only numbers and selectable items are allowed.");
   }
   else
   {
      // check minimum and maximum (only for editable comboboxes)
      if (this->isEditable() == true)
      {
         float64_t f64_Min;
         float64_t f64_Max;
         this->mc_UnscaledMin.GetAnyValueAsFloat64(f64_Min, 0);
         this->mc_UnscaledMax.GetAnyValueAsFloat64(f64_Max, 0);
         if ((static_cast<float64_t>(ors64_Value) < f64_Min) || (static_cast<float64_t>(ors64_Value) > f64_Max))
         {
            QString c_Min;
            QString c_Max;

            c_Min = QString::number(C_OscUtils::h_GetValueScaled(f64_Min, this->mf64_Factor, this->mf64_Offset));
            c_Min.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);
            c_Max = QString::number(C_OscUtils::h_GetValueScaled(f64_Max, this->mf64_Factor, this->mf64_Offset));
            c_Max.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);

            s32_Retval = C_CONFIG;

            orc_ErrorDescription = C_GtGetText::h_GetText("Value is out of range (Minimum: ");
            orc_ErrorDescription += c_Min;
            orc_ErrorDescription += C_GtGetText::h_GetText("; Maximum: ");
            orc_ErrorDescription += c_Max;
            orc_ErrorDescription += ").";
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current index based on the specified value

   \param[in]  os64_Value  Value to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTableBase::SetValue(const int64_t os64_Value)
{
   const QString c_Search = QString::number(os64_Value);
   const int32_t s32_Index = this->mc_Values.indexOf(c_Search); // -1 if not found

   if (this->isEditable() == false)
   {
      this->setCurrentIndex(s32_Index);
   }
   else
   {
      if ((s32_Index >= 0) && (s32_Index < this->mc_Strings.size()))
      {
         this->setCurrentText(this->mc_Strings.at(s32_Index));
      }
      else
      {
         QString c_Temp =
            QString::number(C_OscUtils::h_GetValueScaled(static_cast<float64_t>(os64_Value),
                                                         this->mf64_Factor, this->mf64_Offset));
         c_Temp.replace(QLocale::c().decimalPoint(), QLocale::system().decimalPoint(), Qt::CaseInsensitive);
         this->setCurrentText(c_Temp);
      }
   }
}
