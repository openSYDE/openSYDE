//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver Datapool edit entry
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXDATAPOOLENTRY_HPP
#define C_SDBUEMESSAGERXDATAPOOLENTRY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxDatapoolEntry;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageRxDatapoolEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxDatapoolEntry(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxDatapoolEntry(void) override;

private:
   Ui::C_SdBueMessageRxDatapoolEntry * mpc_Ui;

   //Avoid call
   C_SdBueMessageRxDatapoolEntry(const C_SdBueMessageRxDatapoolEntry &);
   C_SdBueMessageRxDatapoolEntry & operator =(const C_SdBueMessageRxDatapoolEntry &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
