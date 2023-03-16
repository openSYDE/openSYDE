//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for ProgressLog

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPROGRESSLOG_H
#define C_OGEWIPROGRESSLOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiProgressLog;
}

namespace stw
{
namespace opensyde_gui
{
class C_OgeWiProgressLog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiProgressLog(QWidget * const opc_Parent = NULL);
   ~C_OgeWiProgressLog() override;

private:
   //Avoid call
   C_OgeWiProgressLog(const C_OgeWiProgressLog &);
   C_OgeWiProgressLog & operator =(const C_OgeWiProgressLog &) &;

   Ui::C_OgeWiProgressLog * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace
#endif
