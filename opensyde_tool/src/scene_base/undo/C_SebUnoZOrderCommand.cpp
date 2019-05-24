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
/*! \brief   Default constructor

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_IDs    Affected unique IDs
   \param[in,out] opc_Parent Optional pointer to parent
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
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZOrderCommand::~C_SebUnoZOrderCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::undo(void)
{
   vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();
   if (c_AffectedItems.size() == this->mc_OldZValues.size())
   {
      for (uint32 u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         c_AffectedItems[u32_ItAffectedItem]->setZValue(this->mc_OldZValues[u32_ItAffectedItem]);
      }
   }
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo z order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::redo(void)
{
   vector<QGraphicsItem *> c_AffectedItems = this->m_GetSceneItems();
   if (c_AffectedItems.size() == this->mc_NewZValues.size())
   {
      for (uint32 u32_ItAffectedItem = 0; u32_ItAffectedItem < c_AffectedItems.size(); ++u32_ItAffectedItem)
      {
         c_AffectedItems[u32_ItAffectedItem]->setZValue(this->mc_NewZValues[u32_ItAffectedItem]);
      }
   }
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt z order of selected items

   \param[in]     orc_SelectedItems All selected items
   \param[in]     of64_Diff         Difference to add to all selected items
   \param[in,out] orf64_ZOrderHigh  Reference to highest z value
   \param[in,out] orf64_ZOrderLow   Reference to lowest z value
   \param[in,out] orc_NewZValues    Storage for a map of all new z values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderCommand::h_AdaptZOrder(QGraphicsScene * const opc_Scene,
                                          const QList<QGraphicsItem *> & orc_SelectedItems, const float64 of64_Diff,
                                          float64 & orf64_ZOrderHigh, float64 & orf64_ZOrderLow, QMap<QGraphicsItem *,
                                                                                                      float64> & orc_NewZValues)
{
   QList<QGraphicsItem *>::const_iterator c_ItItem;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene * const>(opc_Scene);

   if (pc_Scene != NULL)
   {
      // adapt the z order value
      for (c_ItItem = orc_SelectedItems.begin(); c_ItItem != orc_SelectedItems.end(); ++c_ItItem)
      {
         if (pc_Scene->IsZOrderChangeable(*c_ItItem) == true)
         {
            float64 f64_NewZValue = (*c_ItItem)->zValue() + of64_Diff;

            orc_NewZValues.insert((*c_ItItem), f64_NewZValue);

            // adapt the upper and lower limits
            if (f64_NewZValue < orf64_ZOrderLow)
            {
               orf64_ZOrderLow = f64_NewZValue;
            }
            if (f64_NewZValue > orf64_ZOrderHigh)
            {
               orf64_ZOrderHigh = f64_NewZValue;
            }
         }
      }
   }
}
