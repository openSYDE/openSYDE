//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Bus protocol for specific node base class for connect and disconnect commands (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEUNOBUSPROTNODECONNECTDISCONNECTBASECOMMAND_H
#define C_SDBUEUNOBUSPROTNODECONNECTDISCONNECTBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include <QUndoCommand>
#include "stwtypes.h"

#include "C_OSCCanProtocol.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand :
   public QUndoCommand
{
public:
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_InterfaceIndex,
                                                     const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol,
                                                     QWidget * const opc_Widget, const QString & orc_Text,
                                                     QUndoCommand * const opc_Parent);

protected:
   void m_DoConnectNodeToProt(void);
   void m_DoConnectNodeToProtAndAddDataPool(void);
   void m_DoDisconnectNodeFromProt(void);
   void m_DoDisconnectNodeToProtAndDeleteDataPool(void);
   void m_DoAddDataPool(void);
   void m_DoDeleteDataPool(void);

private:
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_InterfaceIndex;
   const stw_opensyde_core::C_OSCCanProtocol::E_Type me_Protocol;
   QWidget * const mpc_Widget;

   void m_SetComProtocolUsedByInterfaceFlag(const bool oq_Flag) const;
   stw_types::sint32 m_AddDataPool(void) const;
   void m_DeleteDataPool(void) const;
   void m_UpdateUi(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
