//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line Edit for IP Addresses

   Line Edit for IP Addresses.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeLeIpAddress.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeIpAddress::C_OgeLeIpAddress(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event slot for one IpAddress line edit part

   To show empty string instead of "  0.  0.  0.  0".

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::focusInEvent(QFocusEvent * const opc_Event)
{
   this->setText(this->text().trimmed());
   C_OgeLeToolTipBase::focusInEvent(opc_Event);
   Q_EMIT SignalFocusIn(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   To replace empty parts with "0".

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::focusOutEvent(QFocusEvent * const opc_Event)
{
   this->setText(this->text().trimmed());

   C_OgeLeToolTipBase::focusOutEvent(opc_Event);

   if (opc_Event->reason() == Qt::TabFocusReason)
   {
      Q_EMIT (SignalTabKey(this));
   }

   Q_EMIT (SignalFocusOut(this));
}
