//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBSTANDARD_H
#define C_OGEPUBSTANDARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubStandard :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubStandard(QWidget * const opc_Parent = NULL);

protected:
   void m_SetPenColorForFont(QPainter * const opc_Painter) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
