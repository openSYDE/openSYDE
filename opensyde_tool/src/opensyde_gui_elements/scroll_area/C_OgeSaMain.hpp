//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Scroll area for main widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESAMAIN_HPP
#define C_OGESAMAIN_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollArea>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSaMain :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeSaMain(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
