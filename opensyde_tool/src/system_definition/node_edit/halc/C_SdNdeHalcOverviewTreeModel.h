//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channel overview.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVERVIEWTREEMODEL_H
#define C_SDNDEHALCOVERVIEWTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOverviewTreeModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   C_SdNdeHalcOverviewTreeModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcOverviewTreeModel(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -restore

private:
   C_SdNdeHalcOverviewTreeModel(const C_SdNdeHalcOverviewTreeModel &);
   C_SdNdeHalcOverviewTreeModel & operator =(const C_SdNdeHalcOverviewTreeModel &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
