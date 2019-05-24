//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELELISTHEADER_H
#define C_OGELELISTHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeListHeader :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLeListHeader(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigFocus(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   bool mq_JustGotFocus;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
