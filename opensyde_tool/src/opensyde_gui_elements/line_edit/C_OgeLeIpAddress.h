//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for editing IP addresses.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEIPADDRESS_H
#define C_OGELEIPADDRESS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QRegExpValidator>
#include "C_OgeLeProperties.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeIpAddress :
   public C_OgeLeProperties
{
   Q_OBJECT

public:
   explicit C_OgeLeIpAddress(QWidget * const opc_Parent = NULL);
   ~C_OgeLeIpAddress(void);

   void SetIPAddress(const stw_types::uint8 * const opu8_IPAddress);
   std::vector<stw_types::sint32> GetIPAddress(void) const;

protected:
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   QRegularExpressionValidator * mpc_Validator;

   //Avoid call
   C_OgeLeIpAddress(const C_OgeLeIpAddress &);
   C_OgeLeIpAddress & operator =(const C_OgeLeIpAddress &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
