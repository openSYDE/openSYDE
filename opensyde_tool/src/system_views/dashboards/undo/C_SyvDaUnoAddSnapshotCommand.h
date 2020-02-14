//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add new items based on snapshot information (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDSNAPSHOTCOMMAND_H
#define C_SYVDAUNOADDSNAPSHOTCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SyvDaUnoAddBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddSnapshotCommand :
   public C_SyvDaUnoAddBaseCommand
{
public:
   C_SyvDaUnoAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData,
                                const std::vector<stw_types::uint64> & orc_IDs,
                                const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                           C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                const QPointF & orc_NewPos, const stw_types::float64 of64_HighestUsedZValue,
                                QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SyvDaUnoAddSnapshotCommand(void);

protected:
   virtual void m_AddNew(void) override;

private:
   void m_InitialReadRailHandling(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
