//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool list ID (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTID_H
#define C_OSCNODEDATAPOOLLISTID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_OSCNodeDataPoolId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeDataPoolListId :
   public C_OSCNodeDataPoolId
{
public:
   C_OSCNodeDataPoolListId(void);
   C_OSCNodeDataPoolListId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                           const stw_types::uint32 ou32_ListIndex);

   virtual bool operator < (const C_OSCNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OSCNodeDataPoolId & orc_Cmp) const;

   stw_types::uint32 u32_ListIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
