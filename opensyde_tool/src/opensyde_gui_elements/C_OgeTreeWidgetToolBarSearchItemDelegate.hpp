//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Toolbar search item delegate (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLBARSEARCHITEMDELEGATE_HPP
#define C_OGETREEWIDGETTOOLBARSEARCHITEMDELEGATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyledItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeWidgetToolBarSearchItemDelegate :
   public QStyledItemDelegate
{
public:
   C_OgeTreeWidgetToolBarSearchItemDelegate(QObject * const opc_Parent = NULL);

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;
   QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
