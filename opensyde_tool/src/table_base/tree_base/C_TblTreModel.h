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
#include "C_TblTreSimpleModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreModel :
   public C_TblTreSimpleModel
{
public:
   C_TblTreModel(QObject * const opc_Parent = NULL);

   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
