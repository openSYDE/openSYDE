//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard chart data (implementation)

   UI class for dashboard chart data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_PuiSvDbChart.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbChart::C_PuiSvDbChart() :
   C_PuiSvDbWidgetBase(),
   e_SettingZoomMode(C_PuiSvDbChart::eSETTING_ZM_XY)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbChart::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItActive = 0; u32_ItActive < this->c_DataPoolElementsActive.size(); ++u32_ItActive)
   {
      const bool q_Data = this->c_DataPoolElementsActive[u32_ItActive];
      stw_scl::C_SCLChecksums::CalcCRC32(&q_Data, sizeof(q_Data), oru32_HashValue);
   }
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_SettingZoomMode, sizeof(this->e_SettingZoomMode), oru32_HashValue);

   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbChart::IsReadElement(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove element from widget

   \param[in] oru32_Index Internal index

   \retval   C_NO_ERR   Index found
   \retval   C_RANGE    Index not found
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_PuiSvDbChart::RemoveElement(const stw_types::uint32 & oru32_Index)
{
   stw_types::sint32 s32_Retval = C_PuiSvDbWidgetBase::RemoveElement(oru32_Index);
   if (s32_Retval == C_NO_ERR)
   {
      if (oru32_Index < this->c_DataPoolElementsActive.size())
      {
         this->c_DataPoolElementsActive.erase(this->c_DataPoolElementsActive.begin() + oru32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}
