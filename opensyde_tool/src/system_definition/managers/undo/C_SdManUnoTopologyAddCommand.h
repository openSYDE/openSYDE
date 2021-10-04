//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDCOMMAND_H
#define C_SDMANUNOTOPOLOGYADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyAddBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const QString & orc_AdditionalInformation = "", QUndoCommand * const opc_Parent = NULL,
                                const bool & orq_ForceUseAdditionalInformation = false);
   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const stw_types::uint64 ou64_BusConnectorNodeID = 0,
                                const stw_types::uint64 ou64_BusConnectorBusID = 0,
                                const stw_types::uint8 ou8_InterfaceNumber = 0,
                                const std::vector<stw_types::uint8> & orc_NodeIds = std::vector<stw_types::uint8>(),
                                QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyAddCommand(void);

protected:
   virtual void m_AddNew(void) override;

private:
   const E_ElementType me_Type;
   const QPointF mc_NewPos;
   const QString mc_AdditionalInformation;
   const stw_types::uint64 mu64_BusConnectorNodeID;
   const stw_types::uint64 mu64_BusConnectorBusID;
   const stw_types::uint8 mu8_InterfaceNumber;
   const std::vector<stw_types::uint8> mc_NodeIds;
   const bool mq_ForceUseAdditionalInformation;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
