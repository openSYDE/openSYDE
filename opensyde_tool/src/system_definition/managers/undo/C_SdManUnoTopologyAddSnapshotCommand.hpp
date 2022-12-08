//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add new items based on snapshot information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDSNAPSHOTCOMMAND_H
#define C_SDMANUNOTOPOLOGYADDSNAPSHOTCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyAddBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddSnapshotCommand :
   public C_SdManUnoTopologyAddBaseCommand
{
public:
   C_SdManUnoTopologyAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                        const C_SdTopologyDataSnapshot & orc_InitialSnapshotData,
                                        const std::vector<uint64_t> & orc_Ids, const QPointF & orc_NewPos,
                                        const float64_t of64_HighestUsedZetValue,
                                        QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyAddSnapshotCommand(void);

protected:
   void m_AddNew(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
