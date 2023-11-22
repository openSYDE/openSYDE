//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Scroll area for navigation bar (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESANAVIBAR_HPP
#define C_OGESANAVIBAR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollArea>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSaNaviBar :
   public QScrollArea
{
   Q_OBJECT

public:
   explicit C_OgeSaNaviBar(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
