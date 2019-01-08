//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for any add operation (implementation)

   Base class for any add operation

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdManUnoTopologyAddBaseCommand.h"

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

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_IDs                 Affected unique IDs
   \param[in]     orc_Text                Command description
   \param[in,out] opc_Parent              Optional pointer to parent
   \param[in]     orc_InitialSnapshotData Initial snapshot data

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyAddBaseCommand::C_SdManUnoTopologyAddBaseCommand(QGraphicsScene * const opc_Scene,
                                                                   const std::vector<uint64> & orc_IDs,
                                                                   const QString & orc_Text,
                                                                   QUndoCommand * const opc_Parent,
                                                                   const C_SdTopologyDataSnapshot & orc_InitialSnapshotData)
   :
   C_SdManUnoTopologyAddDeleteBaseCommand(opc_Scene, orc_IDs, orc_Text,
                                          opc_Parent, orc_InitialSnapshotData),
   mq_InitialStep(true)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdManUnoTopologyAddBaseCommand::~C_SdManUnoTopologyAddBaseCommand(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo add

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyAddBaseCommand::redo(void)
{
   //Special initial step without any previous data
   if (mq_InitialStep == true)
   {
      this->m_AddNew();
      this->mq_InitialStep = false;
      Q_EMIT this->SigErrorChange();
   }
   else
   {
      this->m_Restore();
   }
   QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo add

   \created     24.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdManUnoTopologyAddBaseCommand::undo(void)
{
   this->m_DeleteSave();
   QUndoCommand::undo();
}
