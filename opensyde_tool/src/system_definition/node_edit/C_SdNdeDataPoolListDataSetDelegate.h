//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list data set drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTDATASETDELEGATE_H
#define C_SDNDEDATAPOOLLISTDATASETDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyledItemDelegate>
#include "C_SdNdeDataPoolListDataSetModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListDataSetDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListDataSetDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const;
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const;
   //lint -restore
   bool SetHoveredCol(const stw_types::sint32 & ors32_Value);
   void SetModel(const C_SdNdeDataPoolListDataSetModel * const opc_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStore(const QModelIndex & orc_Index) const;

private:
   const C_SdNdeDataPoolListDataSetModel * mpc_Model;
   stw_types::sint32 ms32_HoveredCol;
   QModelIndex mc_Edit;

   void m_OnNameChange(const QString & orc_Text) const;
   void m_Store(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
