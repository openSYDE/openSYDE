//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter tree model (implementation)

   Parameter tree model

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_GiSvDaParam.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpUtil.h"
#include "C_SyvDaItPaTreeModel.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaItPaTreeModel::mhc_ICON_ALL_NODE = "";
const QString C_SyvDaItPaTreeModel::mhc_ICON_NODE = ":/images/system_definition/IconNode.svg";
const QString C_SyvDaItPaTreeModel::mhc_ICON_DATAPOOL = ":/images/system_definition/IconDataPoolSmall.svg";
const QString C_SyvDaItPaTreeModel::mhc_ICON_LIST = ":/images/system_definition/IconDatapoolList.svg";
const QString C_SyvDaItPaTreeModel::mhc_ICON_PARAMETER = ":/images/system_definition/IconParameter.svg";
const QString C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING = "N/A";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaTreeModel::C_SyvDaItPaTreeModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   mpc_DataWidget(NULL),
   mq_DarkMode(false),
   mq_EditMode(false),
   mq_Connected(false),
   mq_SaveLoadActive(true),
   mq_ActionActive(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Never took ownership of mpc_DataWidget
C_SyvDaItPaTreeModel::~C_SyvDaItPaTreeModel(void)
{
   //Clean up if necessary
   delete (this->mpc_InvisibleRootItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if model is empty

   \return
   True  Empty
   False Not empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::IsEmpty(void) const
{
   bool q_Retval;

   if (this->mc_ECUValuesReadStatus.size() == 0UL)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload set data column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::ReloadSetValues(void)
{
   QVector<sintn> c_Roles;
   const sint32 s32_ColSet = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eSET);
   const sint32 s32_ColTree = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eTREE);
   c_Roles.append(static_cast<sintn>(Qt::DisplayRole));

   //Trigger set value change
   Q_EMIT this->dataChanged(this->index(0, s32_ColSet), this->index(this->rowCount() - 1, s32_ColSet), c_Roles);
   //Trigger "*" update
   Q_EMIT this->dataChanged(this->index(0, s32_ColTree), this->index(this->rowCount() - 1, s32_ColTree), c_Roles);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark flag value

   \param[in]  oq_Value    New dark flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetDark(const bool oq_Value)
{
   this->mq_DarkMode = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set edit mode status

   \param[in]  oq_EditMode    Edit mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetEditMode(const bool oq_EditMode)
{
   this->mq_EditMode = oq_EditMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection status

   \param[in]  oq_Connected   Connection active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetConnected(const bool oq_Connected)
{
   this->mq_Connected = oq_Connected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set load and save action status

   \param[in]  oq_Active   Load and save action active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetLoadSaveActive(const bool oq_Active)
{
   this->mq_SaveLoadActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set action status

   \param[in]  oq_Active   Action active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetActionActive(const bool oq_Active)
{
   this->mq_ActionActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete all elements

   \param[in]  orc_ListIds    List IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::DeleteSpecified(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      std::vector<C_OSCNodeDataPoolListId> c_ListIds;
      for (uint32 u32_ItList = 0UL; u32_ItList < orc_ListIds.size(); ++u32_ItList)
      {
         const C_OSCNodeDataPoolListElementId & rc_CurId = orc_ListIds[u32_ItList];
         c_ListIds.push_back(rc_CurId);
      }
      pc_ParamWidget->DeleteLists(c_ListIds);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset all stored ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::ClearECUValues(void)
{
   QVector<sintn> c_RolesDisplay;
   QVector<sintn> c_RolesStyle;
   const sint32 s32_ColSet = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eSET);
   const sint32 s32_ColECU = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eDEVICE_VALUE);
   const sint32 s32_ColTree = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eTREE);
   c_RolesDisplay.append(static_cast<sintn>(Qt::DisplayRole));
   c_RolesStyle.append(static_cast<sintn>(Qt::FontRole));
   if (this->mc_ECUValuesString.size() == this->mc_ECUValuesReadStatus.size())
   {
      for (uint32 u32_ItElement = 0UL; u32_ItElement < this->mc_ECUValuesString.size(); ++u32_ItElement)
      {
         auto && rc_Ref = this->mc_ECUValuesReadStatus[u32_ItElement];
         rc_Ref = false;
         for (uint32 u32_ItValue = 0UL; u32_ItValue < this->mc_ECUValuesString[u32_ItElement].size(); ++u32_ItValue)
         {
            //Overwrite all previous values
            this->mc_ECUValuesString[u32_ItElement][u32_ItValue] = C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING;
         }
      }
   }
   //Reset all valid flags
   for (uint32 u32_ItList = 0; u32_ItList < this->mc_ECUCRCValidStatus.size(); ++u32_ItList)
   {
      auto && rc_Ref = this->mc_ECUCRCValidStatus[u32_ItList];
      rc_Ref = false;
   }
   //Trigger set value change
   Q_EMIT this->dataChanged(this->index(0, s32_ColECU), this->index(this->rowCount() - 1, s32_ColECU), c_RolesDisplay);
   //Trigger "*" update
   Q_EMIT this->dataChanged(this->index(0, s32_ColTree), this->index(this->rowCount() - 1, s32_ColTree),
                            c_RolesDisplay);
   //Trigger bold font update
   Q_EMIT this->dataChanged(this->index(0, s32_ColSet), this->index(this->rowCount() - 1, s32_ColSet), c_RolesStyle);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set set values same as device values

   \param[in]      orc_ListIds            List IDs
   \param[in,out]  orc_InvalidValueIds    Invalid value ids
   \param[in,out]  orc_InvalidValues      Invalid values
   \param[in,out]  orc_NewValues          New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::ApplyEcuValues(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds,
                                          std::vector<C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds,
                                          std::vector<QString> & orc_InvalidValues,
                                          std::vector<QString> & orc_NewValues)
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if ((((pc_Param != NULL) && (pc_Param->c_ListValues.size() == this->mc_ECUValues.size())) &&
           (pc_Param->c_ListValues.size() == this->mc_ECUValuesReadStatus.size())) &&
          (pc_Param->c_ListValues.size() == pc_Param->c_DataPoolElementsConfig.size()))
      {
         bool q_AllRead = true;
         C_PuiSvDbParam c_Copy = *pc_Param;
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_ListValues.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               bool q_Found = false;
               for (uint32 u32_ItList = 0UL; (u32_ItList < orc_ListIds.size()) && (q_Found == false); ++u32_ItList)
               {
                  const C_OSCNodeDataPoolListElementId & rc_ListId = orc_ListIds[u32_ItList];
                  if (((rc_Config.c_ElementId.u32_NodeIndex == rc_ListId.u32_NodeIndex) &&
                       (rc_Config.c_ElementId.u32_DataPoolIndex == rc_ListId.u32_DataPoolIndex)) &&
                      (rc_Config.c_ElementId.u32_ListIndex == rc_ListId.u32_ListIndex))
                  {
                     q_Found = true;
                  }
               }
               if (q_Found == true)
               {
                  //Apply read value
                  c_Copy.c_ListValues[u32_ItConfig] = this->mc_ECUValues[u32_ItConfig];
                  //Check if read value valid
                  if (this->mc_ECUValuesReadStatus[u32_ItConfig] == false)
                  {
                     q_AllRead = false;
                  }
                  else
                  {
                     C_SyvDaItPaTreeModel::h_AdaptFloatRangeOfValueAndAppendResults(c_Copy.c_ListValues[u32_ItConfig],
                                                                                    rc_Config.c_ElementId,
                                                                                    orc_InvalidValueIds,
                                                                                    orc_InvalidValues, orc_NewValues);
                  }
               }
            }
         }
         //Do not apply if not all are read
         if (q_AllRead == true)
         {
            QVector<sintn> c_Roles;
            const sintn sn_Col = C_SyvDaItPaTreeModel::h_EnumToColumn(C_SyvDaItPaTreeModel::eSET);
            c_Roles.push_back(static_cast<sintn>(Qt::DisplayRole));
            c_Roles.push_back(static_cast<sintn>(Qt::FontRole));
            pc_ParamWidget->SetParamItem(c_Copy);
            //Update set value column
            Q_EMIT this->dataChanged(this->index(0, sn_Col), this->index(this->rowCount() - 1, sn_Col), c_Roles);
         }
         else
         {
            //Ignore
            orc_InvalidValueIds.clear();
            orc_InvalidValues.clear();
            orc_NewValues.clear();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all Set values in range

   \param[in]  orc_ListIds    Subset of lists to check range for
   \param[in]  orc_ListIds2   Additional subset of lists to check range for

   \return
   True  In range
   False Not in range
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::CheckRange(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds,
                                      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds2)
const
{
   bool q_Retval = true;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if ((rc_Config.c_ElementId.GetIsValid() == true) && (u32_ItConfig < pc_Param->c_ListValues.size()))
            {
               bool q_IsInteresting = false;
               //Check if the list is part of the first interesting elements
               for (uint32 u32_ItInterestingElements = 0UL;
                    (u32_ItInterestingElements < orc_ListIds.size()) && (q_IsInteresting == false);
                    ++u32_ItInterestingElements)
               {
                  const C_OSCNodeDataPoolListElementId & rc_ListId = orc_ListIds[u32_ItInterestingElements];
                  if (((rc_ListId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                       (rc_ListId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                      (rc_ListId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
                  {
                     q_IsInteresting = true;
                  }
               }
               //Check if the list is part of the second interesting elements
               for (uint32 u32_ItInterestingElements = 0UL;
                    (u32_ItInterestingElements < orc_ListIds2.size()) && (q_IsInteresting == false);
                    ++u32_ItInterestingElements)
               {
                  const C_OSCNodeDataPoolListId & rc_ListId = orc_ListIds2[u32_ItInterestingElements];
                  if (((rc_ListId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                       (rc_ListId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                      (rc_ListId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
                  {
                     q_IsInteresting = true;
                  }
               }
               if (q_IsInteresting)
               {
                  const C_OSCNodeDataPoolContent * const pc_SetVal = m_GetSetValue(u32_ItConfig);
                  if (pc_SetVal != NULL)
                  {
                     const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                                   rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                                   rc_Config.c_ElementId.u32_ListIndex,
                                                                                   rc_Config.c_ElementId.u32_ElementIndex);
                     if (pc_OSCElement != NULL)
                     {
                        //With arrays the check has to be exactly like this
                        if ((pc_OSCElement->c_MinValue <= *pc_SetVal) && (*pc_SetVal <= pc_OSCElement->c_MaxValue))
                        {
                           //No change
                        }
                        else
                        {
                           q_Retval = false;
                        }
                     }
                  }
                  else
                  {
                     q_Retval = false;
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all lists have read values

   \return
   True  Read
   False Not read
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::CheckAllListsRead(void) const
{
   bool q_Retval = true;

   for (uint32 u32_ItElement = 0UL; u32_ItElement < this->mc_ECUValuesReadStatus.size(); ++u32_ItElement)
   {
      if (this->mc_ECUValuesReadStatus[u32_ItElement] == false)
      {
         q_Retval = false;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle value preparation

   \param[in]  orc_ListIds    List IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::PrepareChangedValues(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds) const
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if ((pc_ParamWidget != NULL) && (orc_ListIds.size() > 0UL))
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (((pc_Param != NULL) && (pc_Param->c_DataPoolElementsConfig.size() == this->mc_ECUValuesReadStatus.size())) &&
          (this->mc_ECUValuesReadStatus.size() == this->mc_ECUValues.size()))
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               bool q_Found = false;
               for (uint32 u32_ItList = 0UL; (u32_ItList < orc_ListIds.size()) && (q_Found == false); ++u32_ItList)
               {
                  const C_OSCNodeDataPoolListElementId & rc_InputElementId = orc_ListIds[u32_ItList];

                  if (((rc_InputElementId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                       (rc_InputElementId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                      (rc_InputElementId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
                  {
                     q_Found = true;
                  }
               }
               if (q_Found == true)
               {
                  const C_OSCNodeDataPoolContent * const pc_SetVal = m_GetSetValue(u32_ItConfig);
                  if ((pc_SetVal != NULL) && (this->mc_ECUValuesReadStatus[u32_ItConfig] == true))
                  {
                     //Only apply values if necessary
                     if (this->mc_ECUValues[u32_ItConfig] == *pc_SetVal)
                     {
                        //Skip this one
                     }
                     else
                     {
                        const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                              rc_Config.c_ElementId.u32_NodeIndex,
                              rc_Config.c_ElementId.u32_DataPoolIndex,
                              rc_Config.c_ElementId.u32_ListIndex,
                              rc_Config.c_ElementId.u32_ElementIndex);
                        if (pc_OSCElement != NULL)
                        {
                           tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListElementNVMValue(
                                         rc_Config.c_ElementId.u32_NodeIndex,
                                         rc_Config.c_ElementId.u32_DataPoolIndex,
                                         rc_Config.c_ElementId.u32_ListIndex,
                                         rc_Config.c_ElementId.u32_ElementIndex,
                                         *pc_SetVal) == C_NO_ERR);
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the NVM changed flag for all relevant elements

   \param[in]  orc_ListIds    List IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::RemoveValuesChangedFlag(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
const
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if ((pc_ParamWidget != NULL) && (orc_ListIds.size() > 0UL))
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (((pc_Param != NULL) && (pc_Param->c_DataPoolElementsConfig.size() == this->mc_ECUValuesReadStatus.size())) &&
          (this->mc_ECUValuesReadStatus.size() == this->mc_ECUValues.size()))
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               bool q_Found = false;
               for (uint32 u32_ItList = 0UL; (u32_ItList < orc_ListIds.size()) && (q_Found == false); ++u32_ItList)
               {
                  const C_OSCNodeDataPoolListElementId & rc_InputElementId = orc_ListIds[u32_ItList];

                  if (((rc_InputElementId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                       (rc_InputElementId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                      (rc_InputElementId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
                  {
                     q_Found = true;
                  }
               }
               if (q_Found == true)
               {
                  const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                        rc_Config.c_ElementId.u32_NodeIndex,
                        rc_Config.c_ElementId.u32_DataPoolIndex,
                        rc_Config.c_ElementId.u32_ListIndex,
                        rc_Config.c_ElementId.u32_ElementIndex);
                  if (pc_OSCElement != NULL)
                  {
                     tgl_assert(C_PuiSdHandler::h_GetInstance()->SetDataPoolListElementNVMValueChanged(
                                   rc_Config.c_ElementId.u32_NodeIndex,
                                   rc_Config.c_ElementId.u32_DataPoolIndex,
                                   rc_Config.c_ElementId.u32_ListIndex,
                                   rc_Config.c_ElementId.u32_ElementIndex,
                                   false) == C_NO_ERR);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all available indices (first column only)

   \return
   All available indices (first column only)
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndexList C_SyvDaItPaTreeModel::GetAllAvailableIndixesForOneColumn(void) const
{
   QModelIndexList c_Retval;

   if (this->mpc_InvisibleRootItem != NULL)
   {
      for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItAll = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItAll != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItAll)
      {
         //Top level
         const C_TblTreSimpleItem * const pc_AllNode = *c_ItAll;
         if (pc_AllNode != NULL)
         {
            uint32 u32_ItNode = 0UL;
            const QModelIndex c_AllBase = this->index(0, 0);
            for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItNode = pc_AllNode->c_Children.begin();
                 c_ItNode != pc_AllNode->c_Children.end(); ++c_ItNode)
            {
               //Node level
               const C_TblTreSimpleItem * const pc_Node = *c_ItNode;
               if (pc_Node != NULL)
               {
                  uint32 u32_ItDp = 0UL;
                  const QModelIndex c_Node = this->index(static_cast<sintn>(u32_ItNode), 0, c_AllBase);
                  for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItDp = pc_Node->c_Children.begin();
                       c_ItDp != pc_Node->c_Children.end(); ++c_ItDp)
                  {
                     //Data pool level
                     const C_TblTreSimpleItem * const pc_Dp = *c_ItDp;
                     if (pc_Dp != NULL)
                     {
                        uint32 u32_ItLi = 0UL;
                        const QModelIndex c_Dp = this->index(static_cast<sintn>(u32_ItDp), 0, c_Node);
                        for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItLi = pc_Dp->c_Children.begin();
                             c_ItLi != pc_Dp->c_Children.end(); ++c_ItLi)
                        {
                           //List level
                           const C_TblTreSimpleItem * const pc_Li = *c_ItLi;
                           if (pc_Li != NULL)
                           {
                              const QModelIndex c_Li = this->index(static_cast<sintn>(u32_ItLi), 0, c_Dp);
                              for (uint32 u32_ItEl = 0L; u32_ItEl < pc_Li->c_Children.size(); ++u32_ItEl)
                              {
                                 //Element level
                                 const QModelIndex c_El = this->index(static_cast<sintn>(u32_ItEl), 0, c_Li);
                                 c_Retval.push_back(c_El);
                              }
                              c_Retval.push_back(c_Li);
                           }
                           ++u32_ItLi;
                        }
                        c_Retval.push_back(c_Dp);
                     }
                     ++u32_ItDp;
                  }
                  c_Retval.push_back(c_Node);
               }
               ++u32_ItNode;
            }
            c_Retval.push_back(c_AllBase);
         }
      }
   }
   return c_Retval;
   //lint -e{1763} Its a const iterator!
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the index for the specified item

   \param[in]  orc_Id               ID
   \param[in]  oru32_ValidLayers    Number of valid layers for the ID

   \return
   Model index, might be invalid
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SyvDaItPaTreeModel::GetIndexForItem(const C_OSCNodeDataPoolListElementId & orc_Id,
                                                  const uint32 & oru32_ValidLayers) const
{
   QModelIndex c_Retval;

   if ((oru32_ValidLayers <= 4UL) && (this->mpc_InvisibleRootItem != NULL))
   {
      uint32 u32_ItAll = 0UL;
      for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItAll = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItAll != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItAll)
      {
         //Top level

         const C_TblTreItem * const pc_AllNode = dynamic_cast<const C_TblTreItem * const>(*c_ItAll);
         if (pc_AllNode != NULL)
         {
            const QModelIndex c_AllNode = this->index(static_cast<sintn>(u32_ItAll), 0);
            if (oru32_ValidLayers == 0UL)
            {
               c_Retval = c_AllNode;
            }
            else
            {
               uint32 u32_ItNode = 0UL;
               for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItNode = pc_AllNode->c_Children.begin();
                    c_ItNode != pc_AllNode->c_Children.end(); ++c_ItNode)
               {
                  //Node level

                  const C_TblTreItem * const pc_Node = dynamic_cast<const C_TblTreItem * const>(*c_ItNode);
                  if ((pc_Node != NULL) && (pc_Node->u32_Index == orc_Id.u32_NodeIndex))
                  {
                     const QModelIndex c_Node = this->index(static_cast<sintn>(u32_ItNode), 0, c_AllNode);
                     if (oru32_ValidLayers == 1UL)
                     {
                        c_Retval = c_Node;
                     }
                     else
                     {
                        uint32 u32_ItDp = 0UL;
                        for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItDp = pc_Node->c_Children.begin();
                             c_ItDp != pc_Node->c_Children.end(); ++c_ItDp)
                        {
                           //Datapool level

                           const C_TblTreItem * const pc_Dp = dynamic_cast<const C_TblTreItem * const>(*c_ItDp);
                           if ((pc_Dp != NULL) && (pc_Dp->u32_Index == orc_Id.u32_DataPoolIndex))
                           {
                              const QModelIndex c_Dp = this->index(static_cast<sintn>(u32_ItDp), 0, c_Node);
                              if (oru32_ValidLayers == 2UL)
                              {
                                 c_Retval = c_Dp;
                              }
                              else
                              {
                                 uint32 u32_ItLi = 0UL;
                                 for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItLi =
                                         pc_Dp->c_Children.begin();
                                      c_ItLi != pc_Dp->c_Children.end(); ++c_ItLi)
                                 {
                                    //List level

                                    const C_TblTreItem * const pc_Li =
                                       dynamic_cast<const C_TblTreItem * const>(*c_ItLi);
                                    if ((pc_Li != NULL) && (pc_Li->u32_Index == orc_Id.u32_ListIndex))
                                    {
                                       const QModelIndex c_Li = this->index(static_cast<sintn>(u32_ItLi), 0, c_Dp);
                                       if (oru32_ValidLayers == 3UL)
                                       {
                                          c_Retval = c_Li;
                                       }
                                       else
                                       {
                                          uint32 u32_ItEl = 0UL;
                                          for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItEl =
                                                  pc_Li->c_Children.begin();
                                               c_ItEl != pc_Li->c_Children.end(); ++c_ItEl)
                                          {
                                             //Element level

                                             const C_TblTreItem * const pc_El =
                                                dynamic_cast<const C_TblTreItem * const>(*c_ItEl);
                                             if ((pc_El != NULL) && (pc_El->u32_Index == orc_Id.u32_ElementIndex))
                                             {
                                                const QModelIndex c_El = this->index(static_cast<sintn>(u32_ItEl), 0,
                                                                                     c_Li);
                                                c_Retval = c_El;
                                             }
                                             ++u32_ItEl;
                                          }
                                       }
                                    }
                                    ++u32_ItLi;
                                 }
                              }
                           }
                           ++u32_ItDp;
                        }
                     }
                  }
                  ++u32_ItNode;
               }
            }
         }
         ++u32_ItAll;
      }
   }
   return c_Retval;
   //lint -e{1763} Its a const iterator!
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all list IDs

   \return
   All list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeModel::GetAllListIds(void) const
{
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_Retval;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         c_Retval.reserve(pc_Param->c_DataPoolElementsConfig.size());
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               //Wrapper to keep list unique
               h_AppendOnlyUniqueListId(c_Retval, rc_Config.c_ElementId);
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if all specified lists are read

   \param[in]  orc_ListIds    List IDs to check

   \return
   True  Read
   False Not read
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::CheckListsRead(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds) const
{
   bool q_Retval = true;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if ((pc_Param != NULL) && (pc_Param->c_DataPoolElementsConfig.size() == this->mc_ECUValuesReadStatus.size()))
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               for (uint32 u32_ItInput = 0UL; u32_ItInput < orc_ListIds.size(); ++u32_ItInput)
               {
                  const C_OSCNodeDataPoolListElementId & rc_Input = orc_ListIds[u32_ItInput];
                  if (((rc_Config.c_ElementId.u32_NodeIndex == rc_Input.u32_NodeIndex) &&
                       (rc_Config.c_ElementId.u32_DataPoolIndex == rc_Input.u32_DataPoolIndex)) &&
                      (rc_Config.c_ElementId.u32_ListIndex == rc_Input.u32_ListIndex))
                  {
                     //If any not read invalidate return flag
                     if (this->mc_ECUValuesReadStatus[u32_ItConfig] == false)
                     {
                        q_Retval = false;
                     }
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all changed list IDs

   \return
   All changed list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeModel::GetChangedListElementIds(void)
const
{
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_Retval;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (((this->mc_ECUValuesReadStatus.size() == this->mc_ECUValues.size()) &&
              (this->mc_ECUValues.size() == pc_Param->c_ListValues.size())) &&
             (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_ListValues.size()))
         {
            for (uint32 u32_ItElement = 0UL; u32_ItElement < this->mc_ECUValuesReadStatus.size(); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItElement];

               if (this->mc_ECUValuesReadStatus[u32_ItElement] == true)
               {
                  const C_OSCNodeDataPoolContent * const pc_Value = m_GetSetValue(u32_ItElement);
                  if (pc_Value != NULL)
                  {
                     if (this->mc_ECUValues[u32_ItElement] == *pc_Value)
                     {
                        //No add necessary
                     }
                     else
                     {
                        //Changed
                        c_Retval.push_back(rc_Config.c_ElementId);
                     }
                  }
                  else
                  {
                     //Invalid
                     c_Retval.push_back(rc_Config.c_ElementId);
                  }
               }
               else
               {
                  //Not read
                  c_Retval.push_back(rc_Config.c_ElementId);
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all invalid list IDs

   \return
   All invalid list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCNodeDataPoolListId> C_SyvDaItPaTreeModel::GetInvalidListIds(void) const
{
   std::vector<C_OSCNodeDataPoolListId> c_Retval;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (this->mc_ECUValuesReadStatus.size() == pc_Param->c_DataPoolElementsConfig.size())
         {
            for (uint32 u32_ItElement = 0UL; u32_ItElement < this->mc_ECUValuesReadStatus.size(); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItElement];

               //Only consider read elements
               if ((this->mc_ECUValuesReadStatus[u32_ItElement] == true) &&
                   (rc_Config.c_ElementId.GetIsValid() == true))
               {
                  uint32 u32_ItFinalIndex;
                  const bool q_Found1 = m_GetListIndex(rc_Config.c_ElementId, u32_ItFinalIndex);

                  if (((q_Found1 == true) && (u32_ItFinalIndex < this->mc_ECUCRCValidStatus.size())) &&
                      (this->mc_ECUCRCValidStatus[u32_ItFinalIndex] == false))
                  {
                     //Append unique
                     bool q_Found = false;
                     for (uint32 u32_ItExisting = 0UL; (u32_ItExisting < c_Retval.size()) && (q_Found == false);
                          ++u32_ItExisting)
                     {
                        const C_OSCNodeDataPoolListId & rc_ListId = c_Retval[u32_ItExisting];
                        if (((rc_ListId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                             (rc_ListId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                            (rc_ListId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
                        {
                           q_Found = true;
                        }
                     }
                     if (q_Found == false)
                     {
                        c_Retval.push_back(rc_Config.c_ElementId);
                     }
                  }
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update ECU values for specified ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::UpdateECUValues(void)
{
   C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
      if (pc_ParamData != NULL)
      {
         for (uint32 u32_Index = 0UL; u32_Index < pc_ParamData->c_DataPoolElementsConfig.size(); ++u32_Index)
         {
            C_OSCNodeDataPoolContent c_Content;
            if (pc_ParamWidget->GetLastValue(u32_Index, c_Content) == C_NO_ERR)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_Index];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (((u32_Index < this->mc_ECUValuesReadStatus.size()) && (u32_Index < this->mc_ECUValues.size())) &&
                      (u32_Index < this->mc_ECUValuesString.size()))
                  {
                     auto && rc_Ref = this->mc_ECUValuesReadStatus[u32_Index];
                     const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                                   rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                                   rc_Config.c_ElementId.u32_ListIndex,
                                                                                   rc_Config.c_ElementId.u32_ElementIndex);
                     const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                        C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                                  rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                                  rc_Config.c_ElementId.u32_ListIndex,
                                                                                  rc_Config.c_ElementId.u32_ElementIndex);
                     //Confirm read status
                     rc_Ref = true;
                     //Just check
                     if (pc_OSCElement != NULL)
                     {
                        std::vector<QString> & rc_ValueStrings = this->mc_ECUValuesString[u32_Index];
                        this->mc_ECUValues[u32_Index] = c_Content;
                        //Handle string
                        if ((pc_UiElement->q_InterpretAsString == true) && (rc_ValueStrings.size() > 0UL))
                        {
                           rc_ValueStrings[0] =
                              C_SdNdeDpUtil::h_ConvertToString(c_Content);
                        }
                        else
                        {
                           rc_ValueStrings.clear();
                           C_SdNdeDpContentUtil::h_GetValuesAsScaledString(c_Content,
                                                                           pc_OSCElement->f64_Factor,
                                                                           pc_OSCElement->f64_Offset,
                                                                           rc_ValueStrings, false);
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update CRC status

   \param[in]  orc_ListId  LIST ID
   \param[in]  oq_Status   New CRC status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::SetCRCStatus(const C_OSCNodeDataPoolListId & orc_ListId, const bool oq_Status)
{
   C_OSCNodeDataPoolListElementId c_TmpId;
   uint32 u32_ItFinalIndex;
   bool q_Found;

   //Get list
   c_TmpId.u32_NodeIndex = orc_ListId.u32_NodeIndex;
   c_TmpId.u32_DataPoolIndex = orc_ListId.u32_DataPoolIndex;
   c_TmpId.u32_ListIndex = orc_ListId.u32_ListIndex;
   c_TmpId.u32_ElementIndex = 0UL;
   q_Found = m_GetListIndex(c_TmpId, u32_ItFinalIndex);

   if ((q_Found == true) && (u32_ItFinalIndex < this->mc_ECUCRCValidStatus.size()))
   {
      //Update CRC flag
      auto && rc_Ref = this->mc_ECUCRCValidStatus[u32_ItFinalIndex];
      rc_Ref = oq_Status;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get set values for specified list

   \param[in]   orc_ListId       List ID
   \param[out]  orc_ListValues   Set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::GetListSetValues(const C_OSCNodeDataPoolListElementId & orc_ListId,
                                            std::vector<C_OSCNodeDataPoolContent> & orc_ListValues) const
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if ((rc_Config.c_ElementId.GetIsValid() == true) && (u32_ItConfig < pc_Param->c_ListValues.size()))
            {
               if (((orc_ListId.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                    (orc_ListId.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex)) &&
                   (orc_ListId.u32_ListIndex == rc_Config.c_ElementId.u32_ListIndex))
               {
                  const C_OSCNodeDataPoolContent * const pc_Value = m_GetSetValue(u32_ItConfig);
                  if (pc_Value != NULL)
                  {
                     //Check if the order is as expected
                     tgl_assert(orc_ListValues.size() == rc_Config.c_ElementId.u32_ElementIndex);
                     orc_ListValues.push_back(*pc_Value);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize

   \param[in]  opc_DataWidget    Data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::Init(C_PuiSvDbDataElementHandler * const opc_DataWidget)
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(opc_DataWidget);

   this->beginResetModel();
   //Clear
   delete (this->mpc_InvisibleRootItem);
   this->mpc_InvisibleRootItem = new C_TblTreItem();
   this->mpc_DataWidget = opc_DataWidget;
   this->mc_ECUValuesString.clear();
   this->mc_ECUCRCValidStatus.clear();
   this->mc_ECUValuesReadStatus.clear();
   this->mc_ECUValues.clear();

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         //Create root node
         C_TblTreItem * const pc_AllNode = new C_TblTreItem();
         mh_InitAllNode(pc_AllNode, pc_ParamWidget->GetViewIndex());
         this->mpc_InvisibleRootItem->AddChild(pc_AllNode);
         //init
         this->mc_ECUValues = pc_Param->c_ListValues;
         this->mc_ECUValuesString.reserve(pc_Param->c_ListValues.size());
         this->mc_ECUValuesReadStatus.resize(pc_Param->c_ListValues.size(), false);
         for (uint32 u32_It1 = 0UL; u32_It1 < pc_Param->c_DataPoolElementsConfig.size(); ++u32_It1)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_It1];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                  C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                            rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                            rc_Config.c_ElementId.u32_ListIndex,
                                                                            rc_Config.c_ElementId.u32_ElementIndex);
               C_TblTreItem * pc_Node = pc_AllNode->GetItem(rc_Config.c_ElementId.u32_NodeIndex);
               C_TblTreItem * pc_DataPool;
               C_TblTreItem * pc_List;
               C_TblTreItem * pc_Element;
               if (pc_Node == NULL)
               {
                  //Create node
                  pc_Node = new C_TblTreItem();
                  mh_InitNode(pc_Node, rc_Config.c_ElementId.u32_NodeIndex, pc_ParamWidget->GetViewIndex());
                  pc_AllNode->AddChild(pc_Node);
               }
               pc_DataPool = pc_Node->GetItem(rc_Config.c_ElementId.u32_DataPoolIndex);
               //Continue
               if (pc_DataPool == NULL)
               {
                  //Create data pool
                  pc_DataPool = new C_TblTreItem();
                  mh_InitDataPool(pc_DataPool, rc_Config.c_ElementId.u32_NodeIndex,
                                  rc_Config.c_ElementId.u32_DataPoolIndex, pc_Node->q_Enabled);
                  pc_Node->AddChild(pc_DataPool);
               }
               pc_List = pc_DataPool->GetItem(rc_Config.c_ElementId.u32_ListIndex);
               //Continue
               if (pc_List == NULL)
               {
                  //Create list
                  pc_List = new C_TblTreItem();
                  mh_InitList(pc_List, rc_Config.c_ElementId.u32_NodeIndex,
                              rc_Config.c_ElementId.u32_DataPoolIndex,
                              rc_Config.c_ElementId.u32_ListIndex, pc_Node->q_Enabled);
                  pc_DataPool->AddChild(pc_List);
                  //Necessary for each new list
                  this->mc_ECUCRCValidStatus.push_back(false);
               }
               pc_Element = pc_List->GetItem(rc_Config.c_ElementId.u32_ElementIndex);
               //Continue
               if (pc_Element == NULL)
               {
                  //Create element
                  pc_Element = new C_TblTreItem();
                  mh_InitElement(pc_Element, rc_Config.c_ElementId.u32_NodeIndex,
                                 rc_Config.c_ElementId.u32_DataPoolIndex,
                                 rc_Config.c_ElementId.u32_ListIndex,
                                 rc_Config.c_ElementId.u32_ElementIndex, pc_Node->q_Enabled);
                  pc_List->AddChild(pc_Element);
               }
               //Handle ECU values
               if (pc_UiElement != NULL)
               {
                  std::vector<QString> c_Entry;
                  if (pc_UiElement->q_InterpretAsString == true)
                  {
                     c_Entry.push_back(C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING);
                  }
                  else
                  {
                     if (u32_It1 < pc_Param->c_ListValues.size())
                     {
                        const C_OSCNodeDataPoolContent & rc_Content = pc_Param->c_ListValues[u32_It1];
                        if (rc_Content.GetArray() == false)
                        {
                           c_Entry.push_back(C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING);
                        }
                        else
                        {
                           c_Entry.reserve(rc_Content.GetArraySize());
                           for (uint32 u32_ItArr = 0UL; u32_ItArr < rc_Content.GetArraySize(); ++u32_ItArr)
                           {
                              c_Entry.push_back(C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING);
                           }
                        }
                     }
                  }
                  this->mc_ECUValuesString.push_back(c_Entry);
               }
            }
         }
      }
   }
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaTreeModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                          const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SyvDaItPaTreeModel::E_Columns e_Col = C_SyvDaItPaTreeModel::h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eTREE:
            c_Retval = C_GtGetText::h_GetText("Items");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eDEVICE_VALUE:
            c_Retval = C_GtGetText::h_GetText("Device Value");
            break;
         case eSET:
            c_Retval = C_GtGetText::h_GetText("Set Value");
            break;
         case eUNIT:
            c_Retval = C_GtGetText::h_GetText("Unit");
            break;
         case eACTION_READ:
            c_Retval = C_GtGetText::h_GetText("Read");
            break;
         case eACTION_WRITE:
            c_Retval = C_GtGetText::h_GetText("Write");
            break;
         case eACTION_APPLY:
            c_Retval = C_GtGetText::h_GetText("Copy");
            break;
         case eACTION_LOAD:
            c_Retval = C_GtGetText::h_GetText("Load");
            break;
         case eACTION_SAVE:
            c_Retval = C_GtGetText::h_GetText("Save");
            break;
         case eACTION_RECORD:
            c_Retval = C_GtGetText::h_GetText("Rec.");
            break;
         case eACTION_REMOVE:
            c_Retval = C_GtGetText::h_GetText("Del.");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eTREE:
            c_Retval = C_GtGetText::h_GetText("Items");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eDEVICE_VALUE:
            c_Retval = C_GtGetText::h_GetText("Device Value");
            break;
         case eSET:
            c_Retval = C_GtGetText::h_GetText("Set Value");
            break;
         case eUNIT:
            c_Retval = C_GtGetText::h_GetText("Unit");
            break;
         case eACTION_READ:
            c_Retval = C_GtGetText::h_GetText("Read command");
            break;
         case eACTION_WRITE:
            c_Retval = C_GtGetText::h_GetText("Write command");
            break;
         case eACTION_APPLY:
            c_Retval = C_GtGetText::h_GetText("Copy command");
            break;
         case eACTION_LOAD:
            c_Retval = C_GtGetText::h_GetText("Load command");
            break;
         case eACTION_SAVE:
            c_Retval = C_GtGetText::h_GetText("Save command");
            break;
         case eACTION_RECORD:
            c_Retval = C_GtGetText::h_GetText("Record command");
            break;
         case eACTION_REMOVE:
            c_Retval = C_GtGetText::h_GetText("Delete command");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         const QString c_PhysicalValueInfo = C_GtGetText::h_GetText("The raw value of a data element is the value as it is transmitted in the network."
                                                                    "\nThe physical value of a data element is the value of the physical quantity (e.g. speed, "
                                                                    "\nrpm, temperature, etc.) that represents the data element."
                                                                    "\nThe following conversion formula is used to transform the raw value "
                                                                    "\nto a physical value or in the reverse direction:"
                                                                    "\n\n[Physical value] = ([Raw value] * [Factor]) + [Offset]");
         QString c_InfoText;
         switch (e_Col)
         {
         case eTREE:
            c_Retval = "";
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment for this data element.");
            break;
         case eDEVICE_VALUE:
            c_Retval = C_GtGetText::h_GetText("Device value (= actual value), read from system NVM."
                                              "\n\nExecute \"Read\" command to refresh.");
            break;
         case eSET:
            c_Retval = C_GtGetText::h_GetText("Set value (= desired value). Bold styled if != device value."
                                              "\n\nEdit Modes:"
                                              "\n - Custom values (default): values can be edited manually by user"
                                              "\n - Dataset values (if available): Dataset values defined in SYSTEM DEFINITION are used. Cannot be adapted by user."
                                              "\n\nExecute \"Write\" command to write set values to device in parametrization step");
            break;
         case eUNIT:
            c_InfoText = C_GtGetText::h_GetText("Unit of the signals physical quantity\n\n");
            c_Retval = c_InfoText.append(c_PhysicalValueInfo);
            break;
         case eACTION_READ:
            c_Retval = C_GtGetText::h_GetText("Triggers the read of actual device values from System NVM.");
            break;
         case eACTION_WRITE:
            c_Retval = C_GtGetText::h_GetText(
               "Triggers a parametrization dialog where set values (desired values) are written to System NVM");
            break;
         case eACTION_APPLY:
            c_Retval = C_GtGetText::h_GetText("Copies values from \"device values\" column to \"set values\" column");
            break;
         case eACTION_LOAD:
            c_Retval = C_GtGetText::h_GetText("Loads \"set values\" from a parameter set file.");
            break;
         case eACTION_SAVE:
            c_Retval = C_GtGetText::h_GetText("Saves \"set values\" to a parameter set file.");
            break;
         case eACTION_RECORD:
            c_Retval = C_GtGetText::h_GetText(
               "Triggers a parameter set record dialog where actual device values are written to a parameter set image.");
            break;
         case eACTION_REMOVE:
            c_Retval = C_GtGetText::h_GetText("Remove list from parametrization widget.");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No data in default case
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SyvDaItPaTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 12;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItPaTreeModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, osn_Role);

   //Other columns need manual handling
   if (orc_Index.column() > 0)
   {
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;
         const C_OSCNodeDataPoolListElement * pc_OSCElement;
         const C_PuiSdNodeDataPoolListElement * pc_UiElement;
         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());

         const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
         switch (u32_ValidLayers)
         {
         case 1:
            if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) && (e_Col == C_SyvDaItPaTreeModel::eCOMMENT))
            {
               const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_Id.u32_NodeIndex);
               if (pc_Node != NULL)
               {
                  c_Retval = pc_Node->c_Properties.c_Comment.c_str();
               }
            }
            break;
         case 2:
            if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) && (e_Col == C_SyvDaItPaTreeModel::eCOMMENT))
            {
               const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
                  c_Id.u32_NodeIndex,
                  c_Id.u32_DataPoolIndex);
               if (pc_DataPool != NULL)
               {
                  c_Retval = pc_DataPool->c_Comment.c_str();
               }
            }
            break;
         case 3:
            //List
            switch (e_Col) //lint !e788 not all cases handled here explicitly
            {
            case C_SyvDaItPaTreeModel::eCOMMENT:
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
               {
                  const C_OSCNodeDataPoolList * const pc_List =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(c_Id.u32_NodeIndex,
                                                                         c_Id.u32_DataPoolIndex,
                                                                         c_Id.u32_ListIndex);
                  if (pc_List != NULL)
                  {
                     c_Retval = pc_List->c_Comment.c_str();
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eDEVICE_VALUE:
               if (orc_Index.row() >= 0)
               {
                  //Only show CRC if active
                  const C_OSCNodeDataPoolList * const pc_List =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(c_Id.u32_NodeIndex,
                                                                         c_Id.u32_DataPoolIndex,
                                                                         c_Id.u32_ListIndex);
                  if ((pc_List != NULL) && (pc_List->q_NvMCRCActive == true))
                  {
                     if (pc_ParamWidget != NULL)
                     {
                        const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
                        //Just request index of first element
                        c_Id.u32_ElementIndex = 0UL;
                        if (pc_ParamData != NULL)
                        {
                           const uint32 u32_Index = GetParamIndexId(c_Id);
                           if (u32_Index < this->mc_ECUValuesReadStatus.size())
                           {
                              if (this->mc_ECUValuesReadStatus[u32_Index] == true)
                              {
                                 uint32 u32_ItFinalIndex;
                                 const bool q_Found = m_GetListIndex(c_Id, u32_ItFinalIndex);
                                 if ((q_Found == true) && (u32_ItFinalIndex < this->mc_ECUCRCValidStatus.size()))
                                 {
                                    if (this->mc_ECUCRCValidStatus[u32_ItFinalIndex] == true)
                                    {
                                       c_Retval = C_GtGetText::h_GetText("CRC: Valid");
                                    }
                                    else
                                    {
                                       c_Retval = C_GtGetText::h_GetText("CRC: Invalid");
                                    }
                                 }
                              }
                              else
                              {
                                 c_Retval = static_cast<QString>(C_GtGetText::h_GetText("CRC: %1")).arg(
                                    C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING);
                              }
                           }
                        }
                     }
                  }
                  else
                  {
                     c_Retval = C_GtGetText::h_GetText("CRC: Unused");
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eSET:
               if (pc_ParamWidget != NULL)
               {
                  const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
                  //Just request index of first element
                  c_Id.u32_ElementIndex = 0UL;
                  if (pc_ParamData != NULL)
                  {
                     const uint32 u32_Index = GetParamIndexId(c_Id);
                     if (u32_Index < pc_ParamData->c_DataSetSelectionIndices.size())
                     {
                        const sint32 s32_DataSetIndex = pc_ParamData->c_DataSetSelectionIndices[u32_Index];
                        if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                        {
                           if (s32_DataSetIndex < 0)
                           {
                              c_Retval = C_GtGetText::h_GetText("Custom values");
                           }
                           else
                           {
                              const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                              const C_OSCNodeDataPoolList * const pc_List =
                                 C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(c_Id.u32_NodeIndex,
                                                                                     c_Id.u32_DataPoolIndex,
                                                                                     c_Id.u32_ListIndex);
                              if ((pc_List != NULL) && (u32_DataSet < pc_List->c_DataSets.size()))
                              {
                                 const C_OSCNodeDataPoolDataSet & rc_DataSet = pc_List->c_DataSets[u32_DataSet];
                                 c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Dataset \"%1\"")).arg(
                                    rc_DataSet.c_Name.c_str());
                              }
                           }
                        }
                        else if (osn_Role == static_cast<sintn>(Qt::EditRole))
                        {
                           c_Retval = static_cast<sintn>(s32_DataSetIndex + 1L);
                        }
                        else
                        {
                           //Use default value = empty
                        }
                     }
                  }
               }
               break;
            default:
               //Use default value = empty
               break;
            }
            break;
         case 4:
            //Element
            switch (e_Col) //lint !e788 not all cases handled here explicitly
            {
            case C_SyvDaItPaTreeModel::eCOMMENT:
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
               {
                  pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_Id.u32_NodeIndex,
                                                                                             c_Id.u32_DataPoolIndex,
                                                                                             c_Id.u32_ListIndex,
                                                                                             c_Id.u32_ElementIndex);
                  if (pc_OSCElement != NULL)
                  {
                     c_Retval = pc_OSCElement->c_Comment.c_str();
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eDEVICE_VALUE:
               if (pc_ParamWidget != NULL)
               {
                  const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
                  if (pc_ParamData != NULL)
                  {
                     const uint32 u32_Index = GetParamIndexId(c_Id);
                     if ((u32_Index < this->mc_ECUValuesString.size()) &&
                         (u32_Index < this->mc_ECUValuesReadStatus.size()))
                     {
                        if (this->mc_ECUValuesReadStatus[u32_Index] == false)
                        {
                           c_Retval = C_SyvDaItPaTreeModel::mhc_ECU_VALUE_INIT_STRING;
                        }
                        else
                        {
                           pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                              c_Id.u32_NodeIndex,
                              c_Id.u32_DataPoolIndex,
                              c_Id.u32_ListIndex,
                              c_Id.u32_ElementIndex);
                           pc_UiElement = C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(c_Id.u32_NodeIndex,
                                                                                                    c_Id.u32_DataPoolIndex,
                                                                                                    c_Id.u32_ListIndex,
                                                                                                    c_Id.u32_ElementIndex);
                           if ((pc_OSCElement != NULL) && (pc_UiElement != NULL))
                           {
                              if ((pc_UiElement->q_InterpretAsString == false) && (pc_OSCElement->GetArray() == true))
                              {
                                 if (osn_Role == static_cast<sintn>(Qt::EditRole))
                                 {
                                    //Special link value
                                    c_Retval = QPoint();
                                 }
                                 else
                                 {
                                    //Return display value (no edit required)
                                    c_Retval =
                                       C_SdNdeDpContentUtil::h_CombineString(this->mc_ECUValuesString[u32_Index]);
                                 }
                              }
                              else
                              {
                                 //Return display value (no edit required)
                                 c_Retval =
                                    C_SdNdeDpContentUtil::h_CombineString(this->mc_ECUValuesString[u32_Index]);
                              }
                           }
                        }
                     }
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eSET:
               if (pc_ParamWidget != NULL)
               {
                  const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
                  if (pc_ParamData != NULL)
                  {
                     const uint32 u32_Index = GetParamIndexId(c_Id);
                     const C_OSCNodeDataPoolContent * const pc_Content = m_GetSetValue(u32_Index);
                     if (pc_Content != NULL)
                     {
                        pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                           c_Id.u32_NodeIndex,
                           c_Id.u32_DataPoolIndex,
                           c_Id.u32_ListIndex,
                           c_Id.u32_ElementIndex);
                        pc_UiElement = C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(c_Id.u32_NodeIndex,
                                                                                                 c_Id.u32_DataPoolIndex,
                                                                                                 c_Id.u32_ListIndex,
                                                                                                 c_Id.u32_ElementIndex);
                        if ((pc_OSCElement != NULL) && (pc_UiElement != NULL))
                        {
                           if (pc_UiElement->q_InterpretAsString == true)
                           {
                              c_Retval = C_SdNdeDpUtil::h_ConvertToString(*pc_Content);
                           }
                           else
                           {
                              if (osn_Role == static_cast<sintn>(Qt::EditRole))
                              {
                                 if (pc_Content->GetArray() == true)
                                 {
                                    //Special link value
                                    c_Retval = QPoint();
                                 }
                                 else
                                 {
                                    //Return value for edit
                                    c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(*pc_Content,
                                                                                                     pc_OSCElement->f64_Factor,
                                                                                                     pc_OSCElement->f64_Offset,
                                                                                                     0, false);
                                 }
                              }
                              else
                              {
                                 QString c_Output;
                                 C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(*pc_Content,
                                                                                         pc_OSCElement->f64_Factor,
                                                                                         pc_OSCElement->f64_Offset,
                                                                                         c_Output, false);
                                 c_Retval = c_Output;
                              }
                           }
                        }
                     }
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eUNIT:
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
               {
                  pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_Id.u32_NodeIndex,
                                                                                             c_Id.u32_DataPoolIndex,
                                                                                             c_Id.u32_ListIndex,
                                                                                             c_Id.u32_ElementIndex);
                  if (pc_OSCElement != NULL)
                  {
                     c_Retval = pc_OSCElement->c_Unit.c_str();
                  }
               }
               break;
            default:
               //Use default value = empty
               break;
            }
            break;
         default:
            //Use default value = empty
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;

         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         QStringList c_Icons;
         switch (e_Col) //lint !e788 not all cases handled here explicitly
         {
         case eACTION_READ:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/system_views/IconTableRead.svg");
               c_Icons.append("://images/system_views/IconTableReadDisabled.svg");
            }
            break;
         case eACTION_WRITE:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/system_views/IconTableWrite.svg");
               c_Icons.append("://images/system_views/IconTableWriteDisabled.svg");
            }
            break;
         case eACTION_APPLY:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/system_views/IconTableCopyColumn.svg");
               c_Icons.append("://images/system_views/IconTableCopyColumnDisabled.svg");
            }
            break;
         case eACTION_LOAD:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/IconImportFile.svg");
               c_Icons.append("://images/IconImportFileDisabled.svg");
            }
            break;
         case eACTION_SAVE:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/IconExportFile.svg");
               c_Icons.append("://images/IconExportFileDisabled.svg");
            }
            break;
         case eACTION_RECORD:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/system_views/IconTableRecord.svg");
               c_Icons.append("://images/system_views/IconTableRecordDisabled.svg");
            }
            break;
         case eACTION_REMOVE:
            if (u32_ValidLayers < 4)
            {
               c_Icons.push_back(QString::number(16));
               c_Icons.append("://images/main_page_and_navi_bar/Icon_delete.svg");
               c_Icons.append("://images/main_page_and_navi_bar/Icon_delete_disabled.svg");
            }
            break;
         default:
            //Use default value = empty
            break;
         }
         c_Retval = c_Icons;
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font;

         const C_GiSvDaParam * const pc_ParamWidget =
            dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         //Default
         c_Font = mc_STYLE_GUIDE_FONT_REGULAR_11;
         if ((data(orc_Index,
                   static_cast<sintn>(Qt::EditRole)).type() == QVariant::Point) &&
             (flags(orc_Index).testFlag(Qt::ItemIsEditable) == true))
         {
            //Special link handling
            c_Font.setUnderline(true);
         }
         if (pc_ParamWidget != NULL)
         {
            const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
            C_OSCNodeDataPoolListElementId c_Id;
            uint32 u32_ValidLayers;
            switch (e_Col) //lint !e788 not all cases handled here explicitly
            {
            case C_SyvDaItPaTreeModel::eSET:
               C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
               if ((pc_ParamData != NULL) && (u32_ValidLayers == 4))
               {
                  const uint32 u32_Index = GetParamIndexId(c_Id);
                  const C_OSCNodeDataPoolContent * const pc_Value = m_GetSetValue(u32_Index);
                  const C_OSCNodeDataPoolListElement * const pc_Element =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_Id);
                  if ((pc_Element != NULL) && (pc_Value != NULL))
                  {
                     //Check error
                     if ((*pc_Value >= pc_Element->c_MinValue) && (*pc_Value >= pc_Element->c_MinValue))
                     {
                        //No error
                        if (((u32_Index < this->mc_ECUValuesReadStatus.size()) &&
                             (u32_Index < this->mc_ECUValues.size())) &&
                            (this->mc_ECUValuesReadStatus[u32_Index] == true))
                        {
                           if (*pc_Value == this->mc_ECUValues[u32_Index])
                           {
                              //Use default font
                           }
                           else
                           {
                              //Mark as change
                              c_Font.setBold(true);
                           }
                        }
                     }
                     else
                     {
                        //Error
                        c_Font.setBold(true);
                     }
                  }
               }
               break;
            case C_SyvDaItPaTreeModel::eDEVICE_VALUE:
               C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
               if ((pc_ParamData != NULL) && (u32_ValidLayers == 3))
               {
                  const uint32 u32_ItemIndex = GetParamIndexId(c_Id);
                  uint32 u32_ItFinalIndex;
                  const bool q_Found = m_GetListIndex(c_Id, u32_ItFinalIndex);
                  //Check if CRC read and still invalid
                  if (((((q_Found == true) && (u32_ItFinalIndex < this->mc_ECUCRCValidStatus.size())) &&
                        (u32_ItemIndex < this->mc_ECUValuesReadStatus.size())) &&
                       (this->mc_ECUValuesReadStatus[u32_ItemIndex] == true)) &&
                      (this->mc_ECUCRCValidStatus[u32_ItFinalIndex] == false))
                  {
                     c_Font.setBold(true);
                  }
               }
               break;
            default:
               //Use default value = empty
               break;
            }
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(msn_USER_ROLE_TOOL_TIP_HEADING))
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;
         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());

         //Handle action columns
         if (u32_ValidLayers < 4UL)
         {
            switch (e_Col) //lint !e788 not all cases handled here explicitly
            {
            case C_SyvDaItPaTreeModel::eACTION_APPLY:
               c_Retval = C_GtGetText::h_GetText("Copy command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_READ:
               c_Retval = C_GtGetText::h_GetText("Read command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_WRITE:
               c_Retval = C_GtGetText::h_GetText("Write command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_LOAD:
               c_Retval = C_GtGetText::h_GetText("Load command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_SAVE:
               c_Retval = C_GtGetText::h_GetText("Save command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_RECORD:
               c_Retval = C_GtGetText::h_GetText("Record command");
               break;
            case C_SyvDaItPaTreeModel::eACTION_REMOVE:
               c_Retval = C_GtGetText::h_GetText("Remove command");
               break;
            default:
               //Use default value = empty
               break;
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(msn_USER_ROLE_TOOL_TIP_CONTENT))
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;
         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
         if (e_Col == C_SyvDaItPaTreeModel::eCOMMENT)
         {
            c_Retval = this->data(orc_Index, static_cast<sintn>(Qt::DisplayRole));
         }
         else if (((e_Col == C_SyvDaItPaTreeModel::eDEVICE_VALUE) || (e_Col == C_SyvDaItPaTreeModel::eSET)) &&
                  (u32_ValidLayers == 4UL))
         {
            const C_GiSvDaParam * const pc_ParamWidget =
               dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
               if (pc_ParamData != NULL)
               {
                  const uint32 u32_Index = GetParamIndexId(c_Id);
                  if (((u32_Index < this->mc_ECUValuesReadStatus.size()) &&
                       (this->mc_ECUValuesReadStatus[u32_Index] == true)) || (e_Col == C_SyvDaItPaTreeModel::eSET))
                  {
                     const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                           c_Id.u32_NodeIndex,
                           c_Id.u32_DataPoolIndex,
                           c_Id.u32_ListIndex,
                           c_Id.u32_ElementIndex);
                     const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                        C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(c_Id.u32_NodeIndex,
                                                                                  c_Id.u32_DataPoolIndex,
                                                                                  c_Id.u32_ListIndex,
                                                                                  c_Id.u32_ElementIndex);
                     if ((((pc_OSCElement != NULL) && (pc_UiElement != NULL)) &&
                          (pc_UiElement->q_InterpretAsString == false)) && (pc_OSCElement->GetArray() == true))
                     {
                        c_Retval = this->data(orc_Index, static_cast<sintn>(Qt::DisplayRole));
                     }
                  }
               }
            }
         }
         else
         {
            //Handle action columns
            if (u32_ValidLayers < 4UL)
            {
               const QString c_Template = C_SyvDaItPaTreeModel::h_GetSelectedItemTypeTemplate(orc_Index);
               switch (e_Col) //lint !e788 not all cases handled here explicitly
               {
               case C_SyvDaItPaTreeModel::eACTION_APPLY:
               case C_SyvDaItPaTreeModel::eACTION_READ:
               case C_SyvDaItPaTreeModel::eACTION_WRITE:
               case C_SyvDaItPaTreeModel::eACTION_LOAD:
               case C_SyvDaItPaTreeModel::eACTION_SAVE:
               case C_SyvDaItPaTreeModel::eACTION_RECORD:
               case C_SyvDaItPaTreeModel::eACTION_REMOVE:
                  c_Retval = c_Template.arg(C_GtGetText::h_GetText("Apply for"));
                  break;
               default:
                  //Use default value = empty
                  break;
               }
            }
         }
      }
      else
      {
         //Use default value = empty
      }
   }
   else
   {
      //Handle changed marker
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         if (this->m_IsChanged(orc_Index) == true)
         {
            QString c_Tmp = c_Retval.toString();
            c_Tmp += "*";
            c_Retval = c_Tmp;
         }
      }
   }
   //Handle colors for all columns
   if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
   {
      QColor c_DefaultNonInteractive;
      QColor c_Default;
      const QColor c_ERROR = mc_STYLE_GUIDE_COLOR_24;
      const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if (this->mq_DarkMode == true)
      {
         c_DefaultNonInteractive = mc_STYLE_GUIDE_COLOR_2;
         c_Default = mc_STYLE_GUIDE_COLOR_1;
      }
      else
      {
         c_DefaultNonInteractive = mc_STYLE_GUIDE_COLOR_34;
         c_Default = mc_STYLE_GUIDE_COLOR_31;
      }
      c_Retval = c_DefaultNonInteractive;
      if (e_Col == C_SyvDaItPaTreeModel::eSET)
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;
         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         if (u32_ValidLayers < 4UL)
         {
            if (flags(orc_Index).testFlag(Qt::ItemIsEditable) == true)
            {
               c_Retval = c_Default;
            }
         }
         else
         {
            //Check data set

            const C_GiSvDaParam * const pc_ParamWidget =
               dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
               if (pc_ParamData != NULL)
               {
                  const uint32 u32_Index = GetParamIndexId(c_Id);
                  if ((u32_Index < pc_ParamData->c_DataSetSelectionIndices.size()) &&
                      (pc_ParamData->c_DataSetSelectionIndices[u32_Index] < 0L))
                  {
                     const C_OSCNodeDataPoolListElement * const pc_Element =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_Id);
                     //Check error
                     if ((pc_Element != NULL) && (u32_Index < pc_ParamData->c_ListValues.size()))
                     {
                        const C_OSCNodeDataPoolContent & rc_Value = pc_ParamData->c_ListValues[u32_Index];
                        if ((rc_Value >= pc_Element->c_MinValue) && (rc_Value <= pc_Element->c_MaxValue))
                        {
                           //No error
                           c_Retval = c_Default;
                        }
                        else
                        {
                           //Error
                           c_Retval = c_ERROR;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const sintn osn_Role)
{
   bool q_Retval = false;

   if (osn_Role == static_cast<sintn>(Qt::EditRole))
   {
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());

      const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
      switch (e_Col) //lint !e788 not all cases handled here explicitly
      {
      case C_SyvDaItPaTreeModel::eSET:
         switch (u32_ValidLayers)
         {
         case 3:
            //data set index
            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
               if (pc_ParamData != NULL)
               {
                  bool q_Ok;
                  const sint32 s32_NewDataSetValue = static_cast<sint32>(orc_Value.toInt(&q_Ok)) - 1L;
                  if (q_Ok == true)
                  {
                     C_PuiSvDbParam c_Copy = *pc_ParamData;
                     //Step 2: for each element of this list apply data set value
                     for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size();
                          ++u32_ItConfig)
                     {
                        const C_PuiSvDbNodeDataElementConfig & rc_Config =
                           pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
                        if ((((rc_Config.c_ElementId.GetIsValid()) &&
                              (rc_Config.c_ElementId.u32_NodeIndex == c_Id.u32_NodeIndex)) &&
                             (rc_Config.c_ElementId.u32_DataPoolIndex == c_Id.u32_DataPoolIndex)) &&
                            (rc_Config.c_ElementId.u32_ListIndex == c_Id.u32_ListIndex))
                        {
                           if (u32_ItConfig < pc_ParamData->c_DataSetSelectionIndices.size())
                           {
                              c_Copy.c_DataSetSelectionIndices[u32_ItConfig] = s32_NewDataSetValue;
                           }
                        }
                     }
                     if  (pc_ParamWidget->SetParamItem(c_Copy) == C_NO_ERR)
                     {
                        q_Retval = true;
                     }
                  }
               }
            }
            break;
         case 4:
            //set
            if (pc_ParamWidget != NULL)
            {
               const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
               if (pc_ParamData != NULL)
               {
                  const uint32 u32_Index = GetParamIndexId(c_Id);
                  if (u32_Index < pc_ParamData->c_DataPoolElementsConfig.size())
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        pc_ParamData->c_DataPoolElementsConfig[u32_Index];
                     C_PuiSvDbParam c_Copy = *pc_ParamData;
                     if (u32_Index < c_Copy.c_ListValues.size())
                     {
                        C_OSCNodeDataPoolContent & rc_Content = c_Copy.c_ListValues[u32_Index];
                        if (rc_Content.GetArray() == false)
                        {
                           const C_OSCNodeDataPoolListElement * const pc_Element =
                              C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                                 rc_Config.c_ElementId.u32_NodeIndex, rc_Config.c_ElementId.u32_DataPoolIndex,
                                 rc_Config.c_ElementId.u32_ListIndex, rc_Config.c_ElementId.u32_ElementIndex);
                           if (pc_Element != NULL)
                           {
                              C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(orc_Value, rc_Content,
                                                                                            pc_Element->f64_Factor,
                                                                                            pc_Element->f64_Offset,
                                                                                            0);
                           }
                        }
                        else
                        {
                           //Array edit should handle this use-case except string
                           C_SdNdeDpUtil::h_SetString(orc_Value.toString(), rc_Content);
                        }
                        tgl_assert(pc_ParamWidget->SetParamItem(c_Copy) == C_NO_ERR);
                     }
                  }
               }
            }
            break;
         default:
            //Use default value = empty
            break;
         }
         break;
      default:
         //Use default value = empty
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item flags

   \param[in]  orc_Index   Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SyvDaItPaTreeModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = C_TblTreModel::flags(orc_Index);
   C_OSCNodeDataPoolListElementId c_Id;
   uint32 u32_ValidLayers;
   C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
   const E_Columns e_Col = h_ColumnToEnum(orc_Index.column());

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
   switch (u32_ValidLayers)
   {
   case 0:
      if ((((e_Col == eACTION_APPLY) || (e_Col == eACTION_READ)) || (e_Col == eACTION_RECORD)) ||
          (e_Col == eACTION_WRITE))
      {
         if ((this->mq_Connected == true) && (this->mq_ActionActive == false))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if ((e_Col == eACTION_LOAD) || (e_Col == eACTION_SAVE))
      {
         if (this->mq_SaveLoadActive == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if (e_Col == eACTION_REMOVE)
      {
         if ((this->mq_EditMode == true) && (this->IsEmpty() == false))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else
      {
         //Use default value = empty
      }
      break;
   case 1:
   case 2:
      if ((((e_Col == eACTION_APPLY) || (e_Col == eACTION_READ)) || (e_Col == eACTION_RECORD)) ||
          (e_Col == eACTION_WRITE))
      {
         if ((this->mq_Connected == true) && (this->mq_ActionActive == false))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if ((e_Col == eACTION_LOAD) || (e_Col == eACTION_SAVE))
      {
         if (this->mq_SaveLoadActive == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if (e_Col == eACTION_REMOVE)
      {
         if (this->mq_EditMode == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else
      {
         //Use default value = empty
      }
      break;
   case 3:
      if ((((e_Col == eACTION_APPLY) || (e_Col == eACTION_READ)) || (e_Col == eACTION_RECORD)) ||
          (e_Col == eACTION_WRITE))
      {
         if ((this->mq_Connected == true) && (this->mq_ActionActive == false))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if ((e_Col == eACTION_LOAD) || (e_Col == eACTION_SAVE))
      {
         if (this->mq_SaveLoadActive == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      else if (e_Col == eACTION_REMOVE)
      {
         if (this->mq_EditMode == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable;
         }
         else
         {
            c_Retval = Qt::NoItemFlags;
         }
      }
      //data set index
      else if (e_Col == C_SyvDaItPaTreeModel::eSET)
      {
         //Only editable if there are more datasets
         if (pc_ParamWidget != NULL)
         {
            const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
            if (pc_ParamData != NULL)
            {
               const uint32 u32_Index = GetParamIndexId(c_Id);
               if (u32_Index < pc_ParamData->c_DataPoolElementsConfig.size())
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_Index];
                  if (rc_Config.c_ElementId.GetIsValid() == true)
                  {
                     const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                           rc_Config.c_ElementId.u32_NodeIndex,
                           rc_Config.c_ElementId.u32_DataPoolIndex,
                           rc_Config.c_ElementId.u32_ListIndex,
                           rc_Config.c_ElementId.u32_ElementIndex);
                     if ((pc_OSCElement != NULL) && (pc_OSCElement->c_DataSetValues.size() > 0UL))
                     {
                        c_Retval = c_Retval | Qt::ItemIsEditable;
                     }
                  }
               }
            }
         }
      }
      else
      {
         //Use default value = empty
      }
      break;
   case 4:
      //set
      if ((e_Col == C_SyvDaItPaTreeModel::eSET) || (e_Col == C_SyvDaItPaTreeModel::eDEVICE_VALUE))
      {
         //Only editable if not a datasets or dataset with link
         if (pc_ParamWidget != NULL)
         {
            const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
            if (pc_ParamData != NULL)
            {
               const uint32 u32_Index = GetParamIndexId(c_Id);
               if (((u32_Index < pc_ParamData->c_DataSetSelectionIndices.size()) &&
                    (u32_Index < pc_ParamData->c_DataPoolElementsConfig.size())) &&
                   (u32_Index < this->mc_ECUValuesReadStatus.size()))
               {
                  if ((e_Col == C_SyvDaItPaTreeModel::eSET) || (this->mc_ECUValuesReadStatus[u32_Index] == true))
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        pc_ParamData->c_DataPoolElementsConfig[u32_Index];
                     if (rc_Config.c_ElementId.GetIsValid() == true)
                     {
                        if (pc_ParamData->c_DataSetSelectionIndices[u32_Index] >= 0)
                        {
                           const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                              C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                                 rc_Config.c_ElementId.u32_NodeIndex,
                                 rc_Config.c_ElementId.u32_DataPoolIndex,
                                 rc_Config.c_ElementId.u32_ListIndex,
                                 rc_Config.c_ElementId.u32_ElementIndex);
                           const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                              C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(
                                 rc_Config.c_ElementId.u32_NodeIndex,
                                 rc_Config.c_ElementId.u32_DataPoolIndex,
                                 rc_Config.c_ElementId.u32_ListIndex,
                                 rc_Config.c_ElementId.u32_ElementIndex);
                           if ((pc_OSCElement != NULL) && (pc_UiElement != NULL))
                           {
                              if ((pc_UiElement->q_InterpretAsString == false) && (pc_OSCElement->GetArray() == true))
                              {
                                 c_Retval = c_Retval | Qt::ItemIsEditable;
                              }
                           }
                        }
                        else
                        {
                           if (e_Col == C_SyvDaItPaTreeModel::eSET)
                           {
                              c_Retval = c_Retval | Qt::ItemIsEditable;
                           }
                           else
                           {
                              const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                                 C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                                    rc_Config.c_ElementId.u32_NodeIndex,
                                    rc_Config.c_ElementId.u32_DataPoolIndex,
                                    rc_Config.c_ElementId.u32_ListIndex,
                                    rc_Config.c_ElementId.u32_ElementIndex);
                              const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
                                 C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(
                                    rc_Config.c_ElementId.u32_NodeIndex,
                                    rc_Config.c_ElementId.u32_DataPoolIndex,
                                    rc_Config.c_ElementId.u32_ListIndex,
                                    rc_Config.c_ElementId.u32_ElementIndex);
                              if ((pc_OSCElement != NULL) && (pc_UiElement != NULL))
                              {
                                 if ((pc_UiElement->q_InterpretAsString == false) &&
                                     (pc_OSCElement->GetArray() == true))
                                 {
                                    c_Retval = c_Retval | Qt::ItemIsEditable;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      break;
   default:
      //Use default value = empty
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaTreeModel::E_Columns C_SyvDaItPaTreeModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SyvDaItPaTreeModel::E_Columns e_Retval;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eTREE;
      break;
   case 1:
      e_Retval = eCOMMENT;
      break;
   case 2:
      e_Retval = eDEVICE_VALUE;
      break;
   case 3:
      e_Retval = eSET;
      break;
   case 4:
      e_Retval = eUNIT;
      break;
   case 5:
      e_Retval = eACTION_READ;
      break;
   case 6:
      e_Retval = eACTION_WRITE;
      break;
   case 7:
      e_Retval = eACTION_APPLY;
      break;
   case 8:
      e_Retval = eACTION_LOAD;
      break;
   case 9:
      e_Retval = eACTION_SAVE;
      break;
   case 10:
      e_Retval = eACTION_RECORD;
      break;
   case 11:
      e_Retval = eACTION_REMOVE;
      break;
   default:
      e_Retval = eTREE;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in]  ore_Value   Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItPaTreeModel::h_EnumToColumn(const C_SyvDaItPaTreeModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eTREE:
      s32_Retval = 0;
      break;
   case eCOMMENT:
      s32_Retval = 1;
      break;
   case eDEVICE_VALUE:
      s32_Retval = 2;
      break;
   case eSET:
      s32_Retval = 3;
      break;
   case eUNIT:
      s32_Retval = 4;
      break;
   case eACTION_READ:
      s32_Retval = 5;
      break;
   case eACTION_WRITE:
      s32_Retval = 6;
      break;
   case eACTION_APPLY:
      s32_Retval = 7;
      break;
   case eACTION_LOAD:
      s32_Retval = 8;
      break;
   case eACTION_SAVE:
      s32_Retval = 9;
      break;
   case eACTION_RECORD:
      s32_Retval = 10;
      break;
   case eACTION_REMOVE:
      s32_Retval = 11;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decode model index into data structure index

   \param[in]   orc_Index           Index to decode
   \param[out]  orc_Id              ID (Warning: only valid as defined by valid layers)
   \param[out]  oru32_ValidLayers   Number of valid layers:
                                    0: invalid
                                    1: node
                                    2: data pool
                                    3: list
                                    4: element
                                    Else: undefined
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::h_DecodeIndex(const QModelIndex & orc_Index, C_OSCNodeDataPoolListElementId & orc_Id,
                                         uint32 & oru32_ValidLayers)
{
   oru32_ValidLayers = 0UL;
   if (orc_Index.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem =
         static_cast<const C_TblTreItem *>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         const C_TblTreItem * const pc_FirstParent = dynamic_cast<const C_TblTreItem * const>(pc_TreeItem->pc_Parent);
         if ((pc_FirstParent != NULL) && (pc_FirstParent->pc_Parent != NULL))
         {
            const C_TblTreItem * const pc_SecondParent =
               dynamic_cast<const C_TblTreItem * const>(pc_FirstParent->pc_Parent);
            if ((pc_SecondParent != NULL) && (pc_SecondParent->pc_Parent != NULL))
            {
               const C_TblTreItem * const pc_ThirdParent =
                  dynamic_cast<const C_TblTreItem * const>(pc_SecondParent->pc_Parent);
               if ((pc_ThirdParent != NULL) && (pc_ThirdParent->pc_Parent != NULL))
               {
                  const C_TblTreItem * const pc_FourthParent =
                     dynamic_cast<const C_TblTreItem * const>(pc_ThirdParent->pc_Parent);
                  if ((pc_FourthParent != NULL) && (pc_FourthParent->pc_Parent != NULL))
                  {
                     const C_TblTreItem * const pc_FifthParent =
                        dynamic_cast<const C_TblTreItem * const>(pc_FourthParent->pc_Parent);
                     if ((pc_FifthParent != NULL) && (pc_FifthParent->pc_Parent != NULL))
                     {
                        //Should not happen
                        tgl_assert(false);
                     }
                     else
                     {
                        //Data element
                        //5: Invisible item
                        //4: All root item
                        //3: Node item
                        //2: Data pool item
                        //1: List item
                        orc_Id.u32_NodeIndex = pc_ThirdParent->u32_Index;
                        orc_Id.u32_DataPoolIndex = pc_SecondParent->u32_Index;
                        orc_Id.u32_ListIndex = pc_FirstParent->u32_Index;
                        orc_Id.u32_ElementIndex = pc_TreeItem->u32_Index;
                        oru32_ValidLayers = 4UL;
                     }
                  }
                  else
                  {
                     //List
                     //4: Invisible item
                     //3: All root item
                     //2: Node item
                     //1: Data pool item
                     orc_Id.u32_NodeIndex = pc_SecondParent->u32_Index;
                     orc_Id.u32_DataPoolIndex = pc_FirstParent->u32_Index;
                     orc_Id.u32_ListIndex = pc_TreeItem->u32_Index;
                     oru32_ValidLayers = 3UL;
                  }
               }
               else
               {
                  //Data pool
                  //3: Invisible item
                  //2: All root item
                  //1: Node item
                  orc_Id.u32_NodeIndex = pc_FirstParent->u32_Index;
                  orc_Id.u32_DataPoolIndex = pc_TreeItem->u32_Index;
                  oru32_ValidLayers = 2UL;
               }
            }
            else
            {
               //Node
               //1: Invisible item
               //0: All root item
               orc_Id.u32_NodeIndex = pc_TreeItem->u32_Index;
               oru32_ValidLayers = 1UL;
            }
         }
         else
         {
            //All
            //0: Invisible item
            oru32_ValidLayers = 0UL;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get element config index

   \param[in]  orc_Id   ID to search for

   \return
   Element config index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDaItPaTreeModel::GetParamIndexId(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   uint32 u32_Retval = 0UL;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               if (orc_Id == rc_Config.c_ElementId)
               {
                  u32_Retval = u32_ItConfig;
               }
            }
         }
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to handle unique list ID vector operations

   \param[in,out]  orc_Vec       Existing list IDs and vector to adapt if necessary
   \param[in]      orc_ListId    New list ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::h_AppendOnlyUniqueListId(std::vector<C_OSCNodeDataPoolListElementId> & orc_Vec,
                                                    const C_OSCNodeDataPoolListElementId & orc_ListId)
{
   bool q_Found = false;

   for (uint32 u32_ItExisting = 0UL; u32_ItExisting < orc_Vec.size(); ++u32_ItExisting)
   {
      const C_OSCNodeDataPoolListElementId & rc_Entry = orc_Vec[u32_ItExisting];
      if (((rc_Entry.u32_NodeIndex == orc_ListId.u32_NodeIndex) &&
           (rc_Entry.u32_DataPoolIndex == orc_ListId.u32_DataPoolIndex)) &&
          (rc_Entry.u32_ListIndex == orc_ListId.u32_ListIndex))
      {
         q_Found = true;
      }
   }

   if (q_Found == false)
   {
      orc_Vec.push_back(orc_ListId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get template with the type description for the current selection

   \param[in]  orc_Index   Model index

   \return
   Template with the type description for the current selection
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaTreeModel::h_GetSelectedItemTypeTemplate(const QModelIndex & orc_Index)
{
   QString c_Retval;

   C_OSCNodeDataPoolListElementId c_Id;
   uint32 u32_ValidLayers;

   C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
   switch (u32_ValidLayers)
   {
   case 0UL:
      c_Retval = C_GtGetText::h_GetText("%1 all containing lists");
      break;
   case 1UL:
      c_Retval = C_GtGetText::h_GetText("%1 all containing lists of this node");
      break;
   case 2UL:
      c_Retval = C_GtGetText::h_GetText("%1 all containing lists of this Datapool");
      break;
   case 3UL:
      c_Retval = C_GtGetText::h_GetText("%1 this list");
      break;
   case 4UL:
      c_Retval = C_GtGetText::h_GetText("%1 current list"); //not used
      break;
   default:
      //Unknown
      break;
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all list IDs for the specified item

   \param[in]  orc_Id            ID to look for
   \param[in]  ou32_ValidLayers  Number of valid layers in ID

   \return
   All list IDs for the specified item
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeModel::GetListIdsForId(
   const C_OSCNodeDataPoolListElementId & orc_Id, const uint32 ou32_ValidLayers) const
{
   C_OSCNodeDataPoolListElementId c_IdCopy = orc_Id;

   std::vector<C_OSCNodeDataPoolListElementId> c_Retval;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);
   switch (ou32_ValidLayers)
   {
   case 0UL:
      //All
      if (pc_ParamWidget != NULL)
      {
         const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
         if (pc_Param != NULL)
         {
            for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  h_AppendOnlyUniqueListId(c_Retval, rc_Config.c_ElementId);
               }
            }
         }
      }
      break;
   case 1UL:
      //Node
      if (pc_ParamWidget != NULL)
      {
         const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
         if (pc_Param != NULL)
         {
            for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (c_IdCopy.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex)
                  {
                     h_AppendOnlyUniqueListId(c_Retval, rc_Config.c_ElementId);
                  }
               }
            }
         }
      }
      break;
   case 2UL:
      //Data pool
      if (pc_ParamWidget != NULL)
      {
         const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
         if (pc_Param != NULL)
         {
            for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if ((c_IdCopy.u32_NodeIndex == rc_Config.c_ElementId.u32_NodeIndex) &&
                      (c_IdCopy.u32_DataPoolIndex == rc_Config.c_ElementId.u32_DataPoolIndex))
                  {
                     h_AppendOnlyUniqueListId(c_Retval, rc_Config.c_ElementId);
                  }
               }
            }
         }
      }
      break;
   case 3UL:
      //List
      c_IdCopy.u32_ElementIndex = 0UL;
      c_Retval.push_back(c_IdCopy);
      break;
   case 4UL:
      //Element
      c_Retval.push_back(c_IdCopy);
      break;
   default:
      //Unknown
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all element IDs for the specified item

   Only valid for lists and above layers

   \param[in]  orc_Id            ID to look for
   \param[in]  ou32_ValidLayers  Number of valid layers in ID

   \return
   All element IDs for the specified item
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OSCNodeDataPoolListElementId> C_SyvDaItPaTreeModel::GetElementIdsForId(
   const C_OSCNodeDataPoolListElementId & orc_Id, const uint32 ou32_ValidLayers)
const
{
   std::vector<C_OSCNodeDataPoolListElementId> c_Retval;
   const std::vector<C_OSCNodeDataPoolListElementId> c_Tmp = GetListIdsForId(orc_Id, ou32_ValidLayers);
   for (uint32 u32_ItList = 0UL; u32_ItList < c_Tmp.size(); ++u32_ItList)
   {
      const C_OSCNodeDataPoolListElementId & rc_ListId = c_Tmp[u32_ItList];
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         rc_ListId.u32_NodeIndex, rc_ListId.u32_DataPoolIndex, rc_ListId.u32_ListIndex);
      if (pc_List != NULL)
      {
         C_OSCNodeDataPoolListElementId c_Copy = rc_ListId;
         for (uint32 u32_ItElem = 0UL; u32_ItElem < pc_List->c_Elements.size(); ++u32_ItElem)
         {
            bool q_Contains = false;
            c_Copy.u32_ElementIndex = u32_ItElem;
            for (uint32 u32_ItCurRet = 0UL; u32_ItCurRet < c_Retval.size(); ++u32_ItCurRet)
            {
               if (c_Retval[u32_ItCurRet] == c_Copy)
               {
                  q_Contains = true;
                  break;
               }
            }
            if (q_Contains == false)
            {
               c_Retval.push_back(c_Copy);
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt range of value to next valid value and append results

   \param[in,out]  orc_Content            Content to change
   \param[in]      orc_DescriptionId      Description id
   \param[in,out]  orc_InvalidValueIds    Invalid value ids
   \param[in,out]  orc_InvalidValues      Invalid values
   \param[in,out]  orc_NewValues          New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::h_AdaptFloatRangeOfValueAndAppendResults(C_OSCNodeDataPoolContent & orc_Content,
                                                                    const C_OSCNodeDataPoolListElementId & orc_DescriptionId, std::vector<C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds, std::vector<QString> & orc_InvalidValues,
                                                                    std::vector<QString> & orc_NewValues)
{
   const C_OSCNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DescriptionId);

   if (pc_Element != NULL)
   {
      bool q_WasChanged = false;
      std::vector<float64> c_Values;
      C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Content, c_Values);
      for (uint32 u32_ItVal = 0UL; u32_ItVal < c_Values.size(); ++u32_ItVal)
      {
         const float64 f64_Val = C_OSCUtils::h_GetValueScaled(c_Values[u32_ItVal],
                                                              pc_Element->f64_Factor,
                                                              pc_Element->f64_Offset, false);
         if (std::isnan(f64_Val) || std::isinf(f64_Val))
         {
            q_WasChanged = true;
         }
      }
      if (q_WasChanged)
      {
         QString c_Tmp;
         orc_InvalidValueIds.push_back(orc_DescriptionId);
         C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(orc_Content,
                                                                 pc_Element->f64_Factor,
                                                                 pc_Element->f64_Offset, c_Tmp,
                                                                 false);
         orc_InvalidValues.push_back(c_Tmp);
         C_SyvDaItPaTreeModel::h_AdaptFloatRangeOfValue(orc_Content,
                                                        *pc_Element);
         C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(orc_Content,
                                                                 pc_Element->f64_Factor,
                                                                 pc_Element->f64_Offset, c_Tmp,
                                                                 false);
         orc_NewValues.push_back(c_Tmp);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt range of value to next valid value

   \param[in,out]  orc_Content      Content to change
   \param[in]      orc_Description  Description to use as reference
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::h_AdaptFloatRangeOfValue(C_OSCNodeDataPoolContent & orc_Content,
                                                    const C_OSCNodeDataPoolListElement & orc_Description)
{
   std::vector<float64> c_Values;
   C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_Content, c_Values);
   for (uint32 u32_ItVal = 0UL; u32_ItVal < c_Values.size(); ++u32_ItVal)
   {
      float64 f64_Val = C_OSCUtils::h_GetValueScaled(c_Values[u32_ItVal],
                                                     orc_Description.f64_Factor,
                                                     orc_Description.f64_Offset, false);
      if (std::isnan(f64_Val) || std::isinf(f64_Val))
      {
         C_SyvDaItPaTreeModel::h_FixInvalidFloatValue(f64_Val);
         f64_Val = C_OSCUtils::h_GetValueUnscaled(f64_Val, orc_Description.f64_Factor,
                                                  orc_Description.f64_Offset);
         C_OSCNodeDataPoolContentUtil::h_SetValueInContent(f64_Val, orc_Content, u32_ItVal);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt range of value to next valid value

   \param[out]  orf64_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::h_FixInvalidFloatValue(float64 & orf64_Value)
{
   if (std::isnan(orf64_Value))
   {
      orf64_Value = 0.0;
   }
   else
   {
      C_OSCUtils::h_RangeCheckFloat(orf64_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all node tree node

   \param[in,out]  opc_TreeNode     Tree node to initialize
   \param[in]      ou32_ViewIndex   View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::mh_InitAllNode(C_TblTreItem * const opc_TreeNode, const uint32 ou32_ViewIndex)
{
   if (opc_TreeNode != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
      //Name
      if (pc_View != NULL)
      {
         opc_TreeNode->c_Name =
            static_cast<QString>(C_GtGetText::h_GetText("VIEW #%1 - %2")).arg(ou32_ViewIndex + 1).arg(
               pc_View->GetName());
      }
      //Icon
      opc_TreeNode->c_Icon = QIcon(C_SyvDaItPaTreeModel::mhc_ICON_ALL_NODE);
      //State
      opc_TreeNode->q_Enabled = true;
      opc_TreeNode->q_Selectable = false;
   }
} //lint !e429  //no memory leak because we never took ownership

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize node tree node

   \param[in,out]  opc_TreeNode     Tree node to initialize
   \param[in]      ou32_NodeIndex   Node index (identifier)
   \param[in]      ou32_ViewIndex   View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::mh_InitNode(C_TblTreItem * const opc_TreeNode, const uint32 ou32_NodeIndex,
                                       const uint32 ou32_ViewIndex)
{
   if (opc_TreeNode != NULL)
   {
      const C_OSCNode * const pc_NodeData = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

      //Index
      opc_TreeNode->u32_Index = ou32_NodeIndex;
      tgl_assert(pc_NodeData != NULL);
      if (pc_NodeData != NULL)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);
         //Name
         opc_TreeNode->c_Name = pc_NodeData->c_Properties.c_Name.c_str();
         //Icon
         opc_TreeNode->c_Icon = QIcon(C_SyvDaItPaTreeModel::mhc_ICON_NODE);
         //State
         if (pc_View != NULL)
         {
            opc_TreeNode->q_Enabled = static_cast<bool>(pc_View->GetNodeActiveFlags()[ou32_NodeIndex]);
            opc_TreeNode->q_Selectable = false;

            if (opc_TreeNode->q_Enabled == false)
            {
               opc_TreeNode->c_Name += C_GtGetText::h_GetText(" (Inactive in view)");
            }
            else
            {
               // Check a further reason for deactivation
               bool q_Error;
               tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(
                             ou32_ViewIndex,
                             ou32_NodeIndex,
                             q_Error) == C_NO_ERR);

               if (q_Error == true)
               {
                  opc_TreeNode->q_Enabled = false;
                  opc_TreeNode->c_Name += C_GtGetText::h_GetText(
                     " (Disabled flags)");
               }
            }
         }
      }
   }
} //lint !e429  //no memory leak because we never took ownership

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize data pool tree node

   \param[in,out]  opc_TreeNode        Tree node to initialize
   \param[in]      ou32_NodeIndex      Node index (identifier)
   \param[in]      ou32_DataPoolIndex  Data pool index (identifier)
   \param[in]      oq_Enabled          Info if element should be enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::mh_InitDataPool(C_TblTreItem * const opc_TreeNode, const uint32 ou32_NodeIndex,
                                           const uint32 ou32_DataPoolIndex, const bool oq_Enabled)
{
   if (opc_TreeNode != NULL)
   {
      const C_OSCNodeDataPool * const pc_NodeDataPoolData = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         ou32_NodeIndex,
         ou32_DataPoolIndex);

      //Index
      opc_TreeNode->u32_Index = ou32_DataPoolIndex;
      tgl_assert(pc_NodeDataPoolData != NULL);
      if (pc_NodeDataPoolData != NULL)
      {
         //Name
         opc_TreeNode->c_Name = pc_NodeDataPoolData->c_Name.c_str();
         //Icon
         opc_TreeNode->c_Icon = QIcon(C_SyvDaItPaTreeModel::mhc_ICON_DATAPOOL);
         //State
         opc_TreeNode->q_Enabled = oq_Enabled;
         opc_TreeNode->q_Selectable = false;
      }
   }
} //lint !e429  //no memory leak because we never took ownership

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize list tree node

   \param[in,out]  opc_TreeNode        Tree node to initialize
   \param[in]      ou32_NodeIndex      Node index (identifier)
   \param[in]      ou32_DataPoolIndex  Data pool index (identifier)
   \param[in]      ou32_ListIndex      List index (identifier)
   \param[in]      oq_Enabled          Info if element should be enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::mh_InitList(C_TblTreItem * const opc_TreeNode, const uint32 ou32_NodeIndex,
                                       const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex,
                                       const bool oq_Enabled)
{
   if (opc_TreeNode != NULL)
   {
      const C_OSCNodeDataPoolList * const pc_ListData = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         ou32_NodeIndex,
         ou32_DataPoolIndex,
         ou32_ListIndex);

      //Index
      opc_TreeNode->u32_Index = ou32_ListIndex;
      tgl_assert(pc_ListData != NULL);
      if (pc_ListData != NULL)
      {
         //Name
         opc_TreeNode->c_Name = static_cast<QString>("%1 (%2)").arg(pc_ListData->c_Name.c_str()).arg(
            pc_ListData->c_Elements.size());
         //Icon
         opc_TreeNode->c_Icon = QIcon(C_SyvDaItPaTreeModel::mhc_ICON_LIST);
         //State
         opc_TreeNode->q_Enabled = oq_Enabled;
         opc_TreeNode->q_Selectable = false;
      }
   }
} //lint !e429  //no memory leak because we never took ownership

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize element tree node

   \param[in,out]  opc_TreeNode        Tree node to initialize
   \param[in]      ou32_NodeIndex      Node index (identifier)
   \param[in]      ou32_DataPoolIndex  Data pool index (identifier)
   \param[in]      ou32_ListIndex      List index (identifier)
   \param[in]      ou32_ElementIndex   Element index (identifier)
   \param[in]      oq_Enabled          Info if element should be enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeModel::mh_InitElement(C_TblTreItem * const opc_TreeNode, const uint32 ou32_NodeIndex,
                                          const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex,
                                          const uint32 ou32_ElementIndex, const bool oq_Enabled)
{
   if (opc_TreeNode != NULL)
   {
      const C_OSCNodeDataPoolListElement * const pc_ElementData =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
            ou32_NodeIndex,
            ou32_DataPoolIndex,
            ou32_ListIndex, ou32_ElementIndex);

      //Index
      opc_TreeNode->u32_Index = ou32_ElementIndex;
      tgl_assert(pc_ElementData != NULL);
      if (pc_ElementData != NULL)
      {
         //Name
         opc_TreeNode->c_Name = static_cast<QString>("%1 - %2").arg(ou32_ElementIndex + 1).arg(
            pc_ElementData->c_Name.c_str());
         //Icon
         opc_TreeNode->c_Icon = QIcon(C_SyvDaItPaTreeModel::mhc_ICON_PARAMETER);
         //State
         opc_TreeNode->q_Enabled = oq_Enabled;
         opc_TreeNode->q_Selectable = false;
      }
   }
} //lint !e429  //no memory leak because we never took ownership

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value to use for set (wrapper to handle data set or direct value)

   \param[in]  ou32_DataIndex    Data index in param item

   \return
   NULL Value not found
   Else Valid value
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolContent * C_SyvDaItPaTreeModel::m_GetSetValue(const uint32 ou32_DataIndex) const
{
   const C_OSCNodeDataPoolContent * pc_Retval = NULL;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if ((((pc_Param != NULL) && (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_ListValues.size())) &&
           (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_DataSetSelectionIndices.size())) &&
          (ou32_DataIndex < pc_Param->c_DataPoolElementsConfig.size()))
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[ou32_DataIndex];
         if (rc_Config.c_ElementId.GetIsValid() == true)
         {
            if (pc_Param->c_DataSetSelectionIndices[ou32_DataIndex] >= 0L)
            {
               const uint32 u32_DataSetIndex = static_cast<uint32>(pc_Param->c_DataSetSelectionIndices[ou32_DataIndex]);
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                             rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                             rc_Config.c_ElementId.u32_ListIndex,
                                                                             rc_Config.c_ElementId.u32_ElementIndex);
               if ((pc_Element != NULL) && (u32_DataSetIndex < pc_Element->c_DataSetValues.size()))
               {
                  pc_Retval = &pc_Element->c_DataSetValues[u32_DataSetIndex];
               }
            }
            else
            {
               pc_Retval = &pc_Param->c_ListValues[ou32_DataIndex];
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if index contains changed elements

   \param[in]  orc_Index   Index to check

   \return
   True  Change
   False No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::m_IsChanged(const QModelIndex & orc_Index) const
{
   bool q_Retval = false;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
      if (pc_ParamData != NULL)
      {
         C_OSCNodeDataPoolListElementId c_Id;
         uint32 u32_ValidLayers;
         C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
         switch (u32_ValidLayers)
         {
         case 0UL:
            for (uint32 u32_ItConfig = 0UL;
                 (u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size()) && (q_Retval == false); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (m_CheckListCRCIsChanged(rc_Config.c_ElementId) == true)
                  {
                     q_Retval = true;
                  }
                  if (m_CheckElementIsChanged(rc_Config.c_ElementId) == true)
                  {
                     q_Retval = true;
                  }
               }
            }
            break;
         case 1UL:
            for (uint32 u32_ItConfig = 0UL;
                 (u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size()) && (q_Retval == false); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (rc_Config.c_ElementId.u32_NodeIndex == c_Id.u32_NodeIndex)
                  {
                     if (m_CheckListCRCIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                     if (m_CheckElementIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                  }
               }
            }
            break;
         case 2UL:
            for (uint32 u32_ItConfig = 0UL;
                 (u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size()) && (q_Retval == false); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if ((rc_Config.c_ElementId.u32_NodeIndex == c_Id.u32_NodeIndex) &&
                      (rc_Config.c_ElementId.u32_DataPoolIndex == c_Id.u32_DataPoolIndex))
                  {
                     if (m_CheckListCRCIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                     if (m_CheckElementIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                  }
               }
            }
            break;
         case 3UL:
            for (uint32 u32_ItConfig = 0UL;
                 (u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size()) && (q_Retval == false); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (((rc_Config.c_ElementId.u32_NodeIndex == c_Id.u32_NodeIndex) &&
                       (rc_Config.c_ElementId.u32_DataPoolIndex == c_Id.u32_DataPoolIndex)) &&
                      (rc_Config.c_ElementId.u32_ListIndex == c_Id.u32_ListIndex))
                  {
                     if (m_CheckListCRCIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                     if (m_CheckElementIsChanged(rc_Config.c_ElementId) == true)
                     {
                        q_Retval = true;
                     }
                  }
               }
            }
            break;
         case 4UL:
            //4.th layer never gets * marker
            q_Retval = false;
            break;
         default:
            //Skip
            break;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if list CRC is valid

   \param[in]  orc_Id   List ID to check

   \return
   True  Not valid
   False Valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::m_CheckListCRCIsChanged(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   bool q_Retval = false;
   bool q_Read = true;
   //Check if read

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (pc_Param->c_DataPoolElementsConfig.size() == this->mc_ECUValuesReadStatus.size())
         {
            for (uint32 u32_ItElement = 0UL;
                 (u32_ItElement < this->mc_ECUValuesReadStatus.size()) && (q_Read == true); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItElement];
               if ((((rc_Config.c_ElementId.GetIsValid() == true) &&
                     (rc_Config.c_ElementId.u32_NodeIndex == orc_Id.u32_NodeIndex)) &&
                    (rc_Config.c_ElementId.u32_DataPoolIndex == orc_Id.u32_DataPoolIndex)) &&
                   (rc_Config.c_ElementId.u32_ListIndex == orc_Id.u32_ListIndex))
               {
                  if (this->mc_ECUValuesReadStatus[u32_ItElement] == false)
                  {
                     q_Read = false;
                  }
               }
            }
         }
      }
   }

   //If not read assume it does not change
   if (q_Read == true)
   {
      uint32 u32_ItFinalIndex;
      const bool q_Found = m_GetListIndex(orc_Id, u32_ItFinalIndex);

      if ((q_Found == true) && (u32_ItFinalIndex < this->mc_ECUCRCValidStatus.size()))
      {
         if (this->mc_ECUCRCValidStatus[u32_ItFinalIndex] == false)
         {
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if element was changed

   \param[in]  orc_Id   Element ID to check

   \return
   True  Change
   False No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::m_CheckElementIsChanged(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   bool q_Retval = false;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (((this->mc_ECUValuesReadStatus.size() == this->mc_ECUValues.size()) &&
              (this->mc_ECUValues.size() == pc_Param->c_ListValues.size())) &&
             (pc_Param->c_DataPoolElementsConfig.size() == pc_Param->c_ListValues.size()))
         {
            for (uint32 u32_ItElement = 0UL;
                 (u32_ItElement < this->mc_ECUValuesReadStatus.size()) && (q_Retval == false); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Param->c_DataPoolElementsConfig[u32_ItElement];
               if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementId == orc_Id))
               {
                  //If not read assume it does not change
                  if (this->mc_ECUValuesReadStatus[u32_ItElement] == true)
                  {
                     const C_OSCNodeDataPoolContent * const pc_Value = m_GetSetValue(u32_ItElement);
                     if (pc_Value != NULL)
                     {
                        if (this->mc_ECUValues[u32_ItElement] == *pc_Value)
                        {
                           //No change
                        }
                        else
                        {
                           q_Retval = true;
                        }
                     }
                     else
                     {
                        q_Retval = true;
                     }
                  }
               }
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get list index in internal CRC structure

   \param[in]   orc_Id        List ID
   \param[out]  oru32_Index   Index in internal CRC structure

   \return
   True  Found
   False Not found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeModel::m_GetListIndex(const C_OSCNodeDataPoolListElementId & orc_Id, uint32 & oru32_Index) const
{
   bool q_Found = false;

   oru32_Index = 0UL;

   if (this->mpc_InvisibleRootItem != NULL)
   {
      //Node
      for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItAll = this->mpc_InvisibleRootItem->c_Children.begin();
           (c_ItAll != this->mpc_InvisibleRootItem->c_Children.end()) && (q_Found == false);
           ++c_ItAll)
      {
         const C_TblTreItem * const pc_AllNode = dynamic_cast<const C_TblTreItem * const>(*c_ItAll);
         if (pc_AllNode != NULL)
         {
            //Node
            for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItNode = pc_AllNode->c_Children.begin();
                 (c_ItNode != pc_AllNode->c_Children.end()) && (q_Found == false); ++c_ItNode)
            {
               const C_TblTreItem * const pc_Node = dynamic_cast<const C_TblTreItem * const>(*c_ItNode);
               if (pc_Node != NULL)
               {
                  //Datapool
                  for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItDp = pc_Node->c_Children.begin();
                       (c_ItDp != pc_Node->c_Children.end()) && (q_Found == false); ++c_ItDp)
                  {
                     const C_TblTreItem * const pc_Dp = dynamic_cast<const C_TblTreItem * const>(*c_ItDp);
                     if (pc_Dp != NULL)
                     {
                        //List
                        for (std::vector<C_TblTreSimpleItem *>::const_iterator c_ItList = pc_Dp->c_Children.begin();
                             (c_ItList != pc_Dp->c_Children.end()) && (q_Found == false);
                             ++c_ItList)
                        {
                           const C_TblTreItem * const pc_List = dynamic_cast<const C_TblTreItem * const>(*c_ItList);
                           if (pc_List != NULL)
                           {
                              //Check match
                              if (((pc_Node->u32_Index == orc_Id.u32_NodeIndex) &&
                                   (pc_Dp->u32_Index == orc_Id.u32_DataPoolIndex)) &&
                                  (pc_List->u32_Index == orc_Id.u32_ListIndex))
                              {
                                 //Stop
                                 q_Found = true;
                              }
                              else
                              {
                                 //Try next one
                                 ++oru32_Index;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return q_Found;
   //lint -e{1763} Its a const iterator!
}
