//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Move sub line undo command (implementation)

   Move sub line undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SebUnoMoveSubLineCommand.h"
#include "C_GiLiLineGroup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace std;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene              Pointer to currently active scene
   \param[in]     orc_IDs                Affected unique IDs
   \param[in]     ors32_SubLineID        Sub line ID
   \param[in]     orc_PositionDifference Position difference
   \param[in]     opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoMoveSubLineCommand::C_SebUnoMoveSubLineCommand(QGraphicsScene * const opc_Scene,
                                                       const std::vector<stw_types::uint64> & orc_IDs,
                                                       const stw_types::sint32 & ors32_SubLineID,
                                                       const QPointF & orc_PositionDifference,
                                                       QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Move sub line of one line based drawing element", opc_Parent),
   ms32_SubLineID(ors32_SubLineID),
   mc_PositionDifference(orc_PositionDifference)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoMoveSubLineCommand::~C_SebUnoMoveSubLineCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveSubLineCommand::undo(void)
{
   vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
   for (uint32 u32_ItID = 0; u32_ItID < c_Items.size(); ++u32_ItID)
   {
      m_UndoSingle(c_Items[u32_ItID]);
   }
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveSubLineCommand::redo(void)
{
   vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
   for (uint32 u32_ItID = 0; u32_ItID < c_Items.size(); ++u32_ItID)
   {
      m_RedoSingle(c_Items[u32_ItID]);
   }
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo for one item

   \param[in,out] opc_Item Item to perform action on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveSubLineCommand::m_UndoSingle(QGraphicsItem * const opc_Item) const
{
   if (opc_Item != NULL)
   {
      C_GiLiLineGroup * pc_LineGroup;

      pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(opc_Item);
      if (pc_LineGroup != NULL)
      {
         const sint32 s32_NumPoints = pc_LineGroup->GetNumberPoints();
         if ((this->ms32_SubLineID >= 0) && ((this->ms32_SubLineID + 1) < s32_NumPoints))
         {
            QPointF c_OldPos;
            //Move all sub line points
            for (sint32 s32_ItPoint = this->ms32_SubLineID; s32_ItPoint <= (this->ms32_SubLineID + 1); ++s32_ItPoint)
            {
               pc_LineGroup->GetPointPos(s32_ItPoint, c_OldPos);
               pc_LineGroup->UpdatePoint(s32_ItPoint, c_OldPos - this->mc_PositionDifference, true);
            }
            //Handle sig changed only once
            pc_LineGroup->TriggerSigChangedGraphic();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo for one item

   \param[in,out] opc_Item Item to perform action on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoMoveSubLineCommand::m_RedoSingle(QGraphicsItem * const opc_Item) const
{
   if (opc_Item != NULL)
   {
      C_GiLiLineGroup * pc_LineGroup;

      pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(opc_Item);
      if (pc_LineGroup != NULL)
      {
         const sint32 s32_NumPoints = pc_LineGroup->GetNumberPoints();
         if ((this->ms32_SubLineID >= 0) && ((this->ms32_SubLineID + 1) < s32_NumPoints))
         {
            QPointF c_OldPos;
            //Move all sub line points
            for (sint32 s32_ItPoint = this->ms32_SubLineID; s32_ItPoint <= (this->ms32_SubLineID + 1); ++s32_ItPoint)
            {
               pc_LineGroup->GetPointPos(s32_ItPoint, c_OldPos);
               pc_LineGroup->UpdatePoint(s32_ItPoint, c_OldPos + this->mc_PositionDifference, true);
            }
            //Handle sig changed only once
            pc_LineGroup->TriggerSigChangedGraphic();
         }
      }
   }
}
