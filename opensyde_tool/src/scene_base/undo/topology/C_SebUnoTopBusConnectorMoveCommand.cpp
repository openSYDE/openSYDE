//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move / change bus connector undo command (implementation)

   Move / change bus connector undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SebUnoTopBusConnectorMoveCommand.h"
#include "C_GiLiBusConnectorBase.h"

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
/*! \brief  Default constructor

   \param[in,out] opc_Scene         Pointer to currently active scene
   \param[in]     orc_IDs           Affected unique IDs
   \param[in]     orc_InitialPoints Point state before move
   \param[in]     orc_FinalPoints   Point state after move
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoTopBusConnectorMoveCommand::C_SebUnoTopBusConnectorMoveCommand(QGraphicsScene * const opc_Scene,
                                                                       const vector<uint64> & orc_IDs,
                                                                       const vector<QPointF> & orc_InitialPoints,
                                                                       const vector<QPointF> & orc_FinalPoints,
                                                                       QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Bus connector position change", opc_Parent),
   mc_Initial(orc_InitialPoints),
   mc_Final(orc_FinalPoints)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy constructor

   \param[in]     opc_Prev   Original command
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoTopBusConnectorMoveCommand::C_SebUnoTopBusConnectorMoveCommand(
   const C_SebUnoTopBusConnectorMoveCommand * const opc_Prev, QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Prev, opc_Parent),
   mc_Initial(opc_Prev->mc_Initial),
   mc_Final(opc_Prev->mc_Final)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoTopBusConnectorMoveCommand::~C_SebUnoTopBusConnectorMoveCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBusConnectorMoveCommand::undo(void)
{
   const vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiLiBusConnectorBase * const pc_BusConn = dynamic_cast<C_GiLiBusConnectorBase *>(*c_ItItem);
      if (pc_BusConn != NULL)
      {
         pc_BusConn->SetPoints(this->mc_Initial);
      }
   }
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBusConnectorMoveCommand::redo(void)
{
   const vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiLiBusConnectorBase * const pc_BusConn = dynamic_cast<C_GiLiBusConnectorBase *>(*c_ItItem);
      if (pc_BusConn != NULL)
      {
         pc_BusConn->SetPoints(this->mc_Final);
      }
   }
   QUndoCommand::redo();
}
