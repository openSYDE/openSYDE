//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List model for log jobs

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>

#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDalLogJobsListModel.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *  \param[in]  opc_Parent      Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobsListModel::C_SdNdeDalLogJobsListModel(QObject * const opc_Parent) :
   QAbstractListModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update model data

   \param[in]       ou32_NodeIndex     Detailed input parameter description

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListModel::UpdateData(const uint32_t ou32_NodeIndex)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   const uint32_t u32_LogJobCount = pc_Node->c_DataLoggerJobs.size();

   this->mc_LogJobsList.clear();
   this->mc_LogJobsList.reserve(u32_LogJobCount);

   beginResetModel();

   for (uint32_t u32_Index = 0; u32_Index < u32_LogJobCount; u32_Index++)
   {
      C_DalLogJob c_LogJob;
      c_LogJob.c_Name = pc_Node->c_DataLoggerJobs.at(u32_Index).c_Properties.c_Name;
      c_LogJob.q_Enabled = pc_Node->c_DataLoggerJobs.at(u32_Index).q_IsEnabled;

      this->mc_LogJobsList.push_back(c_LogJob);
   }

   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove row at given index

   \param[in]       ou32_ElementIndex     The index of the element to be deleted

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobsListModel::DoRemoveRow(const uint32_t ou32_ElementIndex)
{
   beginRemoveRows(QModelIndex(), ou32_ElementIndex, ou32_ElementIndex);
   this->mc_LogJobsList.erase(this->mc_LogJobsList.begin() + ou32_ElementIndex);
   endRemoveRows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDalLogJobsListModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         if ((this->mc_LogJobsList.at(orc_Index.row()).q_Enabled) == true)
         {
            c_Retval = static_cast<int32_t>(Qt::Checked);
         }
         else
         {
            c_Retval = static_cast<int32_t>(Qt::Unchecked);
         }
      }

      else if (os32_Role == static_cast<int32_t>(Qt::DecorationRole))

      {
         const QIcon c_Icon = QIcon("://images/IconLogging.svg");
         c_Retval = c_Icon;
      }

      else if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         c_Retval = this->mc_LogJobsList.at(orc_Index.row()).c_Name.c_str();
      }
      else
      {
         //Unknown
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data at index
 *
   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  os32_Role   Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobsListModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                         const int32_t os32_Role)
{
   bool q_Retval = false;

   if ((orc_Index.isValid() == true))
   {
      if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         if (orc_Value == static_cast<int32_t>(Qt::Checked))
         {
            this->mc_LogJobsList.at(orc_Index.row()).q_Enabled = true;
         }
         else
         {
            this->mc_LogJobsList.at(orc_Index.row()).q_Enabled = false;
         }

         q_Retval = true;
         Q_EMIT this->dataChanged(orc_Index, orc_Index);
         Q_EMIT this->SigDataChanged(orc_Index, this->mc_LogJobsList.at(orc_Index.row()).q_Enabled);
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalLogJobsListModel::rowCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return this->mc_LogJobsList.size();
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
Qt::ItemFlags C_SdNdeDalLogJobsListModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = QAbstractListModel::flags(orc_Index);

   if (orc_Index.isValid())

   {
      c_Retval = c_Retval | Qt::ItemIsUserCheckable;
   }

   return c_Retval;
}
