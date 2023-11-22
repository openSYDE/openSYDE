//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for any add operation (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDBASECOMMAND_HPP
#define C_SYVDAUNOADDBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SyvDaUnoAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddBaseCommand :
   public C_SyvDaUnoAddDeleteBaseCommand
{
public:
   C_SyvDaUnoAddBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                            const QString & orc_Text, QUndoCommand * const opc_Parent = NULL,
                            const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData = C_SyvDaDashboardSnapshot());
   ~C_SyvDaUnoAddBaseCommand(void) override;
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
