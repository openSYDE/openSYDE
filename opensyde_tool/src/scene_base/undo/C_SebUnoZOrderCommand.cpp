//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Z order undo command (implementation)

   Z order undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "gitypes.h"
#include "C_GiBiBase.h"
#include "C_SebUnoZOrderSortHelper.h"
#include "C_SebUnoZOrderCommand.h"
#include "C_SebScene.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace std;
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

   \param[in,out] opc_Scene      Pointer to currently active scene
   \param[in]     orc_IDs        Affected unique IDs
   \param[in]     orc_NewZValues New Z values
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZOrderCommand::C_SebUnoZOrderCommand(QGraphicsScene * const opc_Scene, const vector<uint64> & orc_IDs,
                                             const vector<float64> & orc_NewZValues, QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Reorder drawing element(s)", opc_Parent),
   mc_NewZValues(orc_NewZValues)
{
   vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();
   if (c_AffectedItems.size() == this->mc_NewZValues.size())
   {
      this->mc_OldZValues.resize(this->mc_NewZValues.size());
      for (uint32 u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         this->mc_OldZValues[u32_ItAffectedItem] = c_AffectedItems[u32_ItAffectedItem]->zValue();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZOrderCommand::~C_SebUnoZOrderCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::undo(void)
{
   this->m_ApplyZValues(this->mc_OldZValues);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::redo(void)
{
   this->m_ApplyZValues(this->mc_NewZValues);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt z order of selected items

   \param[in]     opc_Scene         Pointer to currently active scene
   \param[in]     orc_Items         All items
   \param[in]     orc_SelectedItems All selected items
   \param[in]     oq_BringToFront   Flag if this is the bring to front action (otherwise send to back assumed)
   \param[in,out] orc_NewZValues    Storage for a map of all new z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::h_AdaptZOrder(const QGraphicsScene * const opc_Scene,
                                          const QList<QGraphicsItem *> & orc_Items,
                                          const QList<QGraphicsItem *> & orc_SelectedItems, const bool oq_BringToFront,
                                          QMap<QGraphicsItem *, float64> & orc_NewZValues)
{
   const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

   if (pc_Scene != NULL)
   {
      QList<QGraphicsItem *> c_SelectedItems = orc_SelectedItems;
      //Step 1: filter all relevant items for z order
      pc_Scene->FilterChangableZValues(c_SelectedItems);
      //Step 2: sort all items via z order
      C_SebUnoZOrderSortHelper::h_SortZValues(c_SelectedItems);
      //Step 3: create map for target z values
      C_SebUnoZOrderCommand::mh_CreateZValueMap(opc_Scene, orc_Items, c_SelectedItems, oq_BringToFront, orc_NewZValues);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Central call to change Z values

   \param[in] orc_Values New Z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::m_ApplyZValues(const std::vector<float64> & orc_Values) const
{
   const vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();

   if (c_AffectedItems.size() == orc_Values.size())
   {
      for (uint32 u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         C_GiBiBase * const pc_GiBase = dynamic_cast<C_GiBiBase * const>(c_AffectedItems[u32_ItAffectedItem]);
         if (pc_GiBase != NULL)
         {
            pc_GiBase->SetZValueCustom(orc_Values[u32_ItAffectedItem]);
         }
         else
         {
            c_AffectedItems[u32_ItAffectedItem]->setZValue(orc_Values[u32_ItAffectedItem]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create z order map of selected items

   \param[in]     opc_Scene         Pointer to currently active scene
   \param[in]     orc_Items         All items
   \param[in]     orc_SelectedItems All selected items (sorted via z order!)
   \param[in]     oq_BringToFront   Flag if this is the bring to front action (otherwise send to back assumed)
   \param[in,out] orc_NewZValues    Storage for a map of all new z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::mh_CreateZValueMap(const QGraphicsScene * const opc_Scene,
                                               const QList<QGraphicsItem *> & orc_Items,
                                               const QList<QGraphicsItem *> & orc_SelectedItems,
                                               const bool oq_BringToFront, QMap<QGraphicsItem *,
                                                                                float64> & orc_NewZValues)
{
   if (oq_BringToFront)
   {
      const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

      if (pc_Scene != NULL)
      {
         float64 f64_Value = pc_Scene->GetHighestUsedZValueList(orc_Items);
         //Start with first item -> most obscured one
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
              c_ItItem != orc_SelectedItems.end(); ++c_ItItem)
         {
            //Iterate to next higher z value (the first one is already used!)
            ++f64_Value;

            //Insert new unused value
            orc_NewZValues.insert((*c_ItItem), f64_Value);
         }
      }
   }
   else
   {
      float64 f64_Value = C_SebUnoZOrderCommand::mh_GetLowestUsedZValueList(opc_Scene, orc_Items);
      //Start with last item -> most visible one
      for (QList<QGraphicsItem *>::const_reverse_iterator c_ItItem = orc_SelectedItems.rbegin();
           c_ItItem != orc_SelectedItems.rend(); ++c_ItItem)
      {
         //Iterate to next lower z value (the first one is already used!)
         --f64_Value;

         //Insert new unused value
         orc_NewZValues.insert((*c_ItItem), f64_Value);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get lowest used Z value of all provided items

   \param[in,out] opc_Scene Pointer to currently active scene
   \param[in]     orc_Items Items to get lowest Z value for

   \return
   Lowest used Z value of all scene items
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_SebUnoZOrderCommand::mh_GetLowestUsedZValueList(const QGraphicsScene * const opc_Scene,
                                                          const QList<QGraphicsItem *> & orc_Items)
{
   bool q_NothingFound = true;
   float64 f64_Retval = std::numeric_limits<float64>::max();
   const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

   if (pc_Scene != NULL)
   {
      for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end(); ++c_ItItem)
      {
         const QGraphicsItem * const pc_Item = *c_ItItem;
         if ((pc_Item != NULL) && (pc_Scene->IsZOrderChangeable(pc_Item) == true))
         {
            // search the lowest z value of all items
            if (pc_Item->zValue() < f64_Retval)
            {
               f64_Retval = pc_Item->zValue();
               q_NothingFound = false;
            }
         }
      }
   }
   //Avoid using extreme values
   if (q_NothingFound)
   {
      f64_Retval = 0.0;
   }
   return f64_Retval;
}
