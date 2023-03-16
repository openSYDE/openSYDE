//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEHEADERVIEW_H
#define C_SDNDEDPLISTTABLEHEADERVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QHeaderView>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListTableHeaderView :
   public QHeaderView
{
public:
   C_SdNdeDpListTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListTableHeaderView(void) override;

protected:
   QStyleOptionViewItem viewOptions(void) const override;
   void paintSection(QPainter * const opc_Painter, const QRect & orc_Rect,
                     const int32_t os32_LogicalIndex) const override;
   void enterEvent(QEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeDpListTableHeaderView(const C_SdNdeDpListTableHeaderView &);
   C_SdNdeDpListTableHeaderView & operator =(const C_SdNdeDpListTableHeaderView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
