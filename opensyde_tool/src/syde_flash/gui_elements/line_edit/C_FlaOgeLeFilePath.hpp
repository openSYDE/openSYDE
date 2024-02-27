//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of file path line edit widget
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAOGELEFILEPATH_HPP
#define C_FLAOGELEFILEPATH_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeFilePathBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaOgeLeFilePath :
   public C_OgeLeFilePathBase
{
   Q_OBJECT

public:
   C_FlaOgeLeFilePath(QWidget * const opc_Parent = NULL);

protected:
   QString m_ResolveVariables(const QString & orc_Path) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
