//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom context menu (header)

   Custom context menu (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECONTEXTMENU_H
#define C_OGECONTEXTMENU_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QPaintEvent>
#include <QMenu>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeContextMenu :
   public QMenu
{
   Q_OBJECT

public:
   C_OgeContextMenu(QWidget * const opc_Parent = NULL);
   ~C_OgeContextMenu() override;

private:
   //Avoid call
   C_OgeContextMenu(const C_OgeContextMenu &);
   C_OgeContextMenu & operator =(const C_OgeContextMenu &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
