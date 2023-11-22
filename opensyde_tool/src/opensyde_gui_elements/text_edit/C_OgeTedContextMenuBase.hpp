//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit with custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDBASE_HPP
#define C_OGETEDBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTextEdit>
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedContextMenuBase :
   public QTextEdit
{
   Q_OBJECT

public:
   C_OgeTedContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_Delete() const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
