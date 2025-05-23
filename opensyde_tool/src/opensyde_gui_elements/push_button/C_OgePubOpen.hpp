//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard open button (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBOPEN_HPP
#define C_OGEPUBOPEN_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubOpen :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubOpen(QWidget * const opc_Parent = NULL);

   void SetBackgroundColor(const int32_t os32_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
