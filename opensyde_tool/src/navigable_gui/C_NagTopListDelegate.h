//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for topology list (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPLISTDELEGATE_H
#define C_NAGTOPLISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopListDelegate :
   public QStyledItemDelegate
{
public:
   C_NagTopListDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
