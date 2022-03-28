//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for reconnect bus connector (implementation)

   Base class for reconnect bus connector

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManUnoTopologyReconnectBaseCommand.h"
#include "C_SebUnoTopBusConnectorMoveCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene              Pointer to currently active scene
   \param[in]      orc_IDs                Affected unique IDs
   \param[in]      oru64_StartingItemID   Initial item ID
   \param[in]      oru64_LastItemID       New item ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_Properties         Properties
   \param[in]      orc_Description        Undo step description
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBaseCommand::C_SdManUnoTopologyReconnectBaseCommand(QGraphicsScene * const opc_Scene,
                                                                               const std::vector<uint64> & orc_IDs,
                                                                               const uint64 & oru64_StartingItemID,
                                                                               const uint64 & oru64_LastItemID,
                                                                               const QPointF & orc_ConnectionPos,
                                                                               const sint32 & ors32_Interface,
                                                                               const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, const QString & orc_Description,
                                                                               QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, orc_Description, opc_Parent),
   mu64_StartingItemID(oru64_StartingItemID),
   mu64_LastItemID(oru64_LastItemID),
   mc_ConnectionPos(orc_ConnectionPos),
   ms32_Interface(ors32_Interface),
   mu8_InitialInterface(255U),
   mc_Properties(orc_Properties),
   mc_InitialProperties(),
   mq_Merged(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBaseCommand::~C_SdManUnoTopologyReconnectBaseCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo reconnect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectBaseCommand::undo(void)
{
   m_Reconnect(this->mu64_LastItemID, this->mu64_StartingItemID, static_cast<sint32>(this->mu8_InitialInterface),
               this->mc_InitialProperties);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo reconnect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectBaseCommand::redo(void)
{
   m_Reconnect(this->mu64_StartingItemID, this->mu64_LastItemID, this->ms32_Interface, this->mc_Properties);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current bus connector

   \return
   Bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBusConnector * C_SdManUnoTopologyReconnectBaseCommand::m_GetBusConnector(void) const
{
   C_GiLiBusConnector * pc_Retval = NULL;

   const vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   if (c_Items.size() > 0)
   {
      pc_Retval = dynamic_cast<C_GiLiBusConnector *>(c_Items[0]);
   }
   return pc_Retval;
}
