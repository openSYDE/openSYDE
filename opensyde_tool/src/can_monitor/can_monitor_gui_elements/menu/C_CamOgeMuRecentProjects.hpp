//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Menu for recent projects selection (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEMURECENTPROJECTS_H
#define C_CAMOGEMURECENTPROJECTS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMenu>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeMuRecentProjects :
   public QMenu
{
   Q_OBJECT

public:
   explicit C_CamOgeMuRecentProjects(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
