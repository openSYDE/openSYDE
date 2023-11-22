//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for navigation (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELENAVIGATION_HPP
#define C_OGELENAVIGATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeNavigation :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLeNavigation(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigEscape(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
