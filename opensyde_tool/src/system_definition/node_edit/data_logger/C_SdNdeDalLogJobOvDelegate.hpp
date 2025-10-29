//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Log Jobs overview drawing delegate
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBOVDELEGATE_H
#define C_SDNDEDALLOGJOBOVDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QStyledItemDelegate>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobOvDelegate :
   public QStyledItemDelegate
{
public:
   explicit C_SdNdeDalLogJobOvDelegate(QObject * const opc_Parent = NULL);
   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;
   void SetModel(const QAbstractTableModel * const opc_Value);
   bool SetHoveredRow(const int32_t & ors32_Value);

private:
   const QAbstractTableModel * mpc_Model;
   int32_t ms32_HoveredRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
