//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTARRAYEDITDELEGATE_H
#define C_SDNDEDPLISTARRAYEDITDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyledItemDelegate>
#include "C_SdNdeDpListArrayEditModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListArrayEditDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDpListArrayEditDelegate(QObject * const opc_Parent = NULL);

   QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const override;
   void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                     const QModelIndex & orc_Index) const override;

   void SetModel(const C_SdNdeDpListArrayEditModel * const opc_Value);

private:
   const C_SdNdeDpListArrayEditModel * mpc_Model;

   QWidget * m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
