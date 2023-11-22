//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for search (header)

   Line edit field for search (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELESEARCH_HPP
#define C_OGELESEARCH_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeLeContextMenuBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeSearch :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLeSearch(QWidget * const opc_Parent = NULL);
   void SetDarkTheme(void);
   void SetLightTheme(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
