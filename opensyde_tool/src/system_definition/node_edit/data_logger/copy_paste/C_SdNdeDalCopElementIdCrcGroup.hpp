//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALCOPELEMENTIDCRCGROUP_H
#define C_SDNDEDALCOPELEMENTIDCRCGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDalCopElementIdCrcGroupBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalCopElementIdCrcGroup :
   public C_SdNdeDalCopElementIdCrcGroupBase
{
public:
   C_SdNdeDalCopElementIdCrcGroup();
   ~C_SdNdeDalCopElementIdCrcGroup() noexcept override;

   const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & GetElementId(void) const;
   void SetElementId(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Value);

protected:
   int32_t m_CalcCrc(uint32_t & oru32_Result) const override;

private:
   stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId mc_ElementId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
