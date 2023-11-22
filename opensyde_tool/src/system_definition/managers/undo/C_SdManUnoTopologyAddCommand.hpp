//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDCOMMAND_HPP
#define C_SDMANUNOTOPOLOGYADDCOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyAddBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddCommand :
   public C_SdManUnoTopologyAddBaseCommand
{
public:
   enum E_ElementType ///< Data element type
   {
      eNODE,          ///< Data element for node
      eCAN_BUS,       ///< Data element for can bus
      eETHERNET_BUS,  ///< Data element for bus
      eLINE_ARROW,    ///< Data element for line arrow
      eBOUNDARY,      ///< Data element for boundary
      eTEXT_ELEMENT,  ///< Data element for text element
      eIMAGE,         ///< Data element for image element
      eBUS_CONNECTOR, ///< Data element for bus connector
      eUNKNOWN
   };

   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const QString & orc_AdditionalInformation = "", QUndoCommand * const opc_Parent = NULL,
                                const bool & orq_ForceUseAdditionalInformation = false);
   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const uint64_t ou64_BusConnectorNodeId = 0, const uint64_t ou64_BusConnectorBusId = 0,
                                const uint8_t ou8_InterfaceNumber = 0,
                                const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties = std::vector<C_PuiSdNodeInterfaceAutomaticProperties>(),
                                QUndoCommand * const opc_Parent = NULL);
   ~C_SdManUnoTopologyAddCommand(void) override;

protected:
   void m_AddNew(void) override;

private:
   const E_ElementType me_Type;
   const QPointF mc_NewPos;
   const QString mc_AdditionalInformation;
   const uint64_t mu64_BusConnectorNodeId;
   const uint64_t mu64_BusConnectorBusId;
   const uint8_t mu8_InterfaceNumber;
   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> mc_Properties;
   const bool mq_ForceUseAdditionalInformation;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
