//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list array edit data change undo command (implementation)

   Data pool list array edit data change undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdNdeUnoAedDataPoolListDataChangeCommand.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNodeDataPoolDataSet.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_ListIndex                  Node data pool list index
   \param[in]     oru32_ElementIndex               Node data pool list element index
   \param[in]     ore_ArrayEditType                Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex               If min or max use 0
                                                   Else use data set index
   \param[in]     oru32_ArrayElementIndex          Array index
   \param[in]     orc_NewData                      New data
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in,out] opc_Parent                       Optional pointer to parent

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoAedDataPoolListDataChangeCommand::C_SdNdeUnoAedDataPoolListDataChangeCommand(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex, const uint32 & oru32_ElementIndex, const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex, const uint32 & oru32_ArrayElementIndex, const QVariant & orc_NewData, C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                       QUndoCommand * const opc_Parent)
   :
   QUndoCommand("Change array element", opc_Parent),
   mc_PreviousData(),
   mc_NewData(orc_NewData),
   mq_Initial(true),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_ListIndex(oru32_ListIndex),
   mu32_ElementIndex(oru32_ElementIndex),
   me_ArrayEditType(ore_ArrayEditType),
   mu32_DataSetIndex(oru32_DataSetIndex),
   mu32_ItemIndex(oru32_ArrayElementIndex),
   mpc_DataPoolListModelViewManager(opc_DataPoolListModelViewManager)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListDataChangeCommand::redo(void)
{
   m_Change(this->mc_PreviousData, this->mc_NewData);
   QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListDataChangeCommand::undo(void)
{
   QUndoCommand::undo();
   m_Change(this->mc_NewData, this->mc_PreviousData);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change data values and store previous value

   \param[out] orc_PreviousData Previous data value storage
   \param[in]  orc_NewData      New data value assignment

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListDataChangeCommand::m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData)
{
   const C_OSCNodeDataPoolListElement * const pc_OSCData = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex,
      this->mu32_ListIndex,
      this->mu32_ElementIndex);

   if (pc_OSCData != NULL)
   {
      bool q_ApplyMinToDataSet = false;
      bool q_ApplyMaxToDataSet = false;
      C_OSCNodeDataPoolContent c_Generic;
      c_Generic.SetType(pc_OSCData->GetType());
      c_Generic.SetArray(false);
      //Copy previous value
      switch (this->me_ArrayEditType)
      {
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MIN:
         orc_PreviousData = C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(pc_OSCData->c_MinValue,
                                                                                        pc_OSCData->f64_Factor,
                                                                                        pc_OSCData->f64_Offset,
                                                                                        this->mu32_ItemIndex);
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MAX:
         orc_PreviousData = C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(pc_OSCData->c_MaxValue,
                                                                                        pc_OSCData->f64_Factor,
                                                                                        pc_OSCData->f64_Offset,
                                                                                        this->mu32_ItemIndex);
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET:
         if (this->mu32_DataSetIndex < pc_OSCData->c_DataSetValues.size())
         {
            orc_PreviousData =
               C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(
                  pc_OSCData->c_DataSetValues[this->mu32_DataSetIndex],
                  pc_OSCData->f64_Factor, pc_OSCData->f64_Offset,
                  this->mu32_ItemIndex);
         }
         break;
      }
      //Copy new value
      C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGenericWithScaling(orc_NewData, c_Generic,
                                                                          pc_OSCData->f64_Factor,
                                                                          pc_OSCData->f64_Offset, 0);
      switch (this->me_ArrayEditType)
      {
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MIN:
         C_PuiSdHandler::h_GetInstance()->SetDataPoolListElementMinArray(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex, this->mu32_ElementIndex,
                                                                         this->mu32_ItemIndex, c_Generic);
         q_ApplyMinToDataSet = true;
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MAX:
         C_PuiSdHandler::h_GetInstance()->SetDataPoolListElementMaxArray(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex, this->mu32_ElementIndex,
                                                                         this->mu32_ItemIndex, c_Generic);
         q_ApplyMaxToDataSet = true;
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET:
         C_PuiSdHandler::h_GetInstance()->SetDataPoolListElementDataSetArray(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex,
                                                                             this->mu32_ElementIndex,
                                                                             this->mu32_DataSetIndex,
                                                                             this->mu32_ItemIndex, c_Generic);
         break;
      }
      //Adapt data set values to min/max changes (only if necessary)
      if (q_ApplyMinToDataSet == true)
      {
         if (this->mq_Initial == true)
         {
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex,
                                                                          this->mu32_ElementIndex);
            if (pc_Element != NULL)
            {
               const QVariant c_NewData = C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(
                  pc_Element->c_MinValue,
                  pc_Element->f64_Factor,
                  pc_Element->f64_Offset,
                  this->mu32_ItemIndex);
               for (uint32 u32_ItDataSet = 0; u32_ItDataSet < pc_Element->c_DataSetValues.size(); ++u32_ItDataSet)
               {
                  if (C_SdNdeDataPoolUtil::h_CompareSpecifiedItemSmaller(pc_Element->c_DataSetValues[u32_ItDataSet],
                                                                         pc_Element->c_MinValue,
                                                                         this->mu32_ItemIndex) == true)
                  {
                     new C_SdNdeUnoAedDataPoolListDataChangeCommand(this->mu32_NodeIndex,
                                                                    this->mu32_DataPoolIndex,
                                                                    this->mu32_ListIndex,
                                                                    this->mu32_ElementIndex,
                                                                    C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET,
                                                                    u32_ItDataSet, this->mu32_ItemIndex, c_NewData,
                                                                    this->mpc_DataPoolListModelViewManager, this);
                  }
               }
            }
         }
      }
      if (q_ApplyMaxToDataSet == true)
      {
         if (this->mq_Initial == true)
         {
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex,
                                                                          this->mu32_ElementIndex);
            if (pc_Element != NULL)
            {
               const QVariant c_NewData = C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(
                  pc_Element->c_MaxValue,
                  pc_Element->f64_Factor,
                  pc_Element->f64_Offset,
                  this->mu32_ItemIndex);
               for (uint32 u32_ItDataSet = 0; u32_ItDataSet < pc_Element->c_DataSetValues.size(); ++u32_ItDataSet)
               {
                  if (C_SdNdeDataPoolUtil::h_CompareSpecifiedItemSmaller(pc_Element->c_MaxValue,
                                                                         pc_Element->c_DataSetValues[u32_ItDataSet],
                                                                         this->mu32_ItemIndex) == true)
                  {
                     new C_SdNdeUnoAedDataPoolListDataChangeCommand(this->mu32_NodeIndex,
                                                                    this->mu32_DataPoolIndex,
                                                                    this->mu32_ListIndex,
                                                                    this->mu32_ElementIndex,
                                                                    C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET,
                                                                    u32_ItDataSet, this->mu32_ItemIndex, c_NewData,
                                                                    this->mpc_DataPoolListModelViewManager, this);
                  }
               }
            }
         }
      }
      this->mq_Initial = false;

      //Signal data change
      if (this->mpc_DataPoolListModelViewManager != NULL)
      {
         this->mpc_DataPoolListModelViewManager->GetArrayEditModel(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                   this->mu32_ListIndex, this->mu32_ElementIndex,
                                                                   this->me_ArrayEditType,
                                                                   this->mu32_DataSetIndex)->HandleDataChange(
            this->mu32_ItemIndex);
      }
      //Error change
      if (this->mpc_DataPoolListModelViewManager != NULL)
      {
         this->mpc_DataPoolListModelViewManager->GetElementModel(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                 this->mu32_ListIndex)->HandleDataChange(
            this->mu32_ElementIndex, C_SdNdeDataPoolUtil::eELEMENT_DATA_SET, this->mu32_DataSetIndex);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert to generic data change type

   \return
   generic data change type

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolUtil::E_ElementDataChangeType C_SdNdeUnoAedDataPoolListDataChangeCommand::m_ConvertToChangeType(void)
const
{
   C_SdNdeDataPoolUtil::E_ElementDataChangeType e_Retval = C_SdNdeDataPoolUtil::eELEMENT_MIN;
   switch (this->me_ArrayEditType)
   {
   case C_SdNdeDataPoolUtil::eARRAY_EDIT_MIN:
      e_Retval = C_SdNdeDataPoolUtil::eELEMENT_MIN;
      break;
   case C_SdNdeDataPoolUtil::eARRAY_EDIT_MAX:
      e_Retval = C_SdNdeDataPoolUtil::eELEMENT_MAX;
      break;
   case C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET:
      e_Retval = C_SdNdeDataPoolUtil::eELEMENT_DATA_SET;
      break;
   default:
      break;
   }
   return e_Retval;
}
