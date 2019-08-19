//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple tree model base class
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTRESIMPLEMODEL_H
#define C_TBLTRESIMPLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractItemModel>
#include "C_TblTreSimpleItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreSimpleModel :
   public QAbstractItemModel
{
public:
   C_TblTreSimpleModel(QObject * const opc_Parent = NULL);

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

   //lint -restore

protected:
   C_TblTreSimpleItem * mpc_InvisibleRootItem;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
