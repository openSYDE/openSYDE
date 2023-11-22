//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model base class (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREMODEL_HPP
#define C_TBLTREMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreSimpleModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreModel :
   public C_TblTreSimpleModel
{
public:
   C_TblTreModel(QObject * const opc_Parent = NULL);

   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
