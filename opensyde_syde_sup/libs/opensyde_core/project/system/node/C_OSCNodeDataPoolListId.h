//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool list ID (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTID_H
#define C_OSCNODEDATAPOOLLISTID_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPoolListId
{
public:
   C_OSCNodeDataPoolListId(void);
   C_OSCNodeDataPoolListId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                           const stw_types::uint32 ou32_ListIndex);

   virtual bool operator < (const C_OSCNodeDataPoolListId & orc_Cmp) const;
   virtual bool operator == (const C_OSCNodeDataPoolListId & orc_Cmp) const;

   stw_types::uint32 u32_NodeIndex;
   stw_types::uint32 u32_DataPoolIndex;
   stw_types::uint32 u32_ListIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
