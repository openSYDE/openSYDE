//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data pool UI part (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDNODEDATAPOOL_H
#define C_PUISDNODEDATAPOOL_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>

#include "stwtypes.h"

#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdNodeDataPool
{
public:
   C_PuiSdNodeDataPool(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   std::vector<C_PuiSdNodeDataPoolList> c_DataPoolLists;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
