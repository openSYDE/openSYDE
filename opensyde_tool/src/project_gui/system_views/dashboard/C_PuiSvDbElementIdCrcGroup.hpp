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
#include "C_SdNdeDalCopElementIdCrcGroupBase.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbElementIdCrcGroup :
   public C_SdNdeDalCopElementIdCrcGroupBase
{
public:
   C_PuiSvDbElementIdCrcGroup();
   ~C_PuiSvDbElementIdCrcGroup() noexcept override;

   const C_PuiSvDbNodeDataPoolListElementId & GetElementId(void) const;
   void SetElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_Value);

protected:
   int32_t m_CalcCrc(uint32_t & oru32_Result) const override;

private:
   C_PuiSvDbNodeDataPoolListElementId mc_ElementId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
