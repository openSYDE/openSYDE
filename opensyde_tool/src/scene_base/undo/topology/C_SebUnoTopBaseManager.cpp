//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology scene undo command stack (implementation)

   Handler class for topology scene undo command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SebUnoTopBaseManager.h"
#include "C_SebUnoTopBusConnectorMoveCommand.h"
#include "C_GiLiBusConnectorBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene  Scene to use undo redo framework for
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoTopBaseManager::C_SebUnoTopBaseManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent) :
   C_SebUnoBaseManager(opc_Scene, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoTopBaseManager::~C_SebUnoTopBaseManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register resize

   \param[in] orc_Items                Affected items
   \param[in] ors32_InteractionPointID Interaction point ID
   \param[in] orc_PositionDifference   Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBaseManager::RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items,
                                                const sint32 & ors32_InteractionPointID,
                                                const QPointF & orc_PositionDifference)
{
   if (m_CheckBusConnectorChange(orc_Items) == false)
   {
      C_SebUnoBaseManager::RegisterResizeLine(orc_Items, ors32_InteractionPointID, orc_PositionDifference);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register complete move step

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBaseManager::RegisterCompleteMoveStep(const QList<QGraphicsItem *> & orc_Items,
                                                      const QPointF & orc_PositionDifference)
{
   if (m_CheckBusConnectorChange(orc_Items) == false)
   {
      C_SebUnoBaseManager::RegisterCompleteMoveStep(orc_Items, orc_PositionDifference);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do move sub line

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
   \param[in] ors32_SubLineID        Sub line ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBaseManager::RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items,
                                                 const QPointF & orc_PositionDifference, const sint32 & ors32_SubLineID)
{
   if (m_CheckBusConnectorChange(orc_Items) == false)
   {
      C_SebUnoBaseManager::RegisterMoveSubLine(orc_Items, orc_PositionDifference, ors32_SubLineID);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save current bus connector state for later

   \param[in] opc_BusConnector Bus connector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBaseManager::SaveBusConnectorInitialPoints(const C_GiLiBusConnectorBase * const opc_BusConnector)
{
   if (opc_BusConnector != NULL)
   {
      this->mc_BusConnectorInitialPoints = opc_BusConnector->GetPointsScenePos();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do delete items

   \param[in] orc_Items Items
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_SebUnoTopBaseManager::DoDelete(const QList<QGraphicsItem *> & orc_Items)
{
   Q_UNUSED(orc_Items)
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoTopBaseManager::m_CleanupAction(void)
{
   Q_EMIT this->SigChanged();
   C_UtiUndoStack::m_CleanupAction();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if special handling for bus connectors necessary

   \param[in] orc_Items Affected items

   \return
   true: special handling
   false: no action
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebUnoTopBaseManager::m_CheckBusConnectorChange(const QList<QGraphicsItem *> & orc_Items)
{
   bool q_Retval = false;

   if (orc_Items.size() == 1)
   {
      const C_GiLiBusConnectorBase * const pc_BusConnector =
         dynamic_cast<const C_GiLiBusConnectorBase * const>(orc_Items[0]);
      if (pc_BusConnector != NULL)
      {
         vector<uint64> c_IDs;
         uint64 u64_ID;
         C_SebUnoTopBusConnectorMoveCommand * pc_MoveCommand;

         mh_MapItemToID(pc_BusConnector, u64_ID);
         c_IDs.push_back(u64_ID);
         q_Retval = true;
         pc_MoveCommand = new C_SebUnoTopBusConnectorMoveCommand(this->mpc_Scene, c_IDs,
                                                                 this->mc_BusConnectorInitialPoints,
                                                                 pc_BusConnector->GetPointsScenePos());
         pc_MoveCommand->undo();
         this->DoPush(pc_MoveCommand);
      }
   }
   return q_Retval;
}
