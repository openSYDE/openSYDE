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

#include "C_OgePubToolTipBase.hpp"

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   void paintEvent(QPaintEvent * const opc_Event) override;

   const int32_t ms32_IconSize;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
