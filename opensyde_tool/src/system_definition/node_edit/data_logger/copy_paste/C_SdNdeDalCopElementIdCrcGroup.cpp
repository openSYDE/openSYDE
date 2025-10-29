//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC

   Storage class to group element IDs with their CRC

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDalCopElementIdCrcGroup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
C_SdNdeDalCopElementIdCrcGroup::C_SdNdeDalCopElementIdCrcGroup() :
   C_SdNdeDalCopElementIdCrcGroupBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalCopElementIdCrcGroup::~C_SdNdeDalCopElementIdCrcGroup() noexcept
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get element id

   \return
   Element id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolListElementOptArrayId & C_SdNdeDalCopElementIdCrcGroup::GetElementId() const
{
   return this->mc_ElementId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set element id

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopElementIdCrcGroup::SetElementId(const C_OscNodeDataPoolListElementOptArrayId & orc_Value)
{
   this->mc_ElementId = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc CRC

   \param[out]  oru32_Result  Result

   \return
   STW error codes

   \retval   C_NO_ERR   Checksum calculated
   \retval   C_CONFIG   ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopElementIdCrcGroup::m_CalcCrc(uint32_t & oru32_Result) const
{
   const int32_t s32_Retval = mh_CalcCrcDataPoolElement(oru32_Result, this->mc_ElementId);

   return s32_Retval;
}
