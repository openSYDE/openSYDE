//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for any add operation (implementation)

   Base class for any add operation

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaUnoAddBaseCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_IDs                 Affected unique IDs
   \param[in]     orc_Text                Command description
   \param[in,out] opc_Parent              Optional pointer to parent
   \param[in]     orc_InitialSnapshotData Initial snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddBaseCommand::C_SyvDaUnoAddBaseCommand(QGraphicsScene * const opc_Scene,
                                                   const std::vector<uint64> & orc_IDs, const QString & orc_Text,
                                                   QUndoCommand * const opc_Parent,
                                                   const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData) :
   C_SyvDaUnoAddDeleteBaseCommand(opc_Scene, orc_IDs, orc_Text, opc_Parent, orc_InitialSnapshotData),
   mq_InitialStep(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddBaseCommand::~C_SyvDaUnoAddBaseCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddBaseCommand::redo(void)
{
   //Special initial step without any previous data
   if (mq_InitialStep == true)
   {
      this->m_AddNew();
      this->mq_InitialStep = false;
   }
   else
   {
      this->m_Restore();
   }
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddBaseCommand::undo(void)
{
   this->m_DeleteSave();
   QUndoCommand::undo();
}
