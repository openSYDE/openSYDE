//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button for dashboard chart control buttons with background styles in dark and bright.
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBICONCHART_H
#define C_OGEPUBICONCHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconChart :
   public C_OgePubSvgIconWithTextBase // we do not need text but possibly space for little arrow indicating menu
{
   Q_OBJECT

public:
   explicit C_OgePubIconChart(QWidget * const opc_Parent = NULL);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
