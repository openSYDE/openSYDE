//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store display formatter settings and handle functionality

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <regex>

#include "C_SclChecksums.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiSvDbDataElementDisplayFormatter.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementDisplayFormatter::C_PuiSvDbDataElementDisplayFormatter() :
   q_IsActive(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementDisplayFormatter::~C_PuiSvDbDataElementDisplayFormatter()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc hash

   \param[out]  oru32_HashValue  Hash value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementDisplayFormatter::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsActive, sizeof(this->q_IsActive), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_FormatterString.toStdString().c_str(),
                                       this->c_FormatterString.length(), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current formatter string

   The main intention of this method is to check for a 'safe' printf expression so that no undefined behaviour occurs.
   Therefore using a specific and adapted regex for each type of category (int, float, string) following the regex of
   https://redmine.sensor-technik.de/issues/80665#note-7, except of the identifier part '(?:\\d+\\$)?' since this is not
   supported for printf as well as the double precision specifier 'b' in '[bcdeEufFgGosxX]'.
   Additionally a '%%' (prints a single %) or a '*' at the beginning and/or end of the formatter expression is valid.
   For further details to allowed parameter specifiers and necessary limitations refer to:
   https://redmine.sensor-technik.de/issues/80664

   Used Regex for integer:  ^([%%]{2})?[^%]*%[+-]?(?:[ 0]|'.{1})?-?\\d*(?:\\.\\d+)?[cdiuoxX][^%]*([%%]{2})?$
   Used Regex for float:    ^([%%]{2})?[^%]*%[+-]?(?:[ 0]|'.{1})?-?\\d*(?:\\.\\d+)?[eEfFgG][^%]*([%%]{2})?$
   Used Regex for string:   ^([%%]{2})?[^%]*%[-]?\\d*(?:\\.\\d+)?[s][^%]*([%%]{2})?$

   Regex tested with: https://regex101.com/
   Various printf expression tested with: https://www.onlinegdb.com/online_c_compiler

   \param[in]       oe_Category     Type of category

   \retval   true    c_FormatterString is compatible to the type category
   \retval   false   c_FormatterString is not compatible to the type category. Do not call any GetValue functions with
                     this type and this formatter string. The behavior is not defined!
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbDataElementDisplayFormatter::IsFormatterCompatible(
   const C_PuiSvDbDataElementDisplayFormatter::E_TypeCategory oe_Category) const
{
   bool q_RetVal = true;

   if (oe_Category == E_TypeCategory::eINTEGER)
   {
      const QByteArray c_StringToEvaluate = this->c_FormatterString.toLocal8Bit();
      const std::regex c_RegexInt("^([%%]{2})?[^%]*%[+-]?(?:[ 0]|'.{1})?-?\\d*(?:\\.\\d+)?[cdiuoxX][^%]*([%%]{2})?$");
      if (std::regex_search(c_StringToEvaluate.data(), c_RegexInt) == false)
      {
         q_RetVal = false;
      }
   }
   else if (oe_Category == E_TypeCategory::eFLOAT)
   {
      const QByteArray c_StringToEvaluate = this->c_FormatterString.toLocal8Bit();
      const std::regex c_RegexInt("^([%%]{2})?[^%]*%[+-]?(?:[ 0]|'.{1})?-?\\d*(?:\\.\\d+)?[eEfFgG][^%]*([%%]{2})?$");
      if (std::regex_search(c_StringToEvaluate.data(), c_RegexInt) == false)
      {
         q_RetVal = false;
      }
   }
   else if (oe_Category == E_TypeCategory::eSTRING)
   {
      const QByteArray c_StringToEvaluate = this->c_FormatterString.toLocal8Bit();
      const std::regex c_RegexInt("^([%%]{2})?[^%]*%[-]?\\d*(?:\\.\\d+)?[s][^%]*([%%]{2})?$");
      if (std::regex_search(c_StringToEvaluate.data(), c_RegexInt) == false)
      {
         q_RetVal = false;
      }
   }
   else
   {
      // unsupported type, should not occur
      q_RetVal = false;
   }

   return q_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Formatting the specific value with the c_FormatterString with printf formatter

   The caller is responsible for a compatible formatter string. Check with IsFormatterCompatible for the category
   eINTEGER before calling this function.

   \param[in]       os64_Value              Value to format

   \return
   Formatted value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementDisplayFormatter::GetValueFormatted(const int64_t os64_Value) const
{
   return QString::asprintf(this->c_FormatterString.toStdString().c_str(), os64_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Formatting the specific value with the c_FormatterString with printf formatter

   The caller is responsible for a compatible formatter string. Check with IsFormatterCompatible for the category
   eFLOAT before calling this function.

   \param[in]       of64_Value              Value to format

   \return
   Formatted value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementDisplayFormatter::GetValueFormatted(const float64_t of64_Value) const
{
   return QString::asprintf(this->c_FormatterString.toStdString().c_str(), of64_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Formatting the specific value with the c_FormatterString with printf formatter

   The caller is responsible for a compatible formatter string. Check with IsFormatterCompatible for the category
   eSTRING before calling this function.

   \param[in]       orc_Value               Value to format

   \return
   Formatted value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbDataElementDisplayFormatter::GetValueFormatted(const QString & orc_Value) const
{
   return QString::asprintf(this->c_FormatterString.toStdString().c_str(), orc_Value.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the category type depending of the content type, scaling configuration and string interpretation

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]       orc_Content            Datapool content with configured type
   \param[in]       orc_Scaling            Scaling configuration for Datapool element
   \param[in]       oq_InterpretedAsString Flag if array of sint8 is interpreted as string

   \return
   Detected type category
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementDisplayFormatter::E_TypeCategory C_PuiSvDbDataElementDisplayFormatter::h_GetTypeCategory(
   const C_PuiSvDbDataElementContent & orc_Content, const C_PuiSvDbDataElementScaling & orc_Scaling,
   const bool oq_InterpretedAsString)
{
   C_PuiSvDbDataElementDisplayFormatter::E_TypeCategory e_FormatterCategory;
   const C_PuiSvDbDataElementContent::E_Type e_ElementType = orc_Content.GetType();

   if ((e_ElementType == C_PuiSvDbDataElementContent::eFLOAT32) ||
       (e_ElementType == C_PuiSvDbDataElementContent::eFLOAT64) ||
       (C_OscUtils::h_IsScalingActive(orc_Scaling.f64_Factor, orc_Scaling.f64_Offset) == true))
   {
      // Float values or scaled integer values which must be handled as float
      e_FormatterCategory = C_PuiSvDbDataElementDisplayFormatter::eFLOAT;
   }
   else if (oq_InterpretedAsString == true)
   {
      e_FormatterCategory = C_PuiSvDbDataElementDisplayFormatter::eSTRING;
   }
   else
   {
      e_FormatterCategory = C_PuiSvDbDataElementDisplayFormatter::eINTEGER;
   }

   return e_FormatterCategory;
}
