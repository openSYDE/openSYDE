//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data change undo command (implementation)

   Data pool list data change undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QElapsedTimer>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "TGLUtils.h"
#include "C_SdNdeUnoDataPoolListDataChangeCommand.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     oru32_NodeIndex                Node index
   \param[in]     oru32_DataPoolIndex            Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget    Data pool lists tree widget to perform actions on
   \param[in]     oru32_DataPoolListIndex        Data pool list index
   \param[in]     orc_NewData                    New data
   \param[in]     ore_DataChangeType             Data change type
   \param[in,out] opc_Parent                     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListDataChangeCommand::C_SdNdeUnoDataPoolListDataChangeCommand(const uint32 & oru32_NodeIndex,
                                                                                 const uint32 & oru32_DataPoolIndex,
                                                                                 stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget, const uint32 & oru32_DataPoolListIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ListDataChangeType & ore_DataChangeType,
                                                                                 QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, opc_DataPoolListsTreeWidget,
                                     "Change List data", opc_Parent),
   mu32_DataPoolListIndex(oru32_DataPoolListIndex),
   mc_NewData(orc_NewData),
   me_DataChangeType(ore_DataChangeType)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDataChangeCommand::redo(void)
{
   m_Change(this->mc_PreviousData, this->mc_NewData);
   //Apply parent taks = auto min max after current change
   C_SdNdeUnoDataPoolListBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDataChangeCommand::undo(void)
{
   m_Change(this->mc_NewData, this->mc_PreviousData);
   //Apply parent taks = auto min max after current change
   C_SdNdeUnoDataPoolListBaseCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change data values and store previous value

   \param[out] orc_PreviousData Previous data value storage
   \param[in]  orc_NewData      New data value assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDataChangeCommand::m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData)
{
   const uint16 u16_TimerId = osc_write_log_performance_start();

   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex,
      this->mu32_DataPoolListIndex);

   if (pc_List != NULL)
   {
      //Save previous
      mh_ConvertListTypeToGeneric(*pc_List, this->me_DataChangeType, orc_PreviousData);

      //Apply new
      switch (this->me_DataChangeType)
      {
      case C_SdNdeDpUtil::eLIST_NAME:
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListName(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_DataPoolListIndex,
                                                                         orc_NewData.toString()) == C_NO_ERR);
         break;
      case C_SdNdeDpUtil::eLIST_COMMENT:
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListComment(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_DataPoolListIndex,
                                                                            orc_NewData.toString()) == C_NO_ERR);
         break;
      case C_SdNdeDpUtil::eLIST_SIZE:
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListNVMSize(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_DataPoolListIndex,
                                                                            static_cast<uint32>(orc_NewData.toULongLong())) ==
                    C_NO_ERR);
         break;
      case C_SdNdeDpUtil::eLIST_CRC:
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListNVMCRC(this->mu32_NodeIndex,
                                                                           this->mu32_DataPoolIndex,
                                                                           this->mu32_DataPoolListIndex,
                                                                           orc_NewData.toBool()) == C_NO_ERR);
         break;
      default:
         //Unknown
         break;
      }
      if (this->mpc_DataPoolListsTreeWidget != NULL)
      {
         this->mpc_DataPoolListsTreeWidget->RegisterSizeChange();
         this->mpc_DataPoolListsTreeWidget->UpdateUI();
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Any change of list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert list type to generic

   \param[in]  orc_OSCElement OSC Element
   \param[in]  ore_Type       Type to convert
   \param[out] orc_Generic    Generic output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDataChangeCommand::mh_ConvertListTypeToGeneric(const C_OSCNodeDataPoolList & orc_OSCElement,
                                                                          const C_SdNdeDpUtil::E_ListDataChangeType & ore_Type,
                                                                          QVariant & orc_Generic)
{
   switch (ore_Type)
   {
   case C_SdNdeDpUtil::eLIST_NAME:
      orc_Generic = orc_OSCElement.c_Name.c_str();
      break;
   case C_SdNdeDpUtil::eLIST_COMMENT:
      orc_Generic = orc_OSCElement.c_Comment.c_str();
      break;
   case C_SdNdeDpUtil::eLIST_SIZE:
      orc_Generic = static_cast<uint64>(orc_OSCElement.u32_NvMSize);
      break;
   case C_SdNdeDpUtil::eLIST_CRC:
      orc_Generic = orc_OSCElement.q_NvMCRCActive;
      break;
   default:
      break;
   }
}
