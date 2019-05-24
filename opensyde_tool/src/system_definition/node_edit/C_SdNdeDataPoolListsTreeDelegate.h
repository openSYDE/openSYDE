//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool lists tree drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTSTREEDELEGATE_H
#define C_SDNDEDATAPOOLLISTSTREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListsTreeDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDataPoolListsTreeDelegate(QObject * const opc_Parent = NULL);

   void SetDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
