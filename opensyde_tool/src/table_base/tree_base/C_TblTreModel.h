//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model base class (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREMODEL_H
#define C_TBLTREMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractItemModel>
#include "C_TblTreItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreModel :
   public QAbstractItemModel
{
public:
   C_TblTreModel(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QModelIndex index(const stw_types::sintn osn_Row, const stw_types::sintn osn_Column,
                             const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QModelIndex parent(const QModelIndex & orc_Index) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

protected:
   C_TblTreItem * mpc_InvisibleRootItem;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
