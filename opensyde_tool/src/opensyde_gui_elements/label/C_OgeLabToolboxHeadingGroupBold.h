//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bold heading for group box (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOOLBOXHEADINGGROUPBOLD_H
#define C_OGELABTOOLBOXHEADINGGROUPBOLD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabToolboxHeadingGroupBold :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabToolboxHeadingGroupBold(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
