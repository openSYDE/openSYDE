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
#include "C_TblTreSimpleItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreSimpleModel :
   public QAbstractItemModel
{
public:
   C_TblTreSimpleModel(QObject * const opc_Parent = NULL);

   QModelIndex index(const int32_t os32_Row, const int32_t os32_Column,
                     const QModelIndex & orc_Parent = QModelIndex()) const override;
   QModelIndex parent(const QModelIndex & orc_Index) const override; //lint !e1511  False positive
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

protected:
   C_TblTreSimpleItem * mpc_InvisibleRootItem;

   int32_t m_CountLayers(const QModelIndex & orc_Index, uint32_t & oru32_ValidLayers) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
