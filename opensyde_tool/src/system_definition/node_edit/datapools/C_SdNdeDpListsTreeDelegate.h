//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool lists tree drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTSTREEDELEGATE_H
#define C_SDNDEDPLISTSTREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListsTreeDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDpListsTreeDelegate(QObject * const opc_Parent = NULL);

   void SetDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);

   virtual QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;

   void SetMaximumHeight(const stw_types::sintn sn_Height);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::sintn msn_MaximumHeight;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
