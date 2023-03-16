//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for PieChart

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPIECHART_H
#define C_OGEWIPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_SclString.hpp"

#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_OgeWiPieChart;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeWiPieChart :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiPieChart(QWidget * const opc_Parent = NULL);
   ~C_OgeWiPieChart() override;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   void m_OnSpinboxValuechanged(const int32_t os32_Value);
   //Avoid call
   C_OgeWiPieChart(const C_OgeWiPieChart &);
   C_OgeWiPieChart & operator =(const C_OgeWiPieChart &) &;

   Ui::C_OgeWiPieChart * mpc_Ui;

   uint16_t mu16_ValueInPercent;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace
#endif
