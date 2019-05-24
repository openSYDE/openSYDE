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

#include "C_SdManUnoTopologyAddBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddSnapshotCommand :
   public C_SdManUnoTopologyAddBaseCommand
{
public:
   C_SdManUnoTopologyAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                        const C_SdTopologyDataSnapshot & orc_InitialSnapshotData,
                                        const std::vector<stw_types::uint64> & orc_IDs, const QPointF & orc_NewPos,
                                        QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyAddSnapshotCommand(void);

protected:
   virtual void m_AddNew(void);

private:
   const QPointF mc_NewPos;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
