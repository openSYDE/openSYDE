//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View item list model (implementation)

   View item list model

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMimeData>
#include "C_NagViewListModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_NagViewListModel::C_NagViewListModel(QObject * const opc_Parent) :
   QAbstractListModel(opc_Parent),
   ms32_Rows(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new number of rows

   \param[in]  os32_Rows    Set new number of rows
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewListModel::SetNumRows(const int32_t os32_Rows)
{
   if ((this->ms32_Rows != os32_Rows) && (os32_Rows >= 0))
   {
      bool q_Insert;

      if (this->ms32_Rows <= os32_Rows)
      {
         q_Insert = true;
      }
      else
      {
         q_Insert = false;
      }
      //For performance
      if (q_Insert == true)
      {
         //First new index ms32_Rows, last new index: os32_Rows - 1
         this->beginInsertRows(QModelIndex(), this->ms32_Rows, os32_Rows - 1);
      }
      else
      {
         //First removed index os32_Rows, last removed index: ms32_Rows - 1
         this->beginRemoveRows(QModelIndex(), os32_Rows, this->ms32_Rows - 1);
      }
      this->ms32_Rows = os32_Rows;
      //For performance
      if (q_Insert == true)
      {
         this->endInsertRows();
      }
      else
      {
         this->endRemoveRows();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_NagViewListModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->ms32_Rows;
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
QVariant C_NagViewListModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   //No content
   Q_UNUSED(orc_Index)
   Q_UNUSED(os32_Role)
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_NagViewListModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = QAbstractListModel::flags(orc_Index);
   if (orc_Index.isValid() == true)
   {
      c_Retval = c_Retval | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mime types

   \return
   Table mime types
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_NagViewListModel::mimeTypes(void) const
{
   //Append original types (Default events only accept this type)
   QStringList c_Retval;

   //Append own types last (Default events only accept Qt types at start)
   c_Retval.append("application/stw/opensyde/navigation/view");

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded function for stored mime data

   \param[in]  orc_Indices    Currently selected indices

   \return
   Mime data for drag event
*/
//----------------------------------------------------------------------------------------------------------------------
QMimeData * C_NagViewListModel::mimeData(const QModelIndexList & orc_Indices) const
{
   QMimeData * pc_Retval = NULL;

   if (orc_Indices.size() > 0)
   {
      Q_UNUSED(orc_Indices)
      pc_Retval = new QMimeData();
      pc_Retval->setData(this->mimeTypes().at(0), QString::number(orc_Indices.at(0).row()).toStdString().c_str());
   }
   return pc_Retval;
}
