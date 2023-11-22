//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus protocol for specific node base class for connect and disconnect commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOBUSPROTNODECONNECTDISCONNECTBASECOMMAND_HPP
#define C_SDBUEUNOBUSPROTNODECONNECTDISCONNECTBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoCommand>
#include "stwtypes.hpp"

#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand :
   public QUndoCommand
{
public:
   C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                     const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                                     QWidget * const opc_Widget, const QString & orc_Text,
                                                     QUndoCommand * const opc_Parent);

protected:
   void m_DoConnectNodeToProt(void);
   void m_DoConnectNodeToProtAndAddDataPool(void);
   void m_DoDisconnectNodeFromProt(void);
   void m_DoDisconnectNodeToProtAndDeleteDataPool(void);

private:
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_InterfaceIndex;
   const stw::opensyde_core::C_OscCanProtocol::E_Type me_Protocol;
   QWidget * const mpc_Widget;

   void m_SetComProtocolUsedByInterfaceFlag(const bool oq_Flag) const;
   int32_t m_AddDataPool(void) const;
   void m_DeleteDataPool(void) const;
   void m_UpdateUi(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
