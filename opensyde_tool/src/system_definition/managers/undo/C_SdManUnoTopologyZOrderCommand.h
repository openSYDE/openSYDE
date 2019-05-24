//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Topology specific Z order undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYZORDERCOMMAND_H
#define C_SDMANUNOTOPOLOGYZORDERCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoZOrderCommand.h"
#include "C_GiLiBusConnector.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyZOrderCommand :
   public C_SebUnoZOrderCommand
{
public:
   C_SdManUnoTopologyZOrderCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                   const std::vector<stw_types::float64> & orc_NewZValues,
                                   QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyZOrderCommand(void);

   static void h_CheckZOrderPriority(const QList<QGraphicsItem *> & orc_SelectedItems,
                                     const QList<QGraphicsItem *> & orc_Items, QMap<QGraphicsItem *,
                                                                                    stw_types::float64> & orc_NewZValues);

private:
   static void mh_CheckZOrderOfBusConnectors(stw_opensyde_gui::C_GiLiBusConnector * const opc_Item,
                                             QMap<QGraphicsItem *,
                                                  stw_types::float64> & orc_NewZValues);
   static void mh_InsertOrReplaceByMinimum(QMap<QGraphicsItem *, stw_types::float64> & orc_Map,
                                           QGraphicsItem * const opc_Key, const stw_types::float64 & orf64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
