//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBTOPOLOGYBASECONTEXTMENUMANAGER_H
#define C_SEBTOPOLOGYBASECONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebBaseContextMenuManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebTopologyBaseContextMenuManager :
   public C_SebBaseContextMenuManager
{
public:
   C_SebTopologyBaseContextMenuManager(void);
   ~C_SebTopologyBaseContextMenuManager(void) override;

protected:
   bool m_ActivateSpecificActions(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
