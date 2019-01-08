//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Disconnect node from protocol undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEUNOBUSPROTNODEDISCONNECTCOMMAND_H
#define C_SDBUEUNOBUSPROTNODEDISCONNECTCOMMAND_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueUnoBusProtNodeDisconnectCommand :
   public C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand
{
public:
   C_SdBueUnoBusProtNodeDisconnectCommand(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_types::uint32 ou32_InterfaceIndex,
                                          const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol,
                                          QWidget * const opc_Widget, QUndoCommand * const opc_Parent = NULL);

   virtual void redo(void);
   virtual void undo(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
