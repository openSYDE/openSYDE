//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table model

   HALC overview table model

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCHALCMagicianUtil.h"
#include "C_SdNdeHalcOvTableModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcOvTableModel::C_SdNdeHalcOvTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::UpdateData(void)
{
   this->beginResetModel();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeHalcOvTableModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::SetNodeIndex(const uint32 ou32_NodeIndex)
{
   this->beginResetModel();
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to data

   \param[in]  os32_Index  Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcConfigChannel * C_SdNdeHalcOvTableModel::MapRowToChannel(const sint32 os32_Index) const
{
   const C_OSCHalcConfigChannel * pc_Retval = NULL;

   if (os32_Index >= 0L)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               const uint32 u32_Start = u32_Counter;
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
               if (static_cast<uint32>(os32_Index) < u32_Counter)
               {
                  if (pc_Domain->c_ChannelConfigs.size() > 0UL)
                  {
                     pc_Retval =
                        &pc_Domain->c_ChannelConfigs[static_cast<std::vector<C_OSCHalcConfigChannel>::size_type>(
                                                        static_cast<uint32>(os32_Index) - u32_Start)];
                  }
                  else
                  {
                     pc_Retval = &pc_Domain->c_DomainConfig;
                  }
                  //ensure stop, otherwise this won't work
                  break;
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to domain index

   \param[in]   os32_Index          Index
   \param[out]  oru32_DomainIndex   Domain index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcOvTableModel::MapRowToDomainIndex(const sint32 os32_Index, uint32 & oru32_DomainIndex) const
{
   sint32 s32_Retval = C_RANGE;

   if (os32_Index >= 0L)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
               if (static_cast<uint32>(os32_Index) < u32_Counter)
               {
                  oru32_DomainIndex = u32_ItDomain;
                  s32_Retval = C_NO_ERR;
                  //ensure stop, otherwise this won't work
                  break;
               }
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to channel num

   \param[in]   os32_Index          Index
   \param[out]  oru32_ChannelNum    Channel num

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcOvTableModel::MapRowToChannelNum(const sint32 os32_Index, uint32 & oru32_ChannelNum) const
{
   sint32 s32_Retval = C_RANGE;

   if (os32_Index >= 0L)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               const uint32 u32_Start = u32_Counter;
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
               if (static_cast<uint32>(os32_Index) < u32_Counter)
               {
                  if (pc_Domain->c_ChannelConfigs.size() > 0UL)
                  {
                     oru32_ChannelNum = static_cast<uint32>(os32_Index) - u32_Start;
                     s32_Retval = C_NO_ERR;
                  }
                  else
                  {
                     //None
                  }
                  //ensure stop, otherwise this won't work
                  break;
               }
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to channel def

   \param[in]  os32_Index  Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcDefChannelDef * C_SdNdeHalcOvTableModel::MapRowToChannelDef(const sint32 os32_Index) const
{
   const C_OSCHalcDefChannelDef * pc_Retval = NULL;

   if (os32_Index >= 0L)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               const uint32 u32_Start = u32_Counter;
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
               if (static_cast<uint32>(os32_Index) < u32_Counter)
               {
                  if (pc_Domain->c_ChannelConfigs.size() > 0UL)
                  {
                     pc_Retval =
                        &pc_Domain->c_Channels[static_cast<std::vector<C_OSCHalcDefChannelDef>::size_type>(
                                                  static_cast<uint32>(os32_Index) - u32_Start)];
                  }
                  else
                  {
                     //None
                  }
                  //ensure stop, otherwise this won't work
                  break;
               }
            }
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Map row to domain

   \param[in]  os32_Index  Index

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCHalcConfigDomain * C_SdNdeHalcOvTableModel::MapRowToDomain(const sint32 os32_Index) const
{
   const C_OSCHalcConfigDomain * pc_Retval = NULL;

   if (os32_Index >= 0L)
   {
      const C_OSCNode * const pc_Node =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
               if (static_cast<uint32>(os32_Index) < u32_Counter)
               {
                  pc_Retval = pc_Domain;

                  //ensure stop, otherwise this won't work
                  break;
               }
            }
         }
      }
   }
   return pc_Retval;
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
QVariant C_SdNdeHalcOvTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                             const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeHalcOvTableModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = "#";
            break;
         case eDOMAIN:
            c_Retval = C_GtGetText::h_GetText("Domain");
            break;
         case eCHANNEL:
            c_Retval = C_GtGetText::h_GetText("Channel");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eUSE_CASE:
            c_Retval = C_GtGetText::h_GetText("Use Case");
            break;
         case eLINKED_WITH:
            c_Retval = C_GtGetText::h_GetText("Linked with");
            break;
         case eSAFETY_RELEVANT:
            c_Retval = C_GtGetText::h_GetText("Safety");
            break;
         case eTYPE:
            c_Retval = "";
            break;
         case eDATAPOOL:
            c_Retval = C_GtGetText::h_GetText("Datapool");
            break;
         case eDATABLOCK_ASSIGNMENT:
            c_Retval = C_GtGetText::h_GetText("Datablock Assignment");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eDOMAIN:
            c_Retval = C_GtGetText::h_GetText("Domain");
            break;
         case eCHANNEL:
            c_Retval = C_GtGetText::h_GetText("Channel");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eUSE_CASE:
            c_Retval = C_GtGetText::h_GetText("Use Case");
            break;
         case eLINKED_WITH:
            c_Retval = C_GtGetText::h_GetText("Linked with");
            break;
         case eSAFETY_RELEVANT:
            c_Retval = C_GtGetText::h_GetText("Safety");
            break;
         case eTYPE:
            c_Retval = C_GtGetText::h_GetText("Type");
            break;
         case eDATAPOOL:
            c_Retval = C_GtGetText::h_GetText("Datapool");
            break;
         case eDATABLOCK_ASSIGNMENT:
            c_Retval = C_GtGetText::h_GetText("Datablock Assignment");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Continuous index for all channels and domains");
            break;
         case eDOMAIN:
            c_Retval = C_GtGetText::h_GetText("Parent domain");
            break;
         case eCHANNEL:
            c_Retval = C_GtGetText::h_GetText("Channel name");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("User channel name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("User comment");
            break;
         case eUSE_CASE:
            c_Retval = C_GtGetText::h_GetText("Selected use case");
            break;
         case eLINKED_WITH:
            c_Retval = C_GtGetText::h_GetText("Channel which are linked with this channel");
            break;
         case eSAFETY_RELEVANT:
            c_Retval = C_GtGetText::h_GetText("Flag if this channel was marked as safety relevant");
            break;
         case eTYPE:
            c_Retval = C_GtGetText::h_GetText("A general type for this channel");
            break;
         case eDATAPOOL:
            c_Retval = C_GtGetText::h_GetText("Datapool where this channel is stored");
            break;
         case eDATABLOCK_ASSIGNMENT:
            c_Retval = C_GtGetText::h_GetText("The datablock which is assigned to this datapool");
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
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcOvTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         uint32 u32_Counter = 0UL;
         for (uint32 u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HALCConfig.GetDomainSize(); ++u32_ItDomain)
         {
            const C_OSCHalcConfigDomain * const pc_Domain =
               pc_Node->c_HALCConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  u32_Counter += pc_Domain->c_ChannelConfigs.size();
               }
               else
               {
                  u32_Counter += 1UL;
               }
            }
         }
         sn_Retval = static_cast<sintn>(u32_Counter);
      }
      else
      {
         sn_Retval = 0;
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcOvTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      sn_Retval = 11;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcOvTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeHalcOvTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         const C_OSCHalcConfigDomain * pc_Domain;
         const C_OSCHalcDefChannelDef * pc_Def;
         const C_OSCHalcConfigChannel * pc_Data;
         const C_OSCNode * pc_Node;
         uint32 u32_DomainNum;
         uint32 u32_ChanNum;
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = orc_Index.row() + 1;
            break;
         case eDOMAIN:
            pc_Domain = this->MapRowToDomain(orc_Index.row());
            if (pc_Domain != NULL)
            {
               c_Retval = static_cast<QString>(pc_Domain->c_Name.c_str());
            }
            break;
         case eCHANNEL:
            pc_Def = this->MapRowToChannelDef(orc_Index.row());
            if (pc_Def != NULL)
            {
               c_Retval = static_cast<QString>(pc_Def->c_Name.c_str());
            }
            else
            {
               c_Retval = "-";
            }
            break;
         case eNAME:
            pc_Data = this->MapRowToChannel(orc_Index.row());
            if (pc_Data != NULL)
            {
               c_Retval = static_cast<QString>(pc_Data->c_Name.c_str());
            }
            break;
         case eCOMMENT:
            pc_Data = this->MapRowToChannel(orc_Index.row());
            if (pc_Data != NULL)
            {
               c_Retval = static_cast<QString>(pc_Data->c_Comment.c_str());
            }
            break;
         case eUSE_CASE:
            pc_Domain = this->MapRowToDomain(orc_Index.row());
            pc_Data = this->MapRowToChannel(orc_Index.row());
            if ((pc_Data != NULL) && (pc_Domain != NULL))
            {
               if (pc_Data->u32_UseCaseIndex < pc_Domain->c_ChannelUseCases.size())
               {
                  const C_OSCHalcDefChannelUseCase & rc_UseCase =
                     pc_Domain->c_ChannelUseCases[pc_Data->u32_UseCaseIndex];
                  c_Retval = static_cast<QString>(rc_UseCase.c_Display.c_str());
               }
               else
               {
                  c_Retval = "-";
               }
            }
            else
            {
               c_Retval = "-";
            }
            break;
         case eLINKED_WITH:
            if (this->MapRowToDomainIndex(orc_Index.row(), u32_DomainNum) == C_NO_ERR)
            {
               bool q_Tmp;
               std::vector<QString> c_LinkedChannelNames;
               if (this->MapRowToChannelNum(orc_Index.row(), u32_ChanNum) == C_NO_ERR)
               {
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelLinked(this->mu32_NodeIndex,
                                                                                u32_DomainNum, u32_ChanNum,
                                                                                true, q_Tmp,
                                                                                &c_LinkedChannelNames);
               }
               else
               {
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelLinked(this->mu32_NodeIndex,
                                                                                u32_DomainNum, 0UL, false,
                                                                                q_Tmp, &c_LinkedChannelNames);
               }
               if (c_LinkedChannelNames.size() > 0UL)
               {
                  QStringList c_Tmp;
                  for (uint32 u32_ItLi = 0UL; u32_ItLi < c_LinkedChannelNames.size(); ++u32_ItLi)
                  {
                     if (c_Tmp.isEmpty() == false)
                     {
                        c_Tmp += ", ";
                     }
                     c_Tmp += c_LinkedChannelNames[u32_ItLi];
                  }
                  c_Retval = c_Tmp;
               }
               else
               {
                  c_Retval = "-";
               }
            }
            else
            {
               c_Retval = "-";
            }
            break;
         case eTYPE:
            if (osn_Role == static_cast<sintn>(Qt::EditRole))
            {
               QStringList c_Tmp;
               c_Tmp = this->data(orc_Index, msn_USER_ROLE_ICON).toStringList();
               if (c_Tmp.size() > 1)
               {
                  c_Retval = c_Tmp.at(1);
               }
            }
            break;
         case eDATAPOOL:
            pc_Data = this->MapRowToChannel(orc_Index.row());
            if (pc_Data != NULL)
            {
               c_Retval = C_OSCHALCMagicianUtil::h_GetDatapoolName(pc_Data->q_SafetyRelevant).c_str();
            }
            break;
         case eDATABLOCK_ASSIGNMENT:
            pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
            pc_Data = this->MapRowToChannel(orc_Index.row());
            if ((pc_Node != NULL) && (pc_Data != NULL))
            {
               const C_OSCNodeApplication * pc_Application = NULL;
               const C_OSCNodeDataPool * const pc_Datapool =
                  C_PuiSdHandler::h_GetInstance()->GetHALCDatapool(this->mu32_NodeIndex, pc_Data->q_SafetyRelevant);
               if ((pc_Datapool != NULL) && (pc_Datapool->s32_RelatedDataBlockIndex >= 0))
               {
                  pc_Application =
                     C_PuiSdHandler::h_GetInstance()->
                     GetApplication(this->mu32_NodeIndex, static_cast<uint32>(pc_Datapool->s32_RelatedDataBlockIndex));
               }

               if (pc_Application != NULL)
               {
                  c_Retval = static_cast<QString>(pc_Application->c_Name.c_str());
               }
               else
               {
                  c_Retval = C_GtGetText::h_GetText("<not assigned>");
               }
            }
            break;
         case eSAFETY_RELEVANT:
            // No text
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         if (e_Col == eSAFETY_RELEVANT)
         {
            const C_OSCHalcConfigChannel * const pc_Data =
               this->MapRowToChannel(orc_Index.row());
            if (pc_Data != NULL)
            {
               if (pc_Data->q_SafetyRelevant == true)
               {
                  c_Retval = static_cast<sintn>(Qt::Checked);
               }
               else
               {
                  c_Retval = static_cast<sintn>(Qt::Unchecked);
               }
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         uint32 u32_DomainNum;
         uint32 u32_ChanNum;
         const C_OSCHalcConfigDomain * pc_Domain;
         switch (e_Col)
         {
         case eTYPE:
            pc_Domain = this->MapRowToDomain(orc_Index.row());
            if ((pc_Domain != NULL) && (this->MapRowToDomainIndex(orc_Index.row(), u32_DomainNum) == C_NO_ERR))
            {
               //Linked and error
               bool q_IsLinked = false;
               bool q_Error;
               if (this->MapRowToChannelNum(orc_Index.row(), u32_ChanNum) == C_NO_ERR)
               {
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelLinked(this->mu32_NodeIndex,
                                                                                u32_DomainNum, u32_ChanNum,
                                                                                true, q_IsLinked, NULL);
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelError(this->mu32_NodeIndex,
                                                                               u32_DomainNum, u32_ChanNum,
                                                                               true, q_Error);
               }
               else
               {
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelLinked(this->mu32_NodeIndex,
                                                                                u32_DomainNum, 0UL,
                                                                                false, q_IsLinked, NULL);
                  C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelError(this->mu32_NodeIndex,
                                                                               u32_DomainNum, 0UL,
                                                                               false, q_Error);
               }
               {
                  QStringList c_Tmp;
                  c_Tmp.push_back(QString::number(21)); // icon size
                  {
                     QString c_Path = "://images/system_definition/NodeEdit/halc/";
                     switch (pc_Domain->e_Category)
                     {
                     case C_OSCHalcDefDomain::eCA_INPUT:
                        if (q_IsLinked)
                        {
                           if (q_Error == true)
                           {
                              c_Path += "InputLargeLinkedError.svg";
                           }
                           else
                           {
                              c_Path += "InputLargeLinkedActive.svg";
                           }
                        }
                        else
                        {
                           if (q_Error == true)
                           {
                              c_Path += "InputLargeError.svg";
                           }
                           else
                           {
                              c_Path += "InputLargeActive.svg";
                           }
                        }
                        break;
                     case C_OSCHalcDefDomain::eCA_OUTPUT:
                        if (q_IsLinked == true)
                        {
                           if (q_Error == true)
                           {
                              c_Path += "OutputLargeLinkedError.svg";
                           }
                           else
                           {
                              c_Path += "OutputLargeLinkedActive.svg";
                           }
                        }
                        else
                        {
                           if (q_Error == true)
                           {
                              c_Path += "OutputLargeError.svg";
                           }
                           else
                           {
                              c_Path += "OutputLargeActive.svg";
                           }
                        }
                        break;
                     case C_OSCHalcDefDomain::eCA_OTHER:
                        if (q_IsLinked == true)
                        {
                           if (q_Error == true)
                           {
                              c_Path += "OtherLargeLinkedError.svg";
                           }
                           else
                           {
                              c_Path += "OtherLargeLinkedActive.svg";
                           }
                        }
                        else
                        {
                           if (q_Error == true)
                           {
                              c_Path += "OtherLargeError.svg";
                           }
                           else
                           {
                              c_Path += "OtherLargeActive.svg";
                           }
                        }
                        break;
                     default:
                        break;
                     }
                     c_Tmp.push_back(c_Path);
                  }
                  c_Retval = c_Tmp;
               }
            }
            break;
         case eINDEX:
         case eDOMAIN:
         case eCHANNEL:
         case eNAME:
         case eCOMMENT:
         case eUSE_CASE:
         case eLINKED_WITH:
         case eSAFETY_RELEVANT:
         case eDATAPOOL:
         case eDATABLOCK_ASSIGNMENT:
         default:
            //No decoration
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eINDEX:
         case eDOMAIN:
         case eCHANNEL:
         case eNAME:
         case eUSE_CASE:
         case eLINKED_WITH:
         case eSAFETY_RELEVANT:
         case eTYPE:
         case eDATAPOOL:
         case eDATABLOCK_ASSIGNMENT:
         default:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         c_Retval = mc_STYLE_GUIDE_COLOR_8;
      }
      else
      {
         //Unknown
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeHalcOvTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_SdNdeHalcOvTableModel::E_Columns e_Col = C_SdNdeHalcOvTableModel::h_ColumnToEnum(orc_Index.column());
      //Each item
      c_Retval = QAbstractTableModel::flags(orc_Index);
      //Add edit
      if (e_Col == eSAFETY_RELEVANT)
      {
         //Check box
         c_Retval = c_Retval | Qt::ItemIsUserCheckable;
      }
      else
      {
         //Nothing to add
      }
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
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
C_SdNdeHalcOvTableModel::E_Columns C_SdNdeHalcOvTableModel::h_ColumnToEnum(const sint32 & ors32_Column)
{
   C_SdNdeHalcOvTableModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eINDEX;
      break;
   case 1:
      e_Retval = eTYPE;
      break;
   case 2:
      e_Retval = eDOMAIN;
      break;
   case 3:
      e_Retval = eCHANNEL;
      break;
   case 4:
      e_Retval = eNAME;
      break;
   case 5:
      e_Retval = eCOMMENT;
      break;
   case 6:
      e_Retval = eUSE_CASE;
      break;
   case 7:
      e_Retval = eLINKED_WITH;
      break;
   case 8:
      e_Retval = eSAFETY_RELEVANT;
      break;
   case 9:
      e_Retval = eDATAPOOL;
      break;
   case 10:
      e_Retval = eDATABLOCK_ASSIGNMENT;
      break;
   default:
      //Unknown
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
sint32 C_SdNdeHalcOvTableModel::h_EnumToColumn(const C_SdNdeHalcOvTableModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eTYPE:
      s32_Retval = 1;
      break;
   case eDOMAIN:
      s32_Retval = 2;
      break;
   case eCHANNEL:
      s32_Retval = 3;
      break;
   case eNAME:
      s32_Retval = 4;
      break;
   case eCOMMENT:
      s32_Retval = 5;
      break;
   case eUSE_CASE:
      s32_Retval = 6;
      break;
   case eLINKED_WITH:
      s32_Retval = 7;
      break;
   case eSAFETY_RELEVANT:
      s32_Retval = 8;
      break;
   case eDATAPOOL:
      s32_Retval = 9;
      break;
   case eDATABLOCK_ASSIGNMENT:
      s32_Retval = 10;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}
