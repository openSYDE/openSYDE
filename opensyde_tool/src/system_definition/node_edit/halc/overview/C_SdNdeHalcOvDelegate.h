//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview drawing delegate
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVDELEGATE_H
#define C_SDNDEHALCOVDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeHalcOvDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const;
   //lint -restore
   bool SetHoveredRow(const stw_types::sint32 & ors32_Value);
   void SetModel(const QAbstractTableModel * const opc_Value);

private:
   const QAbstractTableModel * mpc_Model;
   const QPixmap mc_CheckMark;
   stw_types::sint32 ms32_HoveredRow;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
