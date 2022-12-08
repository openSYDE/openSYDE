//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for Data Block tile comment.
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDDBCOMMENT_H
#define C_OGETEDDBCOMMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTextEdit>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedDbComment :
   public QTextEdit
{
   Q_OBJECT

public:
   explicit C_OgeTedDbComment(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
