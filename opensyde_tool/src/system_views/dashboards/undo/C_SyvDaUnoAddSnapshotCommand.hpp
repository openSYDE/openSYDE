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

#include "C_SyvDaUnoAddBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddSnapshotCommand :
   public C_SyvDaUnoAddBaseCommand
{
public:
   C_SyvDaUnoAddSnapshotCommand(QGraphicsScene * const opc_Scene,
                                const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData,
                                const std::vector<uint64_t> & orc_Ids,
                                const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                           C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                const QPointF & orc_NewPos, const float64_t of64_HighestUsedZetValue,
                                QUndoCommand * const opc_Parent = NULL);
   ~C_SyvDaUnoAddSnapshotCommand(void) override;

protected:
   void m_AddNew(void) override;

private:
   void m_InitialReadRailHandling(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
