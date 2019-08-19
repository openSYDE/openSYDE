//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Z order undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOZORDERCOMMAND_H
#define C_SEBUNOZORDERCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"
#include "C_GiLiBusConnector.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoZOrderCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoZOrderCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                         const std::vector<stw_types::float64> & orc_NewZValues,
                         QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoZOrderCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;
   static void h_AdaptZOrder(const QGraphicsScene * const opc_Scene, const QList<QGraphicsItem *> & orc_Items,
                             const QList<QGraphicsItem *> & orc_SelectedItems, const bool oq_BringToFront,
                             QMap<QGraphicsItem *,
                                  stw_types::float64> & orc_NewZValues);

private:
   std::vector<stw_types::float64> mc_OldZValues;
   const std::vector<stw_types::float64> mc_NewZValues;

   void ApplyZValues(const std::vector<stw_types::float64> & orc_Values) const;
   static void mh_CreateZValueMap(const QGraphicsScene * const opc_Scene, const QList<QGraphicsItem *> & orc_Items,
                                  const QList<QGraphicsItem *> & orc_SelectedItems, const bool oq_BringToFront,
                                  QMap<QGraphicsItem *,
                                       stw_types::float64> & orc_NewZValues);
   static stw_types::float64 mh_GetLowestUsedZValueList(const QGraphicsScene * const opc_Scene,
                                                        const QList<QGraphicsItem *> & orc_Items);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
