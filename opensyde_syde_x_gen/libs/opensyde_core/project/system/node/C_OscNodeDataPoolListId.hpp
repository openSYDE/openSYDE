//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool list ID (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTID_HPP
#define C_OSCNODEDATAPOOLLISTID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_OscNodeDataPoolId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolListId :
   public C_OscNodeDataPoolId
{
public:
   C_OscNodeDataPoolListId(void);
   C_OscNodeDataPoolListId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                           const uint32_t ou32_ListIndex);

   virtual bool operator < (const C_OscNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OscNodeDataPoolId & orc_Cmp) const;

   uint32_t u32_ListIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
