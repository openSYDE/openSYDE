//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEHEADERVIEW_H
#define C_SDNDEDATAPOOLLISTTABLEHEADERVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QHeaderView>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListTableHeaderView :
   public QHeaderView
{
public:
   C_SdNdeDataPoolListTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolListTableHeaderView(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QStyleOptionViewItem viewOptions(void) const override;
   virtual void paintSection(QPainter * const opc_Painter, const QRect & orc_Rect,
                             const stw_types::sintn osn_LogicalIndex) const override;
   virtual void enterEvent(QEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdNdeDataPoolListTableHeaderView(const C_SdNdeDataPoolListTableHeaderView &);
   C_SdNdeDataPoolListTableHeaderView & operator =(const C_SdNdeDataPoolListTableHeaderView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
