//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Add bend point for line undo command (implementation)

   Add bend point for line undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SebUnoAddBendPointCommand.h"

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

   \param[in,out] opc_Scene         Pointer to currently active scene
   \param[in]     orc_IDs           Affected unique IDs
   \param[in]     orc_ScenePosition Scene position to bend or remove bend point at
   \param[in,out] opc_Parent        Optional pointer to parent

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebUnoAddBendPointCommand::C_SebUnoAddBendPointCommand(QGraphicsScene * const opc_Scene,
                                                         const std::vector<uint64> & orc_IDs,
                                                         const QPointF & orc_ScenePosition,
                                                         QUndoCommand * const opc_Parent) :
   C_SebUnoBendPointBaseCommand(opc_Scene, orc_IDs, "Add bend point for line(s)", orc_ScenePosition,
                                opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebUnoAddBendPointCommand::~C_SebUnoAddBendPointCommand()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo bend point

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebUnoAddBendPointCommand::undo(void)
{
   this->m_DeleteBend();
   QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo bend point

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebUnoAddBendPointCommand::redo(void)
{
   this->m_AddBend();
   QUndoCommand::redo();
}
