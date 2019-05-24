//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders and with icon as a root item for a tree widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTREEWIDGETROOT_H
#define C_OGEPUBTREEWIDGETROOT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgePubToolTipBase.h"

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubTreeWidgetRoot :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubTreeWidgetRoot(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

   const stw_types::sintn msn_IconSize;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
