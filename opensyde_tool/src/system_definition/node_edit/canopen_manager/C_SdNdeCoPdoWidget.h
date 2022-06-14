//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   Widget for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOWIDGET_H
#define C_SDNDECOPDOWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoPdoWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoPdoWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoPdoWidget(void);

private:
   Ui::C_SdNdeCoPdoWidget * mpc_Ui;

   //Avoid call
   C_SdNdeCoPdoWidget(const C_SdNdeCoPdoWidget &);
   C_SdNdeCoPdoWidget & operator =(const C_SdNdeCoPdoWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
