//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for HALC channel tree.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELTREEDELEGATE_HPP
#define C_SDNDEHALCCHANNELTREEDELEGATE_HPP

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

class C_SdNdeHalcChannelTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeDelegate(QObject * const opc_Parent = NULL);

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;

   void SetCurrentIndexParentRow(const int32_t os32_ParentRow);

private:
   int32_t ms32_CurrentIndexParentRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
