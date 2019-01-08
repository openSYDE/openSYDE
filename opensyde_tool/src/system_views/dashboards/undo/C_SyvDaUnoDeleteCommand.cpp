//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Delete undo command (implementation)

   Delete undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaUnoDeleteCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_IDs    Affected unique IDs
   \param[in,out] opc_Parent Optional pointer to parent

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoDeleteCommand::C_SyvDaUnoDeleteCommand(QGraphicsScene * const opc_Scene,
                                                 const std::vector<stw_types::uint64> & orc_IDs,
                                                 QUndoCommand * const opc_Parent) :
   C_SyvDaUnoAddDeleteBaseCommand(opc_Scene, orc_IDs, "Delete drawing element(s)", opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoDeleteCommand::~C_SyvDaUnoDeleteCommand(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo delete

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoDeleteCommand::redo(void)
{
   this->m_DeleteSave();
   QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo delete

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoDeleteCommand::undo(void)
{
   this->m_Restore();
   QUndoCommand::undo();
}
