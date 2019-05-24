//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List item to display project (header)

   List item to display project (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPLISTITEM_H
#define C_POPLISTITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyleOptionViewItem>
#include <QObject>
#include <QModelIndex>
#include <QSize>
#include <QPainter>
#include <QItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopListItem :
   public QItemDelegate
{
public:
   C_PopListItem(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
