//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for toolbar no search result (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTOOLBARNOSEARCHRESULT_HPP
#define C_OGELABTOOLBARNOSEARCHRESULT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabToolBarNoSearchResult :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabToolBarNoSearchResult(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
