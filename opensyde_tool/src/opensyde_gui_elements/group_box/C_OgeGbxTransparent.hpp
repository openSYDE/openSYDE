//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box transparent (header)

   Custom group box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTRANSPARENT_HPP
#define C_OGEGBXTRANSPARENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeGbxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxTransparent :
   public C_OgeGbxToolTipBase
{
   Q_OBJECT

public:
   C_OgeGbxTransparent(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
