//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bitmask item
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCONTENTBITMASKITEM_H
#define C_OSCHALCDEFCONTENTBITMASKITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefContentBitmaskItem
{
public:
   C_OscHalcDefContentBitmaskItem(void);
   virtual ~C_OscHalcDefContentBitmaskItem();

   int32_t SetValueByString(const stw::scl::C_SclString & orc_Item);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   virtual void CalcHashStructure(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Display; ///< Name to display to user
   stw::scl::C_SclString c_Comment; ///< Description to display to user
   bool q_ApplyValueSetting;        ///< Current setting to apply this bitmask value
   uint64_t u64_Value;              ///< Bitmask value

private:
   static int32_t mh_ParseUintFromString(const stw::scl::C_SclString & orc_Item, uint64_t & oru64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
