//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for identifying data temporarily (implementation)

   Class for identifying data temporarily

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_PuiBsTemporaryDataId.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  ors32_Type     Item type, warning: only casted enum values accepted
   \param[in]  oru32_Index    Index
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsTemporaryDataId::C_PuiBsTemporaryDataId(const int32_t & ors32_Type, const uint32_t & oru32_Index) :
   s32_Type(ors32_Type),
   u32_Index(oru32_Index)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare two temporary data items

   \param[in]  orc_Other   Other element

   \return
   Comparison result
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiBsTemporaryDataId::operator <(const C_PuiBsTemporaryDataId & orc_Other) const
{
   bool q_Retval;

   if (this->s32_Type == orc_Other.s32_Type)
   {
      q_Retval = this->u32_Index < orc_Other.u32_Index;
   }
   else
   {
      q_Retval = this->s32_Type < orc_Other.s32_Type;
   }
   return q_Retval;
}
