//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for reconnect bus connector (implementation)

   Base class for reconnect bus connector

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdManUnoTopologyReconnectBaseCommand.hpp"
#include "C_SebUnoTopBusConnectorMoveCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
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
   \param[in]      orc_Ids                Affected unique IDs
   \param[in]      oru64_StartingItemId   Initial item ID
   \param[in]      oru64_LastItemId       New item ID
   \param[in]      orc_ConnectionPos      Event position
   \param[in]      ors32_Interface        Interface to connect to
   \param[in]      orc_Properties         Properties
   \param[in]      orc_Description        Undo step description
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyReconnectBaseCommand::C_SdManUnoTopologyReconnectBaseCommand(QGraphicsScene * const opc_Scene,
                                                                               const std::vector<uint64_t> & orc_Ids,
                                                                               const uint64_t & oru64_StartingItemId,
                                                                               const uint64_t & oru64_LastItemId,
                                                                               const QPointF & orc_ConnectionPos,
                                                                               const int32_t & ors32_Interface,
                                                                               const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, const QString & orc_Description,
                                                                               QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_Ids, orc_Description, opc_Parent),
   mu64_StartingItemId(oru64_StartingItemId),
   mu64_LastItemId(oru64_LastItemId),
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
   m_Reconnect(this->mu64_LastItemId, this->mu64_StartingItemId, static_cast<int32_t>(this->mu8_InitialInterface),
               this->mc_InitialProperties);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo reconnect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyReconnectBaseCommand::redo(void)
{
   m_Reconnect(this->mu64_StartingItemId, this->mu64_LastItemId, this->ms32_Interface, this->mc_Properties);
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
