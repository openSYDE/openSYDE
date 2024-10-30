//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for log job data selection

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobDataSelectionTableModel.hpp"
#include "C_GtGetText.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *   *  \param[in]  opc_Parent      Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobDataSelectionTableModel::C_SdNdeDalLogJobDataSelectionTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convey data changes to table
 *
 *  \param[in]  orc_DataElements    Selected data elements to be added
 *  \param[in]  ou32_NodeIndex      Current node index

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobDataSelectionTableModel::UpdateData(
   const std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> & orc_DataElements,
   const uint32_t ou32_NodeIndex)
{
   beginResetModel();

   for (uint32_t u32_Index = 0; u32_Index < orc_DataElements.size(); u32_Index++)
   {
      C_DalLogJobData rc_JobData;
      const C_OscNodeDataPoolListElementOptArrayId & rc_ConfiguredElementOptArrayId =
         orc_DataElements[u32_Index].c_ConfiguredElementId;

      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
            rc_ConfiguredElementOptArrayId.u32_NodeIndex,
            rc_ConfiguredElementOptArrayId.u32_DataPoolIndex,
            rc_ConfiguredElementOptArrayId.u32_ListIndex,
            rc_ConfiguredElementOptArrayId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         rc_JobData.c_Comment = pc_Element->c_Comment;
         rc_JobData.c_DataElementName = pc_Element->c_Name;

         if (ou32_NodeIndex == rc_ConfiguredElementOptArrayId.u32_NodeIndex)
         {
            rc_JobData.c_Location = C_GtGetText::h_GetText("Local");
         }
         else
         {
            rc_JobData.c_Location = C_GtGetText::h_GetText("Remote");
         }

         rc_JobData.c_Namespace =
            (C_PuiSdUtil::h_GetNamespaceDatapoolElement(rc_ConfiguredElementOptArrayId)).toStdString().c_str();

         if (orc_DataElements[u32_Index].q_UseCustomName == true)
         {
            rc_JobData.c_LoggingName = orc_DataElements[u32_Index].c_CustomName;
         }
         else
         {
            rc_JobData.c_LoggingName = rc_JobData.c_Namespace;
         }

         this->mc_LoggingDataList.push_back(rc_JobData);
      }
   }
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalLogJobDataSelectionTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 5;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get header data

   \param[in]  os32_Section      Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role         Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDalLogJobDataSelectionTableModel::headerData(const int32_t os32_Section,
                                                             const Qt::Orientation oe_Orientation,
                                                             const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eDATA_ELEMENT:
            c_Retval =  C_GtGetText::h_GetText("Data Element");
            break;
         case eLOCATION:
            c_Retval =  C_GtGetText::h_GetText("Location");
            break;
         case eNAMESPACE:
            c_Retval =  C_GtGetText::h_GetText("Namespace");
            break;
         case eLOGGING_NAME:
            c_Retval =  C_GtGetText::h_GetText("Logging Name");
            break;
         case eCOMMENT:
            c_Retval =  C_GtGetText::h_GetText("Comment");
            break;
         default:
            break;
         }
      }

      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignTop);
      }
      else
      {
         //No special handling
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDalLogJobDataSelectionTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)))
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());

         if (static_cast<uint32_t>(u32_Index) < this->mc_LoggingDataList.size())
         {
            const C_DalLogJobData & rc_Data = this->mc_LoggingDataList[u32_Index];
            switch (e_Col)
            {
            case eDATA_ELEMENT:
               c_Retval =  rc_Data.c_DataElementName.c_str();
               break;
            case eLOCATION:
               c_Retval =  rc_Data.c_Location.c_str();
               break;
            case eNAMESPACE:
               c_Retval =  rc_Data.c_Namespace.c_str();
               break;
            case eLOGGING_NAME:
               c_Retval =  rc_Data.c_LoggingName.c_str();
               break;
            case eCOMMENT:
               c_Retval =  rc_Data.c_Comment.c_str();
               break;
            default:
               break;
            }
         }
      }
      else
      {
         //Unknown
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalLogJobDataSelectionTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   //   int32_t s32_Retval = 0;

   //   return s32_Retval;
   // TODO
   Q_UNUSED(orc_Parent)
   return this->mc_LoggingDataList.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get flags for item.
 *
 *  Handle editable column(s)
 *
 *  \param[in]  orc_Index   Item
 *
 *  \return
 *  Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeDalLogJobDataSelectionTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   //
   if (orc_Index.isValid() == true)
   {
      const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());

      // Default value. These flags ensure that each item is selectable
      c_Retval =  Qt::ItemIsSelectable | Qt::ItemIsEnabled;

      switch (e_Col)
      {
      case eDATA_ELEMENT:
      case eLOCATION:
      case eNAMESPACE:
      case eCOMMENT:
         break;
      case eLOGGING_NAME:
         c_Retval = c_Retval | Qt::ItemIsEditable; // Editing enabled
         break;
      default:
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index
 *
 *  Handle editable column(s).

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobDataSelectionTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                                      const int32_t os32_Role)
{
   bool q_Retval = false;

   // Update the value edited in the table in the vector that contains the model data
   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (os32_Role == static_cast<int32_t>(Qt::EditRole))
      {
         if (orc_Index.column() >= 0)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());

            const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Col =
               h_ColumnToEnum(orc_Index.column());
            switch (e_Col)
            {
            case eDATA_ELEMENT:
            case eLOCATION:
            case eNAMESPACE:
            case eCOMMENT:
               break;
            case eLOGGING_NAME:
               this->mc_LoggingDataList.at(u32_Index).c_LoggingName = orc_Value.toString().toStdString().c_str();
               q_Retval = true;
               break;
            default:
               break;
            }

            Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role);

            // Signal data changes to the data selection widget for data management
            Q_EMIT this->SigDataChanged(orc_Index, orc_Value.toString());
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Column to enum conversion

   \param[in]   ors32_Column          Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobDataSelectionTableModel::E_Columns C_SdNdeDalLogJobDataSelectionTableModel::h_ColumnToEnum(
   const int32_t & ors32_Column)
{
   C_SdNdeDalLogJobDataSelectionTableModel::E_Columns e_Retval = eDATA_ELEMENT;
   switch (ors32_Column)
   {
   //   case 0:
   //      e_Retval = eDATA_ELEMENT_ID;
   //      break;
   case 0:
      e_Retval = eDATA_ELEMENT;
      break;
   case 1:
      e_Retval = eLOCATION;
      break;
   case 2:
      e_Retval = eNAMESPACE;
      break;
   case 3:
      e_Retval = eLOGGING_NAME;
      break;
   case 4:
      e_Retval = eCOMMENT;
      break;
   default:
      // Unknown
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
int32_t C_SdNdeDalLogJobDataSelectionTableModel::h_EnumToColumn(
   const C_SdNdeDalLogJobDataSelectionTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   //   case eDATA_ELEMENT_ID:
   //      s32_Retval = 0;
   //      break;
   case eDATA_ELEMENT:
      s32_Retval = 0;
      break;
   case eLOCATION:
      s32_Retval = 1;
      break;
   case eNAMESPACE:
      s32_Retval = 2;
      break;
   case eLOGGING_NAME:
      s32_Retval = 3;
      break;
   case eCOMMENT:
      s32_Retval = 4;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}
