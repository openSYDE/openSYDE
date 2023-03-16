//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main button in material design of navigation bar

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBUSECASENAV_H
#define C_OGEPUBUSECASENAV_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"
#include "C_OgePubStandard.hpp"
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubUseCaseNav :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubUseCaseNav(QWidget * const opc_Parent = NULL);

   bool q_LastButton;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
