//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdUtil.h"
#include "C_SdManUnoTopologyChangeInterfaceCommand.h"
#include "C_GiLiBusConnector.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Scene                 Pointer to currently active scene
   \param[in]      orc_IDs                   Affected unique IDs
   \param[in]      oru8_PreviousInterface    Previous interface number
   \param[in]      oru8_NewInterface         New interface number to use
   \param[in]      orc_PreviousProperties    Previous properties
   \param[in]      orc_NewProperties         New properties
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyChangeInterfaceCommand::C_SdManUnoTopologyChangeInterfaceCommand(QGraphicsScene * const opc_Scene,
                                                                                   const std::vector<uint64> & orc_IDs,
                                                                                   const uint8 & oru8_PreviousInterface,
                                                                                   const uint8 & oru8_NewInterface,
                                                                                   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_PreviousProperties,
                                                                                   const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_NewProperties,
                                                                                   QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Change interface of bus connection(s)", opc_Parent),
   mu8_PreviousInterface(oru8_PreviousInterface),
   mu8_NewInterface(oru8_NewInterface),
   mc_PreviousProperties(orc_PreviousProperties),
   mc_NewProperties(orc_NewProperties)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyChangeInterfaceCommand::~C_SdManUnoTopologyChangeInterfaceCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo change interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::undo(void)
{
   m_ChangeInterface(this->mu8_PreviousInterface, this->mc_PreviousProperties);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo change interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::redo(void)
{
   m_ChangeInterface(this->mu8_NewInterface, this->mc_NewProperties);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change interface of all bus connectors to specified one

   \param[in]  oru8_NewInterface    New interface to change to
   \param[in]  orc_Properties       Properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyChangeInterfaceCommand::m_ChangeInterface(const uint8 & oru8_NewInterface,
                                                                 const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
const
{
   const vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      C_GiLiBusConnector * const pc_CurConn = dynamic_cast<C_GiLiBusConnector *>(*c_ItItem);
      if (pc_CurConn != NULL)
      {
         pc_CurConn->ChangeInterface(oru8_NewInterface, orc_Properties);
      }
   }
}
