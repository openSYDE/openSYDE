//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Snapshot of system definition items (implementation)

   Snapshot of system definition items

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SdTopologyDataSnapshot.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     23.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdTopologyDataSnapshot::C_SdTopologyDataSnapshot(void) :
   C_PuiBsElements()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdTopologyDataSnapshot::~C_SdTopologyDataSnapshot(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear all data storage

   \created     23.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdTopologyDataSnapshot::Clear(void)
{
   this->c_Boundaries.clear();
   this->c_Images.clear();
   this->c_LineArrows.clear();
   this->c_TextElements.clear();
   this->c_UINodes.clear();
   this->c_UIBuses.clear();
   this->c_OSCNodes.clear();
   this->c_OSCBuses.clear();
   this->c_BusConnections.clear();
   this->c_BusTextElements.clear();
   C_PuiBsElements::Clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Count sum of all items

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdTopologyDataSnapshot::Count(void) const
{
   uint32 u32_Retval = 0;

   u32_Retval += C_PuiBsElements::Count();
   u32_Retval += this->c_UINodes.size();
   for (uint32 u32_ItNode = 0; u32_ItNode < this->c_UINodes.size(); ++u32_ItNode)
   {
      const C_PuiSdNode & rc_UINode = this->c_UINodes[u32_ItNode];
      u32_Retval += rc_UINode.c_UIBusConnections.size();
   }
   u32_Retval += this->c_UIBuses.size();
   u32_Retval += this->c_BusConnections.size();
   u32_Retval += this->c_BusTextElements.size();

   return u32_Retval;
}
