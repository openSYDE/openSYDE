//-----------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYCHANGEINTERFACECOMMAND_H
#define C_SDMANUNOTOPOLOGYCHANGEINTERFACECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SebUnoBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdManUnoTopologyChangeInterfaceCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SdManUnoTopologyChangeInterfaceCommand(QGraphicsScene * const opc_Scene,
                                            const std::vector<stw_types::uint64> & orc_IDs,
                                            const stw_types::uint8 & oru8_PreviousInterface,
                                            const stw_types::uint8 & oru8_NewInterface,
                                            const stw_types::uint8 & oru8_PreviousNodeId,
                                            const stw_types::uint8 & oru8_NewNodeId,
                                            QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyChangeInterfaceCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const stw_types::uint8 mu8_PreviousInterface;
   const stw_types::uint8 mu8_NewInterface;
   const stw_types::uint8 mu8_PreviousNodeId;
   const stw_types::uint8 mu8_NewNodeId;

   void m_ChangeInterface(const stw_types::uint8 & oru8_NewInterface, const stw_types::uint8 & oru8_NodeId);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
