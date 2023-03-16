//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for view list (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGVIEWLISTDELEGATE_H
#define C_NAGVIEWLISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListView>
#include <QStyledItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewListDelegate :
   public QStyledItemDelegate
{
public:
   C_NagViewListDelegate(QObject * const opc_Parent = NULL);

   void SetListView(QListView * const opc_View);
   void RegisterExpand(const QModelIndex & orc_Index);

   QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;

private:
   QListView * mpc_View;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
