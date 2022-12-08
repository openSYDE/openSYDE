//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View item list model (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGVIEWLISTMODEL_H
#define C_NAGVIEWLISTMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractListModel>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewListModel :
   public QAbstractListModel
{
public:
   C_NagViewListModel(QObject * const opc_Parent = NULL);

   void SetNumRows(const int32_t os32_Rows);

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   //Drag & drop
   QStringList mimeTypes(void) const override;
   QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;

private:
   int32_t ms32_Rows;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
