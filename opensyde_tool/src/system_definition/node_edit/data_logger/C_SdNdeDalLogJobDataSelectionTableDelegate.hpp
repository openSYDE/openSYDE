//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table delegate for data selection widget
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBDATASELECTIONTABLEDELEGATE_H
#define C_SDNDEDALLOGJOBDATASELECTIONTABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobDataSelectionTableDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDalLogJobDataSelectionTableDelegate(QObject * const opc_Parent = NULL);
   void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                     const QModelIndex & orc_Index) const override;

   QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
