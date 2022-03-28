//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYCHANGEINTERFACECOMMAND_H
#define C_SDMANUNOTOPOLOGYCHANGEINTERFACECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"
#include "C_PuiSdNodeInterfaceAutomaticProperties.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyChangeInterfaceCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SdManUnoTopologyChangeInterfaceCommand(QGraphicsScene * const opc_Scene,
                                            const std::vector<stw_types::uint64> & orc_IDs,
                                            const stw_types::uint8 & oru8_PreviousInterface,
                                            const stw_types::uint8 & oru8_NewInterface,
                                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_PreviousProperties,
                                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_NewProperties,
                                            QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyChangeInterfaceCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   const stw_types::uint8 mu8_PreviousInterface;
   const stw_types::uint8 mu8_NewInterface;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_PreviousProperties;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_NewProperties;

   void m_ChangeInterface(const stw_types::uint8 & oru8_NewInterface,
                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
