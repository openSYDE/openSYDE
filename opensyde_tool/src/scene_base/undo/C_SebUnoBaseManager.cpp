//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for scene undo command stack (implementation)

   Handler class for scene undo command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <vector>

#include "stwtypes.h"

#include "C_SebUnoBaseManager.h"

#include "C_SebUnoResizeLineCommand.h"
#include "C_SebUnoResizeRectangleCommand.h"
#include "C_SebUnoZOrderCommand.h"
#include "C_SebUnoAlignCommand.h"
#include "C_SebUnoAddBendPointCommand.h"
#include "C_SebUnoRemoveBendPointCommand.h"
#include "C_SebUnoMoveSubLineCommand.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_tgl;

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
   vector<uint64> c_IDs;

   if (this->mpc_MoveCommandGroup == NULL)
   {
      this->mpc_MoveCommandGroup = new C_SebUnoMoveCommandGroup();
   }

   mh_MapItemToID(orc_Items, c_IDs);
   if (this->mpc_LastMoveCommand == NULL)
   {
      this->mpc_LastMoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_IDs, orc_PositionDifference,
                                                          this->mpc_MoveCommandGroup);
      this->mpc_LastMoveCommand->redo();
   }
   else
   {
      C_SebUnoMoveCommand * const pc_MoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_IDs,
                                                                           orc_PositionDifference);
      pc_MoveCommand->redo();
      tgl_assert(this->mpc_LastMoveCommand->mergeWith(pc_MoveCommand) == true);
      delete (pc_MoveCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register resize

   \param[in] orc_Items                Affected items
   \param[in] ors32_InteractionPointID Interaction point ID
   \param[in] orc_PositionDifference   Position difference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items,
                                             const sint32 & ors32_InteractionPointID,
                                             const QPointF & orc_PositionDifference)
{
   vector<uint64> c_IDs;
   C_SebUnoTopResizeLineCommand * pc_ResizeCommand;

   mh_MapItemToID(orc_Items, c_IDs);
   pc_ResizeCommand = new C_SebUnoTopResizeLineCommand(this->mpc_Scene, c_IDs, ors32_InteractionPointID,
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
   vector<uint64> c_IDs;
   C_SebUnoResizeRectangleCommand * pc_ResizeCommand;

   mh_MapItemToID(orc_Items, c_IDs);
   pc_ResizeCommand = new C_SebUnoResizeRectangleCommand(this->mpc_Scene, c_IDs, orc_OldPos,
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
   vector<uint64> c_IDs;
   C_SebUnoMoveCommand * pc_MoveCommand;

   mh_MapItemToID(orc_Items, c_IDs);
   pc_MoveCommand = new C_SebUnoMoveCommand(this->mpc_Scene, c_IDs, orc_PositionDifference,
                                            this->mpc_MoveCommandGroup);
   pc_MoveCommand->undo();
   this->DoPush(pc_MoveCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do move sub line

   \param[in] orc_Items              Affected items
   \param[in] orc_PositionDifference Position difference
   \param[in] ors32_SubLineID        Sub line ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items,
                                              const QPointF & orc_PositionDifference, const sint32 & ors32_SubLineID)
{
   vector<uint64> c_IDs;
   C_SebUnoMoveSubLineCommand * pc_MoveCommand;

   mh_MapItemToID(orc_Items, c_IDs);
   pc_MoveCommand = new C_SebUnoMoveSubLineCommand(this->mpc_Scene, c_IDs, ors32_SubLineID,
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
   vector<uint64> c_IDs;
   uint64 u64_ID;
   C_SebUnoAddBendPointCommand * pc_AddCommand;

   mh_MapItemToID(opc_Line, u64_ID);
   c_IDs.push_back(u64_ID);
   pc_AddCommand = new C_SebUnoAddBendPointCommand(this->mpc_Scene, c_IDs,
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
   vector<uint64> c_IDs;
   uint64 u64_ID;
   C_SebUnoRemoveBendPointCommand * pc_RemoveCommand;

   mh_MapItemToID(opc_Line, u64_ID);
   c_IDs.push_back(u64_ID);
   pc_RemoveCommand = new C_SebUnoRemoveBendPointCommand(this->mpc_Scene, c_IDs,
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
   vector<uint64> c_IDs;
   C_SebUnoAlignCommand * pc_AlignCommand;
   uint64 u64_AlignID;

   mh_MapItemToID(orc_Items, c_IDs);
   mh_MapItemToID(opc_AlignItem, u64_AlignID);
   pc_AlignCommand = new C_SebUnoAlignCommand(this->mpc_Scene, c_IDs, u64_AlignID, ore_Alignment);
   this->DoPush(pc_AlignCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt z order of selected items

   \param[in] orc_SelectedItems All selected items
   \param[in] orc_Items         All items
   \param[in] oq_BringToFront   Flag if this is the bring to front action (otherwise send to back assumed)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::AdaptZOrder(const QList<QGraphicsItem *> & orc_SelectedItems,
                                      const QList<QGraphicsItem *> & orc_Items, const bool oq_BringToFront)
{
   QMap<QGraphicsItem *, float64> c_Changes;
   vector<uint64> c_IDs;
   vector<float64> c_Values;
   QList<QGraphicsItem *> c_List;
   C_SebUnoZOrderCommand * pc_ZOrderCommand;

   Q_UNUSED(orc_Items)

   C_SebUnoZOrderCommand::h_AdaptZOrder(this->mpc_Scene, orc_Items, orc_SelectedItems, oq_BringToFront, c_Changes);

   for (QMap<QGraphicsItem *, float64>::const_iterator c_ItChanges = c_Changes.begin();
        c_ItChanges != c_Changes.end(); ++c_ItChanges)
   {
      c_List.push_back(c_ItChanges.key());
      c_Values.push_back(c_ItChanges.value());
   }
   mh_MapItemToID(c_List, c_IDs);
   pc_ZOrderCommand = new C_SebUnoZOrderCommand(this->mpc_Scene, c_IDs, c_Values);
   this->DoPush(pc_ZOrderCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save style information

   \param[in,out] orc_Items   Affected items
   \param[in]     oq_DarkMode Optional flag if dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::SaveStyleInformation(const QList<QGraphicsItem *> & orc_Items, const bool oq_DarkMode)
{
   vector<uint64> c_IDs;
   if (this->mpc_StyleCommand != NULL)
   {
      delete (this->mpc_StyleCommand);
      this->mpc_StyleCommand = NULL;
   }

   mh_MapItemToID(orc_Items, c_IDs); //new C_SdManUnoTopologySetupStyleCommand(this->mpc_Scene, c_IDs);
   this->mpc_StyleCommand = this->m_GetNewStyleCommand(c_IDs, oq_DarkMode);
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
   \param[out] orc_IDs   Unique IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::mh_MapItemToID(const QList<QGraphicsItem *> & orc_Items, vector<uint64> & orc_IDs)
{
   vector<uint64> c_DuplicateIDs;
   uint32 u32_ItItem = 0;

   //Get ID for each item
   c_DuplicateIDs.resize(orc_Items.size());
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end(); ++c_ItItem)
   {
      mh_MapItemToID(*c_ItItem, c_DuplicateIDs[u32_ItItem]);
      ++u32_ItItem;
   }

   //Clear duplicates
   orc_IDs.reserve(c_DuplicateIDs.size());
   for (uint32 u32_ItDuplicateItem = 0; u32_ItDuplicateItem < c_DuplicateIDs.size(); ++u32_ItDuplicateItem)
   {
      bool q_Found = false;
      for (u32_ItItem = 0; u32_ItItem < orc_IDs.size(); ++u32_ItItem)
      {
         if (orc_IDs[u32_ItItem] == c_DuplicateIDs[u32_ItDuplicateItem])
         {
            q_Found = true;
         }
      }
      if (q_Found == false)
      {
         orc_IDs.push_back(c_DuplicateIDs[u32_ItDuplicateItem]);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map item to unique ID

   \param[in]  opc_Item Original item
   \param[out] oru64_ID Unique ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBaseManager::mh_MapItemToID(const QGraphicsItem * const opc_Item, uint64 & oru64_ID)
{
   const C_GiUnique * const pc_UniqueItem =
      dynamic_cast<const C_GiUnique *>(C_SebUtil::h_GetHighestParent(opc_Item));

   if (pc_UniqueItem != NULL)
   {
      oru64_ID = pc_UniqueItem->GetID();
   }
   else
   {
      oru64_ID = 0xFFFFFFFFFFFFFFFFULL;
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
C_SebUnoSetupStyleCommand * C_SebUnoBaseManager::m_GetNewStyleCommand(const std::vector<uint64> & orc_Items,
                                                                      const bool oq_DarkMode)
{
   return new C_SebUnoSetupStyleCommand(this->mpc_Scene, orc_Items, oq_DarkMode);
}
