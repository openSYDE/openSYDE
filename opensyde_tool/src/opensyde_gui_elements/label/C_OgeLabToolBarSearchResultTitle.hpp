//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbar search result title (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOOLBARSEARCHRESULTTITLE_H
#define C_OGELABTOOLBARSEARCHRESULTTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabToolBarSearchResultTitle :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabToolBarSearchResultTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
