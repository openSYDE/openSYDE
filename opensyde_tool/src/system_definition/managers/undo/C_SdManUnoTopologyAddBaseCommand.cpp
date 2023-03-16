//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for any add operation (implementation)

   Base class for any add operation

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdManUnoTopologyAddBaseCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Scene               Pointer to currently active scene
   \param[in]     orc_Ids                 Affected unique IDs
   \param[in]     orc_Text                Command description
   \param[in,out] opc_Parent              Optional pointer to parent
   \param[in]     orc_InitialSnapshotData Initial snapshot data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddBaseCommand::C_SdManUnoTopologyAddBaseCommand(QGraphicsScene * const opc_Scene,
                                                                   const std::vector<uint64_t> & orc_Ids,
                                                                   const QString & orc_Text,
                                                                   QUndoCommand * const opc_Parent,
                                                                   const C_SdTopologyDataSnapshot & orc_InitialSnapshotData)
   :
   C_SdManUnoTopologyAddDeleteBaseCommand(opc_Scene, orc_Ids, orc_Text,
                                          opc_Parent, orc_InitialSnapshotData),
   mq_InitialStep(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologyAddBaseCommand::~C_SdManUnoTopologyAddBaseCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologyAddBaseCommand::redo(void)
{
   //Special initial step without any previous data
   if (mq_InitialStep == true)
   {
      this->m_AddNew();
      this->mq_InitialStep = false;
      Q_EMIT (this->SigErrorChange());
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
void C_SdManUnoTopologyAddBaseCommand::undo(void)
{
   this->m_DeleteSave();
   QUndoCommand::undo();
}
