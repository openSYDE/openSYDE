//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list array edit drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAARDELEGATE_H
#define C_SYVDAITPAARDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyledItemDelegate>
#include "C_SyvDaItPaArModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaArDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvDaItPaArDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const;
   //lint -restore
   void SetModel(const C_SyvDaItPaArModel * const opc_Value);

private:
   const C_SyvDaItPaArModel * mpc_Model;

   QWidget * m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
