//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Transparent push button with hover state (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBNAVIGATIONHOVER_HPP
#define C_OGEPUBNAVIGATIONHOVER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubNavigationHover :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubNavigationHover(QWidget * const opc_Parent = NULL);

   void SetActive(const bool oq_Active);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
