//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Z order undo command (implementation)

   Z order undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "gitypes.hpp"
#include "C_GiBiBase.hpp"
#include "C_SebUnoZetOrderSortHelper.hpp"
#include "C_SebUnoZetOrderCommand.hpp"
#include "C_SebScene.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene      Pointer to currently active scene
   \param[in]     orc_Ids        Affected unique IDs
   \param[in]     orc_NewZetValues New Z values
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZetOrderCommand::C_SebUnoZetOrderCommand(QGraphicsScene * const opc_Scene, const vector<uint64_t> & orc_Ids,
                                                 const vector<float64_t> & orc_NewZetValues,
                                                 QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_Ids, "Reorder drawing element(s)", opc_Parent),
   mc_NewZetValues(orc_NewZetValues)
{
   vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();
   if (c_AffectedItems.size() == this->mc_NewZetValues.size())
   {
      this->mc_OldZetValues.resize(this->mc_NewZetValues.size());
      for (uint32_t u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         this->mc_OldZetValues[u32_ItAffectedItem] = c_AffectedItems[u32_ItAffectedItem]->zValue();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZetOrderCommand::~C_SebUnoZetOrderCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderCommand::undo(void)
{
   this->m_ApplyZetValues(this->mc_OldZetValues);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderCommand::redo(void)
{
   this->m_ApplyZetValues(this->mc_NewZetValues);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt z order of selected items

   \param[in]     opc_Scene         Pointer to currently active scene
   \param[in]     orc_Items         All items
   \param[in]     orc_SelectedItems All selected items
   \param[in]     oq_BringToFront   Flag if this is the bring to front action (otherwise send to back assumed)
   \param[in,out] orc_NewZetValues    Storage for a map of all new z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderCommand::h_AdaptZetOrder(const QGraphicsScene * const opc_Scene,
                                              const QList<QGraphicsItem *> & orc_Items,
                                              const QList<QGraphicsItem *> & orc_SelectedItems,
                                              const bool oq_BringToFront, QMap<QGraphicsItem *,
                                                                               float64_t> & orc_NewZetValues)
{
   const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

   if (pc_Scene != NULL)
   {
      QList<QGraphicsItem *> c_SelectedItems = orc_SelectedItems;
      //Step 1: filter all relevant items for z order
      pc_Scene->FilterChangableZetValues(c_SelectedItems);
      //Step 2: sort all items via z order
      C_SebUnoZetOrderSortHelper::h_SortZetValues(c_SelectedItems);
      //Step 3: create map for target z values
      C_SebUnoZetOrderCommand::mh_CreateZetValueMap(opc_Scene, orc_Items, c_SelectedItems, oq_BringToFront,
                                                    orc_NewZetValues);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Central call to change Z values

   \param[in] orc_Values New Z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderCommand::m_ApplyZetValues(const std::vector<float64_t> & orc_Values) const
{
   const vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();

   if (c_AffectedItems.size() == orc_Values.size())
   {
      for (uint32_t u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         C_GiBiBase * const pc_GiBase = dynamic_cast<C_GiBiBase * const>(c_AffectedItems[u32_ItAffectedItem]);
         if (pc_GiBase != NULL)
         {
            pc_GiBase->SetZetValueCustom(orc_Values[u32_ItAffectedItem]);
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
   \param[in,out] orc_NewZetValues    Storage for a map of all new z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderCommand::mh_CreateZetValueMap(const QGraphicsScene * const opc_Scene,
                                                   const QList<QGraphicsItem *> & orc_Items,
                                                   const QList<QGraphicsItem *> & orc_SelectedItems,
                                                   const bool oq_BringToFront, QMap<QGraphicsItem *,
                                                                                    float64_t> & orc_NewZetValues)
{
   if (oq_BringToFront)
   {
      const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

      if (pc_Scene != NULL)
      {
         float64_t f64_Value = pc_Scene->GetHighestUsedZetValueList(orc_Items);
         //Start with first item -> most obscured one
         for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_SelectedItems.begin();
              c_ItItem != orc_SelectedItems.end(); ++c_ItItem)
         {
            //Iterate to next higher z value (the first one is already used!)
            ++f64_Value;

            //Insert new unused value
            orc_NewZetValues.insert((*c_ItItem), f64_Value);
         }
      }
   }
   else
   {
      float64_t f64_Value = C_SebUnoZetOrderCommand::mh_GetLowestUsedZetValueList(opc_Scene, orc_Items);
      //Start with last item -> most visible one
      for (QList<QGraphicsItem *>::const_reverse_iterator c_ItItem = orc_SelectedItems.rbegin();
           c_ItItem != orc_SelectedItems.rend(); ++c_ItItem)
      {
         //Iterate to next lower z value (the first one is already used!)
         --f64_Value;

         //Insert new unused value
         orc_NewZetValues.insert((*c_ItItem), f64_Value);
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
float64_t C_SebUnoZetOrderCommand::mh_GetLowestUsedZetValueList(const QGraphicsScene * const opc_Scene,
                                                                const QList<QGraphicsItem *> & orc_Items)
{
   bool q_NothingFound = true;
   float64_t f64_Retval = std::numeric_limits<float64_t>::max();
   const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(opc_Scene);

   if (pc_Scene != NULL)
   {
      for (QList<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end(); ++c_ItItem)
      {
         const QGraphicsItem * const pc_Item = *c_ItItem;
         if ((pc_Item != NULL) && (pc_Scene->IsZetOrderChangeable(pc_Item) == true))
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
