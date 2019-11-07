//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list drawing delegate (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORLISTDELEGATE_H
#define C_SDNDEDPSELECTORLISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "C_SdNdeDpSelectorItemWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorListDelegate :
   public QStyledItemDelegate
{
public:
   C_SdNdeDpSelectorListDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore

   void StartPaint(const stw_types::sint32 s32_Index,
                   stw_opensyde_gui::C_SdNdeDpSelectorItemWidget * const opc_Widget);
   void StopPaint(void);

private:
   //Avoid call
   C_SdNdeDpSelectorListDelegate(const C_SdNdeDpSelectorListDelegate &);
   C_SdNdeDpSelectorListDelegate & operator =(const C_SdNdeDpSelectorListDelegate &);

   stw_opensyde_gui::C_SdNdeDpSelectorItemWidget * mpc_ActualWidget;
   stw_types::sint32 ms32_IndexPaint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
