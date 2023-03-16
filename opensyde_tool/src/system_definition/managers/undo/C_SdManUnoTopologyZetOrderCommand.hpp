//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Topology specific Z order undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYZETORDERCOMMAND_H
#define C_SDMANUNOTOPOLOGYZETORDERCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoZetOrderCommand.hpp"
#include "C_GiLiBusConnector.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyZetOrderCommand :
   public C_SebUnoZetOrderCommand
{
public:
   C_SdManUnoTopologyZetOrderCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                     const std::vector<float64_t> & orc_NewZetValues,
                                     QUndoCommand * const opc_Parent = NULL);
   ~C_SdManUnoTopologyZetOrderCommand(void) override;

   static void h_CheckZetOrderPriority(const QList<QGraphicsItem *> & orc_SelectedItems,
                                       const QList<QGraphicsItem *> & orc_Items, QMap<QGraphicsItem *,
                                                                                      float64_t> & orc_NewZetValues);

private:
   static void mh_CheckZetOrderOfBusConnectors(stw::opensyde_gui::C_GiLiBusConnector * const opc_Item,
                                               QMap<QGraphicsItem *,
                                                    float64_t> & orc_NewZetValues);
   static void mh_InsertOrReplaceByMinimum(QMap<QGraphicsItem *, float64_t> & orc_Map, QGraphicsItem * const opc_Key,
                                           const float64_t & orf64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
