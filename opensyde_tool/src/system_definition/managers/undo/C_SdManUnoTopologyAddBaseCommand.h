//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for any add operation (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDBASECOMMAND_H
#define C_SDMANUNOTOPOLOGYADDBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdManUnoTopologyAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddBaseCommand :
   public C_SdManUnoTopologyAddDeleteBaseCommand
{
public:
   C_SdManUnoTopologyAddBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                    const QString & orc_Text, QUndoCommand * const opc_Parent = NULL, const C_SdTopologyDataSnapshot & orc_InitialSnapshotData =
                                       C_SdTopologyDataSnapshot());
   virtual ~C_SdManUnoTopologyAddBaseCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

protected:
   virtual void m_AddNew(void) = 0;

private:
   bool mq_InitialStep;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
