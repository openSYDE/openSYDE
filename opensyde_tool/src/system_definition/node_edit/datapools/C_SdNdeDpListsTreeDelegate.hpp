//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool lists tree drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTSTREEDELEGATE_HPP
#define C_SDNDEDPLISTSTREEDELEGATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListsTreeDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDpListsTreeDelegate(QObject * const opc_Parent = NULL);

   void SetDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);

   QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;

   void SetMaximumHeight(const int32_t os32_Height);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   int32_t ms32_MaximumHeight;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
