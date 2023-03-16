//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move undo command (implementation)

   Move undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SebUnoMoveCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene              Pointer to currently active scene
   \param[in]     orc_Ids                Affected unique IDs
   \param[in]     orc_PositionDifference Position difference for this move action
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoMoveCommand::C_SebUnoMoveCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                         const QPointF & orc_PositionDifference, QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_Ids, "Move drawing element(s)", opc_Parent),
   mc_PositionDifference(orc_PositionDifference)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoMoveCommand::~C_SebUnoMoveCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveCommand::undo(void)
{
   vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
   for (uint32_t u32_ItId = 0; u32_ItId < c_Items.size(); ++u32_ItId)
   {
      m_UndoSingle(c_Items[u32_ItId]);
   }
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveCommand::redo(void)
{
   vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
   for (uint32_t u32_ItId = 0; u32_ItId < c_Items.size(); ++u32_ItId)
   {
      m_RedoSingle(c_Items[u32_ItId]);
   }
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Attempt merge with current command

   \param[in] opc_Command Command to merge with

   \return
   true: merged into current command
   false: no merge possible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebUnoMoveCommand::mergeWith(const QUndoCommand * const opc_Command)
{
   bool q_Retval = false;
   const C_SebUnoMoveCommand * const pc_MoveCommand =
      dynamic_cast<const C_SebUnoMoveCommand * const>(opc_Command);

   if (pc_MoveCommand != NULL)
   {
      //Check item IDs
      if (m_MergePossible(pc_MoveCommand) == true)
      {
         q_Retval = true;
         this->mc_PositionDifference += pc_MoveCommand->mc_PositionDifference;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo for one item

   \param[in,out] opc_Item Item to perform action on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveCommand::m_UndoSingle(QGraphicsItem * const opc_Item) const
{
   if (opc_Item != NULL)
   {
      opc_Item->setPos(opc_Item->pos() - this->mc_PositionDifference);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo for one item

   \param[in,out] opc_Item Item to perform action on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveCommand::m_RedoSingle(QGraphicsItem * const opc_Item) const
{
   if (opc_Item != NULL)
   {
      opc_Item->setPos(opc_Item->pos() + this->mc_PositionDifference);
   }
}
