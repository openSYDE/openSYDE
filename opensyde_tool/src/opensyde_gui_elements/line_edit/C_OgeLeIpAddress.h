//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line Edit for IP Addresses
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEIPADDRESS_H
#define C_OGELEIPADDRESS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeIpAddress :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeLeIpAddress(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SignalFocusIn(const QLineEdit * const opc_LineEdit);
   void SignalFocusOut(const QLineEdit * const opc_LineEdit);
   void SignalTabKey(const QLineEdit * const opc_LineEdit);

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
