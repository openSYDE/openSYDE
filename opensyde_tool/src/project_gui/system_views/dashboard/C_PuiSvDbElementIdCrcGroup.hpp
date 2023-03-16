//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBELEMENTIDCRCGROUP_HPP
#define C_PUISVDBELEMENTIDCRCGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbElementIdCrcGroup
{
public:
   C_PuiSvDbElementIdCrcGroup();

   int32_t UpdateCrc(void);
   int32_t CheckCrc(void) const;

   uint32_t GetCrc(void) const;
   void SetCrc(const uint32_t ou32_Crc);

   const C_PuiSvDbNodeDataPoolListElementId & GetElementId(void) const;
   void SetElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_Value);

private:
   C_PuiSvDbNodeDataPoolListElementId mc_ElementId;
   uint32_t mu32_Crc;

   int32_t m_CalcCrc(uint32_t & oru32_Result) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
