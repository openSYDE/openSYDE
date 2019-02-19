//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text element for system view bus (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVTEXTELEMENTBUS_H
#define C_GISVTEXTELEMENTBUS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiTextElementBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvTextElementBus :
   public C_GiTextElementBus
{
public:
   C_GiSvTextElementBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                        QGraphicsItem * const opc_Parent = NULL);
   virtual void GenerateHint(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
