//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element stored in a node (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTELEMENTID_H
#define C_OSCNODEDATAPOOLLISTELEMENTID_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include "C_OSCNodeDataPoolListId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPoolListElementId :
   public C_OSCNodeDataPoolListId
{
public:
   C_OSCNodeDataPoolListElementId(void);
   C_OSCNodeDataPoolListElementId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                  const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_ElementIndex);

   virtual bool operator < (const C_OSCNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OSCNodeDataPoolId & orc_Cmp) const;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint32 u32_ElementIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
