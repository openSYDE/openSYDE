//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table model

   HALC overview table model

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "C_SdNdeHalcOvTableModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   this->m_FillHalcInfo();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeHalcOvTableModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->UpdateData();
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
int32_t C_SdNdeHalcOvTableModel::MapRowToDomainIndex(const int32_t os32_Index, uint32_t & oru32_DomainIndex) const
{
   int32_t s32_Retval = C_RANGE;

   if ((static_cast<uint32_t>(os32_Index) < this->mc_HalcInfoAll.size()) && (os32_Index >= 0))
   {
      oru32_DomainIndex = this->mc_HalcInfoAll[os32_Index].u32_DomainIndex;
      s32_Retval = C_NO_ERR;
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
int32_t C_SdNdeHalcOvTableModel::MapRowToChannelNum(const int32_t os32_Index, uint32_t & oru32_ChannelNum) const
{
   int32_t s32_Retval = C_RANGE;

   if ((static_cast<uint32_t>(os32_Index) < this->mc_HalcInfoAll.size()) && (os32_Index >= 0))
   {
      if (this->mc_HalcInfoAll[os32_Index].q_DomainOnly == false)
      {
         oru32_ChannelNum = this->mc_HalcInfoAll[os32_Index].u32_ChannelIndex;
         s32_Retval = C_NO_ERR;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcOvTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                             const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeHalcOvTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
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
            c_Retval = C_GtGetText::h_GetText("Data Block Assignment");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
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
            c_Retval = C_GtGetText::h_GetText("Data Block Assignment");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
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
            c_Retval = C_GtGetText::h_GetText("The Data Block which is assigned to this Datapool");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
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
int32_t C_SdNdeHalcOvTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_HalcInfoAll.size();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcOvTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 11;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcOvTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeHalcOvTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const int32_t s32_Index = orc_Index.row();
         if (static_cast<uint32_t>(s32_Index) < this->mc_HalcInfoAll.size())
         {
            switch (e_Col)
            {
            case eINDEX:
               c_Retval = orc_Index.row() + 1;
               break;
            case eDOMAIN:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_DomainName;
               break;
            case eCHANNEL:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_ChannelName;
               break;
            case eNAME:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_CustomName;
               break;
            case eCOMMENT:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_Comment;
               break;
            case eUSE_CASE:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_UseCase;
               break;
            case eLINKED_WITH:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_LinkedWith;
               break;
            case eTYPE:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_Type;
               }
               break;
            case eDATAPOOL:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_DatapoolName;
               break;
            case eDATABLOCK_ASSIGNMENT:
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_DatablockAssignment;
               break;
            case eSAFETY_RELEVANT:
               // No text
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         if (e_Col == eSAFETY_RELEVANT)
         {
            const int32_t s32_Index = orc_Index.row();
            if (static_cast<uint32_t>(s32_Index) < this->mc_HalcInfoAll.size())
            {
               c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.s32_SafetyRelevant;
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eTYPE:
            {
               const int32_t s32_Index = orc_Index.row();
               if (static_cast<uint32_t>(s32_Index) < this->mc_HalcInfoAll.size())
               {
                  c_Retval = this->mc_HalcInfoAll[s32_Index].c_Data.c_Icon;
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
      else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
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
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
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
C_SdNdeHalcOvTableModel::E_Columns C_SdNdeHalcOvTableModel::h_ColumnToEnum(const int32_t & ors32_Column)
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
int32_t C_SdNdeHalcOvTableModel::h_EnumToColumn(const C_SdNdeHalcOvTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all HALC data for overview table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::m_FillHalcInfo(void)
{
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   this->mc_HalcInfoAll.clear();

   if (pc_Node != NULL)
   {
      uint32_t u32_ItDomain;
      for (u32_ItDomain = 0UL; u32_ItDomain < pc_Node->c_HalcConfig.GetDomainSize(); ++u32_ItDomain)
      {
         const C_OscHalcConfigDomain * const pc_Domain =
            pc_Node->c_HalcConfig.GetDomainConfigDataConst(u32_ItDomain);
         if (pc_Domain != NULL)
         {
            C_HalcTableConfig c_Config;
            c_Config.u32_DomainIndex = u32_ItDomain;

            if (pc_Domain->c_ChannelConfigs.size() == 0UL)
            {
               C_HalcTableData c_Data;

               // Domain only
               c_Config.q_DomainOnly = true;
               c_Config.u32_ChannelIndex = 0U;

               // Channel specific part
               c_Data.c_ChannelName = "-";

               // Domain specific part
               c_Data.c_DomainName = static_cast<QString>(pc_Domain->c_Name.c_str());

               // Generic config part
               m_FillHalcInfoData(*pc_Domain, pc_Domain->c_DomainConfig, c_Data);
               this->m_FillHalcInfoLinked(u32_ItDomain, 0U, false, c_Data);
               this->m_FillHalcInfoIcon(u32_ItDomain, 0U, false, pc_Domain->e_Category, c_Data);

               // Save the row
               c_Config.c_Data = c_Data;
               this->mc_HalcInfoAll.push_back(c_Config);
            }
            else
            {
               uint32_t u32_ItChannel;

               // Channel
               c_Config.q_DomainOnly = false;

               for (u32_ItChannel = 0U; u32_ItChannel < pc_Domain->c_ChannelConfigs.size(); ++u32_ItChannel)
               {
                  C_HalcTableData c_Data;

                  // Channel specific part
                  c_Config.u32_ChannelIndex = u32_ItChannel;
                  c_Data.c_ChannelName = static_cast<QString>(pc_Domain->c_Channels[u32_ItChannel].c_Name.c_str());

                  // Domain specific part
                  c_Data.c_DomainName = static_cast<QString>(pc_Domain->c_Name.c_str());

                  // Generic config part
                  m_FillHalcInfoData(*pc_Domain, pc_Domain->c_ChannelConfigs[u32_ItChannel], c_Data);
                  this->m_FillHalcInfoLinked(u32_ItDomain, u32_ItChannel, true, c_Data);
                  this->m_FillHalcInfoIcon(u32_ItDomain, u32_ItChannel, true, pc_Domain->e_Category, c_Data);

                  // Save the row
                  c_Config.c_Data = c_Data;
                  this->mc_HalcInfoAll.push_back(c_Config);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collects all HALC linked data for overview table

   \param[in]       ou32_DomainIndex     Index of current domain
   \param[in]       ou32_ChannelIndex    Index of current channel in domain (if oq_UseChannelIndex == true)
   \param[in]       oq_UseChannelIndex   Flag if channel index is used
   \param[in,out]   orc_Data             Row data for channel/domain for data function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::m_FillHalcInfoLinked(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                   const bool oq_UseChannelIndex,
                                                   C_SdNdeHalcOvTableModel::C_HalcTableData & orc_Data)
{
   bool q_Tmp;

   std::vector<QString> c_LinkedChannelNames;

   C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex,
                                                                 ou32_DomainIndex, ou32_ChannelIndex,
                                                                 oq_UseChannelIndex,
                                                                 q_Tmp, &c_LinkedChannelNames);

   if (c_LinkedChannelNames.size() > 0UL)
   {
      QStringList c_Tmp;
      for (uint32_t u32_ItLi = 0UL; u32_ItLi < c_LinkedChannelNames.size(); ++u32_ItLi)
      {
         if (c_Tmp.isEmpty() == false)
         {
            c_Tmp += ", ";
         }
         c_Tmp += c_LinkedChannelNames[u32_ItLi];
      }
      orc_Data.c_LinkedWith = c_Tmp;
   }
   else
   {
      orc_Data.c_LinkedWith = "-";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collects all HALC icon data for overview table

   \param[in]       ou32_DomainIndex     Index of current domain
   \param[in]       ou32_ChannelIndex    Index of current channel in domain (if oq_UseChannelIndex == true)
   \param[in]       oq_UseChannelIndex   Flag if channel index is used
   \param[in]       oe_Category          Category of current domain
   \param[in,out]   orc_Data             Row data for channel/domain for data function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::m_FillHalcInfoIcon(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                 const bool oq_UseChannelIndex,
                                                 const C_OscHalcDefDomain::E_Category oe_Category,
                                                 C_SdNdeHalcOvTableModel::C_HalcTableData & orc_Data)
{
   //Linked and error
   bool q_IsLinked = false;
   bool q_Error;
   QStringList c_Tmp;

   C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex,
                                                                 ou32_DomainIndex, ou32_ChannelIndex,
                                                                 oq_UseChannelIndex, q_IsLinked, NULL);
   C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelError(this->mu32_NodeIndex,
                                                                ou32_DomainIndex, ou32_ChannelIndex,
                                                                oq_UseChannelIndex, q_Error);

   c_Tmp.push_back(QString::number(21)); // icon size
   {
      QString c_Path = "://images/system_definition/NodeEdit/halc/";
      switch (oe_Category)
      {
      case C_OscHalcDefDomain::eCA_INPUT:
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
      case C_OscHalcDefDomain::eCA_OUTPUT:
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
      case C_OscHalcDefDomain::eCA_OTHER:
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
      orc_Data.c_Type = c_Path;
   }
   orc_Data.c_Icon = c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collects all HALC data for overview table

   \param[in]       orc_Domain    Domain specific configuration of current domain
   \param[in]       orc_Config    Generic configuration of current channel (or domain if domain has no channels)
   \param[in,out]   orc_Data      Row data for channel/domain for data function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcOvTableModel::m_FillHalcInfoData(const C_OscHalcConfigDomain & orc_Domain,
                                                 const C_OscHalcConfigChannel & orc_Config,
                                                 C_SdNdeHalcOvTableModel::C_HalcTableData & orc_Data)
{
   const C_OscNodeApplication * pc_Application = NULL;
   const C_OscNodeDataPool * pc_Datapool;

   orc_Data.c_CustomName = static_cast<QString>(orc_Config.c_Name.c_str());
   orc_Data.c_Comment = static_cast<QString>(orc_Config.c_Comment.c_str());

   if (orc_Config.u32_UseCaseIndex < orc_Domain.c_ChannelUseCases.size())
   {
      const C_OscHalcDefChannelUseCase & rc_UseCase = orc_Domain.c_ChannelUseCases[orc_Config.u32_UseCaseIndex];
      orc_Data.c_UseCase = static_cast<QString>(rc_UseCase.c_Display.c_str());
   }
   else
   {
      orc_Data.c_UseCase = "-";
   }

   orc_Data.c_DatapoolName = C_OscHalcMagicianUtil::h_GetDatapoolName(orc_Config.q_SafetyRelevant).c_str();

   if (orc_Config.q_SafetyRelevant == true)
   {
      orc_Data.s32_SafetyRelevant = static_cast<int32_t>(Qt::Checked);
   }
   else
   {
      orc_Data.s32_SafetyRelevant = static_cast<int32_t>(Qt::Unchecked);
   }

   // Datablock assignment
   pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetHalcDatapool(this->mu32_NodeIndex, orc_Config.q_SafetyRelevant);
   if ((pc_Datapool != NULL) && (pc_Datapool->s32_RelatedDataBlockIndex >= 0))
   {
      pc_Application =
         C_PuiSdHandler::h_GetInstance()->
         GetApplication(this->mu32_NodeIndex, static_cast<uint32_t>(pc_Datapool->s32_RelatedDataBlockIndex));
   }

   if (pc_Application != NULL)
   {
      orc_Data.c_DatablockAssignment = static_cast<QString>(pc_Application->c_Name.c_str());
   }
   else
   {
      orc_Data.c_DatablockAssignment = C_GtGetText::h_GetText("<not assigned>");
   }
}
