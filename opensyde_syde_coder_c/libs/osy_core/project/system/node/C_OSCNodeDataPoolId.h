//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool ID (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLID_H
#define C_OSCNODEDATAPOOLID_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPoolId
{
public:
   C_OSCNodeDataPoolId(void);
   C_OSCNodeDataPoolId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);

   virtual bool operator < (const C_OSCNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OSCNodeDataPoolId & orc_Cmp) const;
   virtual bool operator != (const C_OSCNodeDataPoolId & orc_Cmp) const;

   stw_types::uint32 u32_NodeIndex;
   stw_types::uint32 u32_DataPoolIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */

} //end of namespace

#endif
