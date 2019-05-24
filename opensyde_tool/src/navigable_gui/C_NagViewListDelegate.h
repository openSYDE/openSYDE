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
namespace stw_opensyde_gui
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   QListView * mpc_View;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
