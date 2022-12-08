//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGELEFILEPATH_H
#define C_CAMOGELEFILEPATH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeFilePathBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeLeFilePath :
   public C_OgeLeFilePathBase
{
   Q_OBJECT

public:
   explicit C_CamOgeLeFilePath(QWidget * const opc_Parent = NULL);

protected:
   QString m_ResolveVariables(const QString & orc_Path) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
