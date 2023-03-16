//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbar search result subtitle (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOOLBARSEARCHRESULTSUBTITLE_H
#define C_OGELABTOOLBARSEARCHRESULTSUBTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabToolBarSearchResultSubtitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabToolBarSearchResultSubtitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
