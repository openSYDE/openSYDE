//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Z order undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOZETORDERCOMMAND_H
#define C_SEBUNOZETORDERCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.hpp"
#include "C_GiLiBusConnector.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoZetOrderCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoZetOrderCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                           const std::vector<float64_t> & orc_NewZetValues, QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoZetOrderCommand(void) override;
   void undo(void) override;
   void redo(void) override;
   static void h_AdaptZetOrder(const QGraphicsScene * const opc_Scene, const QList<QGraphicsItem *> & orc_Items,
                               const QList<QGraphicsItem *> & orc_SelectedItems, const bool oq_BringToFront,
                               QMap<QGraphicsItem *,
                                    float64_t> & orc_NewZetValues);

private:
   std::vector<float64_t> mc_OldZetValues;
   const std::vector<float64_t> mc_NewZetValues;

   void m_ApplyZetValues(const std::vector<float64_t> & orc_Values) const;
   static void mh_CreateZetValueMap(const QGraphicsScene * const opc_Scene, const QList<QGraphicsItem *> & orc_Items,
                                    const QList<QGraphicsItem *> & orc_SelectedItems, const bool oq_BringToFront,
                                    QMap<QGraphicsItem *,
                                         float64_t> & orc_NewZetValues);
   static float64_t mh_GetLowestUsedZetValueList(const QGraphicsScene * const opc_Scene,
                                                 const QList<QGraphicsItem *> & orc_Items);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
