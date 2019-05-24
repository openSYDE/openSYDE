//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget for page navigation (header)

   Tab widget for page navigation (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETAWPAGENAVI_H
#define C_OGETAWPAGENAVI_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeTawToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTawPageNavi :
   public C_OgeTawToolTipBase
{
   Q_OBJECT

public:
   C_OgeTawPageNavi(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
