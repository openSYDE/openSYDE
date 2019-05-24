//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with tool tip functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETOBTOOLTIPBASE_H
#define C_OGETOBTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QToolButton>

#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTobToolTipBase :
   public QToolButton,
   public C_OgeToolTipBase
{
   Q_OBJECT

public:
   C_OgeTobToolTipBase(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
