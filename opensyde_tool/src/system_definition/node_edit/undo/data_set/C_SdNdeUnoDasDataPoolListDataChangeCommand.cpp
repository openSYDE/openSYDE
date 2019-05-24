//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set data change undo command (implementation)

   Data pool list data set data change undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdNdeUnoDasDataPoolListDataChangeCommand.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNodeDataPoolDataSet.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     oru32_DataPoolListElementIndex   Node data pool list element index
   \param[in]     orc_NewData                      New data
   \param[in]     ore_DataChangeType               Data change type
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListDataChangeCommand::C_SdNdeUnoDasDataPoolListDataChangeCommand(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex, C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager, const uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData, const C_SdNdeDataPoolUtil::E_DataSetDataChangeType & ore_DataChangeType,
                                                                                       QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoDasDataPoolListBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                        opc_DataPoolListModelViewManager,
                                        "Change List Dataset", opc_Parent),
   mc_PreviousData(),
   mc_NewData(orc_NewData),
   mu32_DataPoolListDataSetIndex(oru32_DataPoolListDataSetIndex),
   me_DataChangeType(ore_DataChangeType)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListDataChangeCommand::redo(void)
{
   m_Change(this->mc_PreviousData, this->mc_NewData);
   C_SdNdeUnoDasDataPoolListBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListDataChangeCommand::undo(void)
{
   C_SdNdeUnoDasDataPoolListBaseCommand::undo();
   m_Change(this->mc_NewData, this->mc_PreviousData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change data values and store previous value

   \param[out] orc_PreviousData Previous data value storage
   \param[in]  orc_NewData      New data value assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListDataChangeCommand::m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData)
{
   const C_OSCNodeDataPoolDataSet * const pc_OSCData = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListDataSet(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex,
      this->mu32_DataPoolListIndex,
      this->mu32_DataPoolListDataSetIndex);

   if (pc_OSCData != NULL)
   {
      C_OSCNodeDataPoolDataSet c_OSCData = *pc_OSCData;
      //Copy previous value
      switch (this->me_DataChangeType)
      {
      case C_SdNdeDataPoolUtil::eDATA_SET_NAME:
         orc_PreviousData = QString(c_OSCData.c_Name.c_str());
         break;
      case C_SdNdeDataPoolUtil::eDATA_SET_COMMENT:
         orc_PreviousData = QString(c_OSCData.c_Comment.c_str());
         break;
      default:
         break;
      }
      //Copy new value
      switch (this->me_DataChangeType)
      {
      case C_SdNdeDataPoolUtil::eDATA_SET_NAME:
         c_OSCData.c_Name = orc_NewData.toString().toStdString().c_str();
         break;
      case C_SdNdeDataPoolUtil::eDATA_SET_COMMENT:
         c_OSCData.c_Comment = orc_NewData.toString().toStdString().c_str();
         break;
      default:
         break;
      }
      C_PuiSdHandler::h_GetInstance()->SetOSCNodeDataPoolDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex,
                                                                 this->mu32_DataPoolListDataSetIndex,
                                                                 c_OSCData);

      //Signal data change
      if (this->mpc_DataPoolListModelViewManager != NULL)
      {
         this->mpc_DataPoolListModelViewManager->GetDataSetModel(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex)->HandleDataChange(
            this->mu32_DataPoolListDataSetIndex,
            this->me_DataChangeType);
      }
      //Register error change
      if (this->mpc_DataPoolListModelViewManager != NULL)
      {
         this->mpc_DataPoolListModelViewManager->GetDataSetModel(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_DataPoolListIndex)->HandleErrorChange();
      }
   }
}
