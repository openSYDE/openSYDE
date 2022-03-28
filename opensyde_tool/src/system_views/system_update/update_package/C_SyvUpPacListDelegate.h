//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package list drawing delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTDELEGATE_H
#define C_SYVUPPACLISTDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "C_SyvUpPacNodeWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvUpPacListDelegate(QObject * const opc_Parent = NULL);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;

   void StartPaint(const stw_types::sint32 os32_Index, stw_opensyde_gui::C_SyvUpPacNodeWidget * const opc_Widget);
   void StopPaint(void);

private:
   //Avoid call
   C_SyvUpPacListDelegate(const C_SyvUpPacListDelegate &);
   C_SyvUpPacListDelegate & operator =(const C_SyvUpPacListDelegate &);

   stw_opensyde_gui::C_SyvUpPacNodeWidget * mpc_ActualWidget;
   stw_types::sint32 ms32_IndexPaint;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
