//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABCONTEXTMENUBASE_H
#define C_OGELABCONTEXTMENUBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeContextMenu.hpp"
#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabContextMenuBase :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeLabContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_Copy() const;
   void m_SelectAll();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
