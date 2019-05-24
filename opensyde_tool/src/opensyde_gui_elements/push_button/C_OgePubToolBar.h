//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders and with icon

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBTOOLBAR_H
#define C_OGEPUBTOOLBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.h"
#include "C_OgePubStandard.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubToolBar :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubToolBar(QWidget * const opc_Parent = NULL, const stw_types::uint32 ou32_Index = 0UL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigClickedWithIndex(const stw_types::uint32 ou32_Index);

private:
   void m_ButtonClicked(void);

   const stw_types::uint32 mu32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
