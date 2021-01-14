//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDESFORESETMESSAGETABLEDELEGATE_H
#define C_SDNDESFORESETMESSAGETABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeSfoResetMessageTableDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeSfoResetMessageTableDelegate(QObject * const opc_Parent = NULL);

   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const override;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
