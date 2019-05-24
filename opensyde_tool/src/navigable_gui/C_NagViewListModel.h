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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewListModel :
   public QAbstractListModel
{
public:
   C_NagViewListModel(QObject * const opc_Parent = NULL);

   void SetNumRows(const stw_types::sintn osn_Rows);
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   //Drag & drop
   virtual QStringList mimeTypes(void) const override;
   virtual QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;
   //lint -restore

private:
   stw_types::sintn msn_Rows;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
