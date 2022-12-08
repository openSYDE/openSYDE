//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for file paths (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEFILEPATH_H
#define C_OGELEFILEPATH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeFilePathBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLeFilePath :
   public C_OgeLeFilePathBase
{
   Q_OBJECT

public:
   explicit C_OgeLeFilePath(QWidget * const opc_Parent = NULL);

   void SetDbProjectPath(const QString & orc_DbPath);

protected:
   QString m_ResolveVariables(const QString & orc_Path) override;

private:
   QString mc_DbProjectPath;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
