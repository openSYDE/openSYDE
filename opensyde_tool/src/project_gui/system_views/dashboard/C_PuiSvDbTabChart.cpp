//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard tab chart data (implementation)

   UI class for dashboard tab chart data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvDbTabChart.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;

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
C_PuiSvDbTabChart::C_PuiSvDbTabChart() :
   C_PuiSvDbWidgetBase(),
   s32_SplitterLeftWidth(300),
   e_SettingZoomMode(C_PuiSvDbTabChart::eSETTING_ZM_XY),
   e_SettingVerticalAxisMode(C_PuiSvDbTabChart::eSETTING_YA_ONE_VISIBLE),
   q_IsZoomModeActive(false),
   q_IsPaused(false),
   q_AreSamplePointsShown(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbTabChart::CalcHash(uint32_t & oru32_HashValue) const
{
   for (uint32_t u32_ItActive = 0; u32_ItActive < this->c_DataPoolElementsActive.size(); ++u32_ItActive)
   {
      const bool q_Data = this->c_DataPoolElementsActive[u32_ItActive];
      stw::scl::C_SclChecksums::CalcCRC32(&q_Data, sizeof(q_Data), oru32_HashValue);
   }
   for (uint32_t u32_ItActive = 0; u32_ItActive < this->c_DataPoolElementsColorIndex.size(); ++u32_ItActive)
   {
      const uint8_t u8_Data = this->c_DataPoolElementsColorIndex[u32_ItActive];
      stw::scl::C_SclChecksums::CalcCRC32(&u8_Data, sizeof(u8_Data), oru32_HashValue);
   }
   for (uint32_t u32_ItOuter = 0; u32_ItOuter < this->c_VisibleScreen.size(); ++u32_ItOuter)
   {
      const std::array<float64_t, 4> & orc_Inner = this->c_VisibleScreen[u32_ItOuter];
      for (uint32_t u32_ItInner = 0; u32_ItInner < orc_Inner.size(); ++u32_ItInner)
      {
         //lint -e{736,9120} C++ interface
         const float32_t f32_Val = orc_Inner[u32_ItInner];
         //lint -e{9110} Usual way
         stw::scl::C_SclChecksums::CalcCRC32(&f32_Val, sizeof(f32_Val), oru32_HashValue);
      }
   }
   stw::scl::C_SclChecksums::CalcCRC32(&this->s32_SplitterLeftWidth, sizeof(this->s32_SplitterLeftWidth),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_SettingZoomMode, sizeof(this->e_SettingZoomMode), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsZoomModeActive, sizeof(this->q_IsZoomModeActive), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_SettingVerticalAxisMode, sizeof(this->e_SettingVerticalAxisMode),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsPaused, sizeof(this->q_IsPaused), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_AreSamplePointsShown, sizeof(this->q_AreSamplePointsShown),
                                       oru32_HashValue);

   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbTabChart::IsReadElement(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove element from widget

   \param[in]  oru32_Index    Internal index

   \retval   C_NO_ERR   Index found
   \retval   C_RANGE    Index not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbTabChart::RemoveElement(const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_PuiSvDbWidgetBase::RemoveElement(oru32_Index);

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

      if (oru32_Index < this->c_DataPoolElementsColorIndex.size())
      {
         this->c_DataPoolElementsColorIndex.erase(this->c_DataPoolElementsColorIndex.begin() + oru32_Index);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   return s32_Retval;
}
