//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for dashboard widget data (implementation)

   UI base class for dashboard widget data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbWidgetBase::C_PuiSvDbWidgetBase(void) :
   C_PuiBsBox(),
   e_DisplayStyle(eOPENSYDE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbWidgetBase::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_DataPoolElementsConfig.size(); ++u32_Counter)
   {
      this->c_DataPoolElementsConfig[u32_Counter].CalcHash(oru32_HashValue);
   }
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_DisplayStyle, sizeof(this->e_DisplayStyle), oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current index for specified element ID

   \param[in]   orc_Id        Data element id
   \param[out]  oru32_Index   Data element index

   \return
   C_NO_ERR Data element id found
   C_RANGE  Data element id not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbWidgetBase::GetIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id, uint32_t & oru32_Index) const
{
   int32_t s32_Retval = C_RANGE;

   for (uint32_t u32_It = 0; u32_It < this->c_DataPoolElementsConfig.size(); ++u32_It)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = this->c_DataPoolElementsConfig[u32_It];
      if (rc_Config.c_ElementId == orc_Id)
      {
         s32_Retval = C_NO_ERR;
         oru32_Index = u32_It;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove element from widget

   \param[in]  oru32_Index    Internal index

   \retval   C_NO_ERR   Index found
   \retval   C_RANGE    Index not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvDbWidgetBase::RemoveElement(const uint32_t & oru32_Index)
{
   int32_t s32_Retval = C_NO_ERR;

   if (oru32_Index < this->c_DataPoolElementsConfig.size())
   {
      this->c_DataPoolElementsConfig.erase(this->c_DataPoolElementsConfig.begin() + oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all references to element id

   \param[in]  orc_DataElementId    Data element id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbWidgetBase::RemoveAllReferencesToElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId)
{
   for (uint32_t u32_It = 0; u32_It < this->c_DataPoolElementsConfig.size();)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = this->c_DataPoolElementsConfig[u32_It];
      if (rc_Config.c_ElementId == orc_DataElementId)
      {
         tgl_assert(this->RemoveElement(u32_It) == C_NO_ERR);
      }
      else
      {
         ++u32_It;
      }
   }
}
