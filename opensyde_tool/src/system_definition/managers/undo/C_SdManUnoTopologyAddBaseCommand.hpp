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

#include "C_SdManUnoTopologyAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddBaseCommand :
   public C_SdManUnoTopologyAddDeleteBaseCommand
{
public:
   C_SdManUnoTopologyAddBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                    const QString & orc_Text, QUndoCommand * const opc_Parent = NULL, const C_SdTopologyDataSnapshot & orc_InitialSnapshotData =
                                       C_SdTopologyDataSnapshot());
   ~C_SdManUnoTopologyAddBaseCommand(void) override;
   void undo(void) override;
   void redo(void) override;

protected:
   virtual void m_AddNew(void) = 0;

private:
   bool mq_InitialStep;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
