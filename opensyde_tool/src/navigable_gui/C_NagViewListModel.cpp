//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       View item list model (implementation)

   View item list model

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QMimeData>
#include "C_NagViewListModel.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagViewListModel::C_NagViewListModel(QObject * const opc_Parent) :
   QAbstractListModel(opc_Parent),
   msn_Rows(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new number of rows

   \param[in] osn_Rows Set new number of rows

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewListModel::SetNumRows(const sintn osn_Rows)
{
   if ((this->msn_Rows != osn_Rows) && (osn_Rows >= 0))
   {
      bool q_Insert;

      if (this->msn_Rows <= osn_Rows)
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
         //First new index msn_Rows, last new index: osn_Rows - 1
         this->beginInsertRows(QModelIndex(), this->msn_Rows, osn_Rows - 1);
      }
      else
      {
         //First removed index osn_Rows, last removed index: msn_Rows - 1
         this->beginRemoveRows(QModelIndex(), osn_Rows, this->msn_Rows - 1);
      }
      this->msn_Rows = osn_Rows;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get table row count

   \param[in] orc_Parent Parent

   \return
   Row count

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_NagViewListModel::rowCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      sn_Retval = this->msn_Rows;
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_NagViewListModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   //No content
   Q_UNUSED(orc_Index)
   Q_UNUSED(osn_Role)
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get flags for item

   \param[in] orc_Index Item

   \return
   Flags for item

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
Qt::ItemFlags C_NagViewListModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = QAbstractListModel::flags(orc_Index);
   if (orc_Index.isValid() == true)
   {
      c_Retval = c_Retval | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Mime types

   \return
   Table mime types

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QStringList C_NagViewListModel::mimeTypes(void) const
{
   //Append original types (Default events only accept this type)
   QStringList c_Retval;

   //Append own types last (Default events only accept Qt types at start)
   c_Retval.append("application/stw/opensyde/navigation/view");

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overloaded function for stored mime data

   \param[in] orc_Items Current selected items

   \return
   Mime data for drag event

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
