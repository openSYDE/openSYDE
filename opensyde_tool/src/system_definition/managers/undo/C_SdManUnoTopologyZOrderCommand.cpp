//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Topology specific Z order undo command (implementation)

   Z order undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "gitypes.h"
#include "C_SdManUnoTopologyZOrderCommand.h"

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

   \param[in,out] opc_Scene         Pointer to currently active scene
   \param[in]     orc_IDs           Affected unique IDs
   \param[in]     orc_NewZValues    New Z values
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyZOrderCommand::C_SdManUnoTopologyZOrderCommand(QGraphicsScene * const opc_Scene,
                                                                 const vector<uint64> & orc_IDs,
                                                                 const vector<float64> & orc_NewZValues,
                                                                 QUndoCommand * const opc_Parent) :
   C_SebUnoZOrderCommand(opc_Scene, orc_IDs, orc_NewZValues, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyZOrderCommand::~C_SdManUnoTopologyZOrderCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyZOrderCommand::h_CheckZOrderPriority(const QList<QGraphicsItem *> & orc_SelectedItems,
                                                            const QList<QGraphicsItem *> & orc_Items,
                                                            QMap<QGraphicsItem *,
                                                                 float64> & orc_NewZValues)
{
   QList<QGraphicsItem *>::const_iterator c_ItSelectedItem;
   QList<QGraphicsItem *>::const_iterator c_ItItem;

   for (c_ItSelectedItem = orc_SelectedItems.begin(); c_ItSelectedItem != orc_SelectedItems.end(); ++c_ItSelectedItem)
   {
      C_GiLiBusConnector * pc_BusConnectorItem;

      if (((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_NODE) ||
          ((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_BUS) ||
          ((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_CANBUS) ||
          ((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_ETHERNETBUS))
      {
         // search all bus connectors of the node or bus
         for (c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end(); ++c_ItItem)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_BusConnectorItem = dynamic_cast<C_GiLiBusConnector *>(*c_ItItem);

            if (pc_BusConnectorItem != NULL)
            {
               if (((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_NODE) &&
                   (pc_BusConnectorItem->GetNodeItem() == (*c_ItSelectedItem)))
               {
                  // connector of node
                  mh_CheckZOrderOfBusConnectors(pc_BusConnectorItem, orc_NewZValues);
               }
               else if (pc_BusConnectorItem->GetBusItem() == (*c_ItSelectedItem))
               {
                  // connector of bus
                  mh_CheckZOrderOfBusConnectors(pc_BusConnectorItem, orc_NewZValues);
               }
               else
               {
                  // nothing to do
               }
            }
         }
      }
      else if ((*c_ItSelectedItem)->type() == msn_GRAPHICS_ITEM_BUS_CONNECT)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_BusConnectorItem = dynamic_cast<C_GiLiBusConnector *>(*c_ItSelectedItem);
         mh_CheckZOrderOfBusConnectors(pc_BusConnectorItem, orc_NewZValues);
      }
      else
      {
         // no further element types to check
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyZOrderCommand::mh_CheckZOrderOfBusConnectors(C_GiLiBusConnector * const opc_Item,
                                                                    QMap<QGraphicsItem *,
                                                                         float64> & orc_NewZValues)
{
   if (opc_Item != NULL)
   {
      // Check the z order value against the node. It has to be smaller
      if (opc_Item->zValue() >= opc_Item->GetNodeItem()->zValue())
      {
         mh_InsertOrReplaceByMinimum(orc_NewZValues, opc_Item, opc_Item->GetNodeItem()->zValue() - 1.0);
      }

      // Check the z order value against the bus. It has to be smaller
      if (opc_Item->zValue() >= opc_Item->GetBusItem()->zValue())
      {
         mh_InsertOrReplaceByMinimum(orc_NewZValues, opc_Item, opc_Item->GetBusItem()->zValue() - 1.0);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search map for key and replace by minimum

   \param[in,out] orc_Map     Map containing all relevant values
   \param[in,out] opc_Key     Key to replace
   \param[in]     orf64_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyZOrderCommand::mh_InsertOrReplaceByMinimum(QMap<QGraphicsItem *, float64> & orc_Map,
                                                                  QGraphicsItem * const opc_Key,
                                                                  const float64 & orf64_Value)
{
   QMap<QGraphicsItem *, float64>::iterator c_Found = orc_Map.find(opc_Key);
   if (c_Found != orc_Map.end())
   {
      float64 f64_Improved = std::min(*c_Found, orf64_Value);
      orc_Map.erase(c_Found);
      orc_Map.insert(opc_Key, f64_Improved);
   }
   else
   {
      orc_Map.insert(opc_Key, orf64_Value);
   }
}
