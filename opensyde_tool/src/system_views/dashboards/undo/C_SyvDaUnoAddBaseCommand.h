//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for any add operation (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDBASECOMMAND_H
#define C_SYVDAUNOADDBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SyvDaUnoAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaUnoAddBaseCommand :
   public C_SyvDaUnoAddDeleteBaseCommand
{
public:
   C_SyvDaUnoAddBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                            const QString & orc_Text, QUndoCommand * const opc_Parent = NULL,
                            const C_PuiSvDashboard & orc_InitialSnapshotData = C_PuiSvDashboard());
   virtual ~C_SyvDaUnoAddBaseCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

protected:
   virtual void m_AddNew(void) = 0;

private:
   bool mq_InitialStep;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
