//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for HALC channel tree.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELTREEDELEGATE_H
#define C_SDNDEHALCCHANNELTREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const;
   //lint -restore

   void SetCurrentIndexParentRow(const stw_types::sintn osn_ParentRow);

private:
   stw_types::sintn msn_CurrentIndexParentRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
