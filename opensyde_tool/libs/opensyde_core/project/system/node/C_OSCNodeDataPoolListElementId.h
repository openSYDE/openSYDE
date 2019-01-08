//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element stored in a node (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
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

   virtual bool operator < (const C_OSCNodeDataPoolListId & orc_Cmp) const;
   virtual bool operator == (const C_OSCNodeDataPoolListId & orc_Cmp) const;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint32 u32_ElementIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
