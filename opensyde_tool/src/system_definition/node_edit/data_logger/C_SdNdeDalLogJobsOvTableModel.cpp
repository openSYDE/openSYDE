//----------------------------------------------------------------------------------------------------------------------
/*!
   \file

   \brief       Log Jobs overview table model

   Log Jobs overview table model

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobsOvTableModel.hpp"
#include "C_GtGetText.hpp"
#include "constants.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_SdNdeDalLogJobsOvTableModel::C_SdNdeDalLogJobsOvTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableModel::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update data

   \param[in]  orc_AllLoggerJobElementsLocationCount     vector holds all Logjobs Index, Local elements count, remote elements count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableModel::UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                                            uint32_t> > & orc_AllLoggerJobElementsLocationCount)
{
   this->beginResetModel();
   this->m_FillLogJobsInfo(orc_AllLoggerJobElementsLocationCount);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDalLogJobsOvTableModel::GetNodeIndex() const
{
   return this->mu32_NodeIndex;
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
QVariant C_SdNdeDalLogJobsOvTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                                   const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeDalLogJobsOvTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = "#";
            break;
         case eICON:
            c_Retval = C_GtGetText::h_GetText("Icon");
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
         case eTRIGGERMODE:
            c_Retval = C_GtGetText::h_GetText("Trigger Mode");
            break;
         case eLOGGINGDATA:
            c_Retval = C_GtGetText::h_GetText("Logging Data");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eICON:
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
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
         case eTRIGGERMODE:
            c_Retval = C_GtGetText::h_GetText("Trigger Mode");
            break;
         case eLOGGINGDATA:
            c_Retval = C_GtGetText::h_GetText("Logging Data");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eICON:
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Continuous index for all LogJobs");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("LogJob name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("LogJob comment");
            break;
         case eUSE_CASE:
            c_Retval = C_GtGetText::h_GetText("Selected use case");
            break;
         case eTRIGGERMODE:
            c_Retval = C_GtGetText::h_GetText("LogJob Trigger mode");
            break;
         case eLOGGINGDATA:
            c_Retval = C_GtGetText::h_GetText("LogJob Logging data");
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
int32_t C_SdNdeDalLogJobsOvTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_LogJobsInfoAll.size();
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
int32_t C_SdNdeDalLogJobsOvTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 7;
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
QVariant C_SdNdeDalLogJobsOvTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDalLogJobsOvTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const int32_t s32_Index = orc_Index.row();
         if (static_cast<uint32_t>(s32_Index) < this->mc_LogJobsInfoAll.size())
         {
            switch (e_Col)
            {
            case eINDEX:
               c_Retval = orc_Index.row() + 1;
               break;
            case eICON:
               break;
            case eNAME:
               c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobName;
               break;
            case eCOMMENT:
               c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobComment;
               break;
            case eUSE_CASE:
               c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobUseCase;
               break;
            case eTRIGGERMODE:
               c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobTriggerMode;
               break;
            case eLOGGINGDATA:
               c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobLoggingData;
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         switch (e_Col)
         {
         case eICON:
            {
               const int32_t s32_Index = orc_Index.row();
               if (static_cast<uint32_t>(s32_Index) < this->mc_LogJobsInfoAll.size())
               {
                  c_Retval = this->mc_LogJobsInfoAll[s32_Index].c_LogJobIcon;
               }
            }
            break;
         case eINDEX:
         case eNAME:
         case eCOMMENT:
         case eUSE_CASE:
         case eTRIGGERMODE:
         case eLOGGINGDATA:
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
         case eICON:
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eINDEX:
         case eNAME:
         case eUSE_CASE:
         case eTRIGGERMODE:
         case eLOGGINGDATA:
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
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsOvTableModel::E_Columns C_SdNdeDalLogJobsOvTableModel::h_ColumnToEnum(const int32_t & ors32_Column)
{
   C_SdNdeDalLogJobsOvTableModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eINDEX;
      break;
   case 1:
      e_Retval = eICON;
      break;
   case 2:
      e_Retval = eNAME;
      break;
   case 3:
      e_Retval = eCOMMENT;
      break;
   case 4:
      e_Retval = eUSE_CASE;
      break;
   case 5:
      e_Retval = eTRIGGERMODE;
      break;
   case 6:
      e_Retval = eLOGGINGDATA;
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
int32_t C_SdNdeDalLogJobsOvTableModel::h_EnumToColumn(const C_SdNdeDalLogJobsOvTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eICON:
      s32_Retval = 1;
      break;
   case eNAME:
      s32_Retval = 2;
      break;
   case eCOMMENT:
      s32_Retval = 3;
      break;
   case eUSE_CASE:
      s32_Retval = 4;
      break;
   case eTRIGGERMODE:
      s32_Retval = 5;
      break;
   case eLOGGINGDATA:
      s32_Retval = 6;
      break;
   default:
      s32_Retval = -1;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collects all LogJobs data for overview table

   \param[in]  oc_AllLoggerJobElementsLocationCount     vector holds all Logjobs Index, Local elements count, remote elements count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsOvTableModel::m_FillLogJobsInfo(const std::vector<std::tuple<uint32_t, uint32_t,
                                                                                   uint32_t> > & orc_AllLoggerJobElementsLocationCount)
{
   const C_OscNode * const pc_Node =
      C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   this->mc_LogJobsInfoAll.clear();
   if (pc_Node != NULL)
   {
      for (uint32_t u32_It = 0UL; u32_It < pc_Node->c_DataLoggerJobs.size(); ++u32_It)
      {
         C_LogJobOvTableData c_LogJobInfo;
         QStringList c_Tmp;
         const QString c_IconPath = "://images/IconLogging.svg";
         c_LogJobInfo.c_Index = QString::number(u32_It);
         c_Tmp.push_back(QString::number(21)); // icon size
         c_Tmp.push_back(c_IconPath);
         c_LogJobInfo.c_LogJobIcon = c_Tmp;
         //Log job name
         c_LogJobInfo.c_LogJobName = pc_Node->c_DataLoggerJobs[u32_It].c_Properties.c_Name.c_str();
         //Log job comment
         c_LogJobInfo.c_LogJobComment = pc_Node->c_DataLoggerJobs[u32_It].c_Properties.c_Comment.c_str();
         //Log job UseCase
         switch (pc_Node->c_DataLoggerJobs[u32_It].c_Properties.e_UseCase)
         {
         case  C_OscDataLoggerJobProperties::E_UseCase::eUC_AWS:
            c_LogJobInfo.c_LogJobUseCase = "AWS";
            break;
         case  C_OscDataLoggerJobProperties::E_UseCase::eUC_MACHINES_CLOUD:
            c_LogJobInfo.c_LogJobUseCase = "machines.cloud";
            break;
         case  C_OscDataLoggerJobProperties::E_UseCase::eUC_MANUAL:
            c_LogJobInfo.c_LogJobUseCase = "Manual";
            break;
         default:
            break;
         }
         //Log job Trigger
         if (pc_Node->c_DataLoggerJobs[u32_It].c_Properties.e_LocalLogTrigger ==
             C_OscDataLoggerJobProperties::E_LocalLogTrigger::eLLT_ON_CHANGE)
         {
            c_LogJobInfo.c_LogJobTriggerMode = "On Change";
         }
         else
         {
            c_LogJobInfo.c_LogJobTriggerMode = "Interval";
         }
         //Log job LoggingData
         if ((u32_It < orc_AllLoggerJobElementsLocationCount.size()) &&
             (u32_It == std::get<0>(orc_AllLoggerJobElementsLocationCount[u32_It])))
         {
            //LogJob local elements count
            const uint32_t u32_LocalCount = std::get<1>(orc_AllLoggerJobElementsLocationCount[u32_It]);
            //LogJob remote elements count
            const uint32_t u32_RemoteCount = std::get<2>(orc_AllLoggerJobElementsLocationCount[u32_It]);
            //LogJob local + remote elements count
            const uint32_t u32_TotalCount = u32_LocalCount + u32_RemoteCount;

            QString c_DataElementsText;
            if (u32_TotalCount == 1)
            {
               c_DataElementsText =
                  static_cast<QString>(C_GtGetText::h_GetText("%1 data element (%2 local; %3 remote)"))
                  .arg(u32_TotalCount)
                  .arg(u32_LocalCount)
                  .arg(u32_RemoteCount);
            }
            else if (u32_TotalCount > 1)
            {
               c_DataElementsText =
                  static_cast<QString>(C_GtGetText::h_GetText("%1 data elements (%2 local; %3 remote)"))
                  .arg(u32_TotalCount)
                  .arg(u32_LocalCount)
                  .arg(u32_RemoteCount);
            }
            else
            {
               c_DataElementsText = C_GtGetText::h_GetText("No data elements");
            }

            c_LogJobInfo.c_LogJobLoggingData = c_DataElementsText;
         }
         this->mc_LogJobsInfoAll.push_back(c_LogJobInfo);
      }
   }
}
