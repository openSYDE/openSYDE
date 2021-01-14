//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with tool tip functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIWITHTOOLTIP_H
#define C_OGEWIWITHTOOLTIP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiWithToolTip :
   public QWidget,
   public C_OgeToolTipBase
{
   Q_OBJECT

public:
   C_OgeWiWithToolTip(QWidget * const opc_Parent = NULL);

   void HideToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   virtual bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
