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

#include "C_SebUnoBaseCommand.hpp"
#include "C_PuiSdNodeInterfaceAutomaticProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyChangeInterfaceCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SdManUnoTopologyChangeInterfaceCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                            const uint8_t & oru8_PreviousInterface, const uint8_t & oru8_NewInterface,
                                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_PreviousProperties,
                                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_NewProperties,
                                            QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyChangeInterfaceCommand(void);
   void undo(void) override;
   void redo(void) override;

private:
   const uint8_t mu8_PreviousInterface;
   const uint8_t mu8_NewInterface;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_PreviousProperties;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_NewProperties;

   void m_ChangeInterface(const uint8_t & oru8_NewInterface,
                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
