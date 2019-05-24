//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file comparison description storage
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_SyvUpParamSetFileInfoComparisonDescription.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

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
C_SyvUpParamSetFileInfoComparisonDescription::C_SyvUpParamSetFileInfoComparisonDescription(void) :
   u32_LayerNum(0UL),
   e_ResultType(eRT_NOT_FOUND)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get display text for result

   \return
   Display text for result
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpParamSetFileInfoComparisonDescription::GetResultText(void) const
{
   QString c_Retval;

   switch (this->e_ResultType)
   {
   case eRT_MATCH:
      c_Retval = C_GtGetText::h_GetText("match");
      break;
   case eRT_NO_MATCH:
      c_Retval = C_GtGetText::h_GetText("don't match");
      break;
   case eRT_NOT_FOUND:
      c_Retval = C_GtGetText::h_GetText("-");
      break;
   }

   return c_Retval;
}
