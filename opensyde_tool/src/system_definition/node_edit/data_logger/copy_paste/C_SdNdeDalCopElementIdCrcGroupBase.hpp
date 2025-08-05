//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for groups of element IDs with their CRC
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALCOPELEMENTIDCRCGROUPBASE_H
#define C_SDNDEDALCOPELEMENTIDCRCGROUPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalCopElementIdCrcGroupBase
{
public:
   C_SdNdeDalCopElementIdCrcGroupBase();
   virtual ~C_SdNdeDalCopElementIdCrcGroupBase(void);

   int32_t UpdateCrc(void);
   int32_t CheckCrc(void) const;

   uint32_t GetCrc(void) const;
   void SetCrc(const uint32_t ou32_Crc);

protected:
   uint32_t mu32_Crc;

   virtual int32_t m_CalcCrc(uint32_t & oru32_Result) const = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
