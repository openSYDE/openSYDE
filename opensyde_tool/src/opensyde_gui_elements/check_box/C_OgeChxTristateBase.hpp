//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom tristate check box base (Handle check box indicator resize) (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATEBASE_HPP
#define C_OGECHXTRISTATEBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxTristateBase :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxTristateBase(QWidget * const opc_Parent = NULL);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
