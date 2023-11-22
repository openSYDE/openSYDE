//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit with custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELECONTEXTMENUBASE_HPP
#define C_OGELECONTEXTMENUBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLineEdit>
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeContextMenuBase :
   public QLineEdit
{
   Q_OBJECT

public:
   C_OgeLeContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
