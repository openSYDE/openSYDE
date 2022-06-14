//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the EDS file of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEEDSWIDGET_H
#define C_SDNDECODEVICEEDSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceEdsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoDeviceEdsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceEdsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoDeviceEdsWidget(void);

private:
   Ui::C_SdNdeCoDeviceEdsWidget * mpc_Ui;

   //Avoid call
   C_SdNdeCoDeviceEdsWidget(const C_SdNdeCoDeviceEdsWidget &);
   C_SdNdeCoDeviceEdsWidget & operator =(const C_SdNdeCoDeviceEdsWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
