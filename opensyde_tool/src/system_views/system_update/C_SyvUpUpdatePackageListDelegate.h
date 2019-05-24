//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package list drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTDELEGATE_H
#define C_SYVUPUPDATEPACKAGELISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "C_SyvUpUpdatePackageNodeWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdatePackageListDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvUpUpdatePackageListDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore

   void StartPaint(const stw_types::sint32 s32_Index,
                   stw_opensyde_gui::C_SyvUpUpdatePackageNodeWidget * const opc_Widget);
   void StopPaint(void);

private:
   //Avoid call
   C_SyvUpUpdatePackageListDelegate(const C_SyvUpUpdatePackageListDelegate &);
   C_SyvUpUpdatePackageListDelegate & operator =(const C_SyvUpUpdatePackageListDelegate &);

   stw_opensyde_gui::C_SyvUpUpdatePackageNodeWidget * mpc_ActualWidget;
   stw_types::sint32 ms32_IndexPaint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
