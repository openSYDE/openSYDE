//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for scene undo command stack (implementation)

   Handler class for scene undo command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <vector>

#include "stwtypes.hpp"

#include "C_SebUnoBaseManager.hpp"

#include "C_SebUnoResizeLineCommand.hpp"
#include "C_SebUnoResizeRectangleCommand.hpp"
#include "C_SebUnoZetOrderCommand.hpp"
#include "C_SebUnoAlignCommand.hpp"
#include "C_SebUnoAddBendPointCommand.hpp"
#include "C_SebUnoRemoveBendPointCommand.hpp"
#include "C_SebUnoMoveSubLineCommand.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::tgl;

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
C_SebUnoBaseManager::C_SebUnoBaseManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent) :
   C_UtiUndoStack(opc_Parent),
   mpc_StyleCommand(NULL),
   mpc_Scene(opc_Scene),
   mpc_MoveCommandGroup(NULL),
   mpc_LastMoveCommand(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseManager::~C_SebUnoBaseManager(void)
{
   delete (mpc_StyleCommand);
   delete (mpc_MoveCommandGroup);
   delete (mpc_LastMoveCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterMove(void)
{
   if (this->mpc_MoveCommandGroup != NULL)
   {
      this->mpc_MoveCommandGroup->undo();
      this->DoPush(this->mpc_MoveCommandGroup);
      this->mpc_MoveCommandGroup = NULL;
      this->mpc_LastMoveCommand = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do move

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::DoMove(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_PositionDifference)
{
   vector<uint64_t> c_Ids;

   if (this->mpc_MoveCommandGroup == NULL)
   {
      this->mpc_MoveCommandGroup = new C_SebUnoMoveCommandGroup();
   }

   mh_MapItemToId(orc_Items, c_Ids);
   if (this->mpc_LastMoveCommand == NULL)
   {
      this->mpc_LastMoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_Ids, orc_PositionDifference,
                                                          this->mpc_MoveCommandGroup);
      this->mpc_LastMoveCommand->redo();
   }
   else
   {
      C_SebUnoMoveCommand * const pc_MoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_Ids,
                                                                           orc_PositionDifference);
      pc_MoveCommand->redo();
      tgl_assert(this->mpc_LastMoveCommand->mergeWith(pc_MoveCommand) == true);
      delete (pc_MoveCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register resize

   \param[in] orc_Items                Affected items
   \param[in] ors32_InteractionPointId Interaction point ID
   \param[in] orc_PositionDifference   Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items,
                                             const int32_t & ors32_InteractionPointId,
                                             const QPointF & orc_PositionDifference)
{
   vector<uint64_t> c_Ids;
   C_SebUnoTopResizeLineCommand * pc_ResizeCommand;

   mh_MapItemToId(orc_Items, c_Ids);
   pc_ResizeCommand = new C_SebUnoTopResizeLineCommand(this->mpc_Scene, c_Ids, ors32_InteractionPointId,
                                                       orc_PositionDifference);
   pc_ResizeCommand->undo();
   this->DoPush(pc_ResizeCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register resize action

   \param[in] orc_Items   Affected items
   \param[in] orc_OldPos  Old position
   \param[in] orc_OldSize Old size
   \param[in] orc_NewPos  New position
   \param[in] orc_NewSize New size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterResizeRectangle(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_OldPos,
                                                  const QSizeF & orc_OldSize, const QPointF & orc_NewPos,
                                                  const QSizeF & orc_NewSize)
{
   vector<uint64_t> c_Ids;
   C_SebUnoResizeRectangleCommand * pc_ResizeCommand;

   mh_MapItemToId(orc_Items, c_Ids);
   pc_ResizeCommand = new C_SebUnoResizeRectangleCommand(this->mpc_Scene, c_Ids, orc_OldPos,
                                                         orc_OldSize, orc_NewPos, orc_NewSize);
   pc_ResizeCommand->undo();
   this->DoPush(pc_ResizeCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register complete move step

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterCompleteMoveStep(const QList<QGraphicsItem *> & orc_Items,
                                                   const QPointF & orc_PositionDifference)
{
   vector<uint64_t> c_Ids;
   C_SebUnoMoveCommand * pc_MoveCommand;

   mh_MapItemToId(orc_Items, c_Ids);
   pc_MoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_Ids, orc_PositionDifference,
                                            this->mpc_MoveCommandGroup);
   pc_MoveCommand->undo();
   this->DoPush(pc_MoveCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do move sub line

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
   \param[in] ors32_SubLineId        Sub line ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items,
                                              const QPointF & orc_PositionDifference, const int32_t & ors32_SubLineId)
{
   vector<uint64_t> c_Ids;
   C_SebUnoMoveSubLineCommand * pc_MoveCommand;

   mh_MapItemToId(orc_Items, c_Ids);
   pc_MoveCommand = new C_SebUnoMoveSubLineCommand(this->mpc_Scene, c_Ids, ors32_SubLineId,
                                                   orc_PositionDifference);

   pc_MoveCommand->undo();
   this->DoPush(pc_MoveCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add bend point to line

   \param[in,out] opc_Line     Line item
   \param[in]     orc_ScenePos Scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::DoBendLine(const C_GiLiLineGroup * const opc_Line, const QPointF & orc_ScenePos)
{
   vector<uint64_t> c_Ids;
   uint64_t u64_Id;
   C_SebUnoAddBendPointCommand * pc_AddCommand;

   mh_MapItemToId(opc_Line, u64_Id);
   c_Ids.push_back(u64_Id);
   pc_AddCommand = new C_SebUnoAddBendPointCommand(this->mpc_Scene, c_Ids,
                                                   orc_ScenePos);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove bend point of line

   \param[in,out] opc_Line     Line item
   \param[in]     orc_ScenePos Scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::DoRemoveBendLine(const C_GiLiLineGroup * const opc_Line, const QPointF & orc_ScenePos)
{
   vector<uint64_t> c_Ids;
   uint64_t u64_Id;
   C_SebUnoRemoveBendPointCommand * pc_RemoveCommand;

   mh_MapItemToId(opc_Line, u64_Id);
   c_Ids.push_back(u64_Id);
   pc_RemoveCommand = new C_SebUnoRemoveBendPointCommand(this->mpc_Scene, c_Ids,
                                                         orc_ScenePos);
   this->DoPush(pc_RemoveCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do align

   \param[in,out] orc_Items     Items to align
   \param[in]     opc_AlignItem Item to align items to
   \param[in]     ore_Alignment Alignment type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::DoAlign(const QList<QGraphicsItem *> & orc_Items, const QGraphicsItem * const opc_AlignItem,
                                  const E_Alignment & ore_Alignment)
{
   vector<uint64_t> c_Ids;
   C_SebUnoAlignCommand * pc_AlignCommand;
   uint64_t u64_AlignId;

   mh_MapItemToId(orc_Items, c_Ids);
   mh_MapItemToId(opc_AlignItem, u64_AlignId);
   pc_AlignCommand = new C_SebUnoAlignCommand(this->mpc_Scene, c_Ids, u64_AlignId, ore_Alignment);
   this->DoPush(pc_AlignCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt z order of selected items

   \param[in] orc_SelectedItems All selected items
   \param[in] orc_Items         All items
   \param[in] oq_BringToFront   Flag if this is the bring to front action (otherwise send to back assumed)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::AdaptZetOrder(const QList<QGraphicsItem *> & orc_SelectedItems,
                                        const QList<QGraphicsItem *> & orc_Items, const bool oq_BringToFront)
{
   QMap<QGraphicsItem *, float64_t> c_Changes;
   vector<uint64_t> c_Ids;
   vector<float64_t> c_Values;
   QList<QGraphicsItem *> c_List;
   C_SebUnoZetOrderCommand * pc_ZetOrderCommand;

   Q_UNUSED(orc_Items)

   C_SebUnoZetOrderCommand::h_AdaptZetOrder(this->mpc_Scene, orc_Items, orc_SelectedItems, oq_BringToFront, c_Changes);

   for (QMap<QGraphicsItem *, float64_t>::const_iterator c_ItChanges = c_Changes.begin();
        c_ItChanges != c_Changes.end(); ++c_ItChanges)
   {
      c_List.push_back(c_ItChanges.key());
      c_Values.push_back(c_ItChanges.value());
   }
   mh_MapItemToId(c_List, c_Ids);
   pc_ZetOrderCommand = new C_SebUnoZetOrderCommand(this->mpc_Scene, c_Ids, c_Values);
   this->DoPush(pc_ZetOrderCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save style information

   \param[in,out] orc_Items   Affected items
   \param[in]     oq_DarkMode Optional flag if dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::SaveStyleInformation(const QList<QGraphicsItem *> & orc_Items, const bool oq_DarkMode)
{
   vector<uint64_t> c_Ids;
   if (this->mpc_StyleCommand != NULL)
   {
      delete (this->mpc_StyleCommand);
      this->mpc_StyleCommand = NULL;
   }

   mh_MapItemToId(orc_Items, c_Ids); //new C_SdManUnoTopologySetupStyleCommand(this->mpc_Scene, c_Ids);
   this->mpc_StyleCommand = this->m_GetNewStyleCommand(c_Ids, oq_DarkMode);
   this->mpc_StyleCommand->InitPrevious();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register a new style change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterStyleChange(void)
{
   if (this->mpc_StyleCommand != NULL)
   {
      this->mpc_StyleCommand->InitNext();
      this->mpc_StyleCommand->undo();
      this->DoPush(this->mpc_StyleCommand);
      this->mpc_StyleCommand = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map items to unique IDs

   \param[in]  orc_Items Original items
   \param[out] orc_Ids   Unique IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::mh_MapItemToId(const QList<QGraphicsItem *> & orc_Items, vector<uint64_t> & orc_Ids)
{
   vector<uint64_t> c_DuplicateIds;
   uint32_t u32_ItItem = 0;

   //Get ID for each item
   c_DuplicateIds.resize(orc_Items.size());
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end(); ++c_ItItem)
   {
      mh_MapItemToId(*c_ItItem, c_DuplicateIds[u32_ItItem]);
      ++u32_ItItem;
   }

   //Clear duplicates
   orc_Ids.reserve(c_DuplicateIds.size());
   for (uint32_t u32_ItDuplicateItem = 0; u32_ItDuplicateItem < c_DuplicateIds.size(); ++u32_ItDuplicateItem)
   {
      bool q_Found = false;
      for (u32_ItItem = 0; u32_ItItem < orc_Ids.size(); ++u32_ItItem)
      {
         if (orc_Ids[u32_ItItem] == c_DuplicateIds[u32_ItDuplicateItem])
         {
            q_Found = true;
         }
      }
      if (q_Found == false)
      {
         orc_Ids.push_back(c_DuplicateIds[u32_ItDuplicateItem]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map item to unique ID

   \param[in]  opc_Item Original item
   \param[out] oru64_Id Unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::mh_MapItemToId(const QGraphicsItem * const opc_Item, uint64_t & oru64_Id)
{
   const C_GiUnique * const pc_UniqueItem =
      dynamic_cast<const C_GiUnique *>(C_SebUtil::h_GetHighestParent(opc_Item));

   if (pc_UniqueItem != NULL)
   {
      oru64_Id = pc_UniqueItem->GetId();
   }
   else
   {
      oru64_Id = 0xFFFFFFFFFFFFFFFFULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get new style command (Destruction has to be handled by calling function)

   \param[in] orc_Items   Item IDs
   \param[in] oq_DarkMode Optional flag if dark mode active

   \return
   Valid pointer to new style command
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoSetupStyleCommand * C_SebUnoBaseManager::m_GetNewStyleCommand(const std::vector<uint64_t> & orc_Items,
                                                                      const bool oq_DarkMode)
{
   return new C_SebUnoSetupStyleCommand(this->mpc_Scene, orc_Items, oq_DarkMode);
}
