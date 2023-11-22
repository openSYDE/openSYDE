//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for reconnect bus connector (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYRECONNECTBASECOMMAND_HPP
#define C_SDMANUNOTOPOLOGYRECONNECTBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.hpp"
#include "C_GiLiBusConnector.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyReconnectBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SdManUnoTopologyReconnectBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                          const uint64_t & oru64_StartingItemId, const uint64_t & oru64_LastItemId,
                                          const QPointF & orc_ConnectionPos, const int32_t & ors32_Interface,
                                          const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                          const QString & orc_Description, QUndoCommand * const opc_Parent = NULL);
   ~C_SdManUnoTopologyReconnectBaseCommand(void) override;
   void undo(void) override;
   void redo(void) override;

protected:
   const uint64_t mu64_StartingItemId;
   const uint64_t mu64_LastItemId;
   const QPointF mc_ConnectionPos;
   //TODO: Save initial position (If this command ever needs to be undone) -> also add to m_Reconnect interface
   const int32_t ms32_Interface;
   uint8_t mu8_InitialInterface;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_Properties;
   std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_InitialProperties;

   stw::opensyde_gui::C_GiLiBusConnector * m_GetBusConnector(void) const;
   virtual void m_Reconnect(const uint64_t & oru64_StartingId, const uint64_t & oru64_LastId,
                            const int32_t & ors32_Interface,
                            const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties) = 0;

private:
   bool mq_Merged;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
