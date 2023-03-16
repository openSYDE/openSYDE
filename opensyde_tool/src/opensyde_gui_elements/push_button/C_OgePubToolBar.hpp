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

#include "precomp_headers.hpp"
#include "C_OgePubStandard.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubToolBar :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubToolBar(QWidget * const opc_Parent = NULL, const uint32_t ou32_Index = 0UL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigClickedWithIndex(const uint32_t ou32_Index);

private:
   void m_ButtonClicked(void);

   const uint32_t mu32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
