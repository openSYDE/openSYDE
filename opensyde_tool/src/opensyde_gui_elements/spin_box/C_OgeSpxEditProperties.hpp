//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box field for editing properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXEDITPROPERTIES_H
#define C_OGESPXEDITPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeSpxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxEditProperties :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxEditProperties(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigFocus(void);

protected:
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   bool mq_JustGotFocus;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
