//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add bend point for line undo command (implementation)

   Add bend point for line undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SebUnoAddBendPointCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene         Pointer to currently active scene
   \param[in]     orc_Ids           Affected unique IDs
   \param[in]     orc_ScenePosition Scene position to bend or remove bend point at
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoAddBendPointCommand::C_SebUnoAddBendPointCommand(QGraphicsScene * const opc_Scene,
                                                         const std::vector<uint64_t> & orc_Ids,
                                                         const QPointF & orc_ScenePosition,
                                                         QUndoCommand * const opc_Parent) :
   C_SebUnoBendPointBaseCommand(opc_Scene, orc_Ids, "Add bend point for line(s)", orc_ScenePosition,
                                opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoAddBendPointCommand::~C_SebUnoAddBendPointCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo bend point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoAddBendPointCommand::undo(void)
{
   this->m_DeleteBend();
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo bend point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoAddBendPointCommand::redo(void)
{
   this->m_AddBend();
   QUndoCommand::redo();
}
