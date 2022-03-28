//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBELEMENTIDCRCGROUP_H
#define C_PUISVDBELEMENTIDCRCGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbElementIdCRCGroup
{
public:
   C_PuiSvDbElementIdCRCGroup();

   stw_types::sint32 UpdateCRC(void);
   stw_types::sint32 CheckCRC(void) const;

   stw_types::uint32 GetCRC(void) const;
   void SetCRC(const stw_types::uint32 ou32_CRC);

   const C_PuiSvDbNodeDataPoolListElementId & GetElementId(void) const;
   void SetElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_Value);

private:
   C_PuiSvDbNodeDataPoolListElementId mc_ElementId;
   stw_types::uint32 mu32_Crc;

   stw_types::sint32 m_CalcCRC(stw_types::uint32 & oru32_Result) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
