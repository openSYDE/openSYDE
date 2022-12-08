//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for displaying content of tooltips (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDTOOLTIPCONTENT_H
#define C_OGETEDTOOLTIPCONTENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTextEdit>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedToolTipContent :
   public QTextEdit
{
   Q_OBJECT

public:
   C_OgeTedToolTipContent(QWidget * const opc_Parent = NULL);

private:
   void m_AdjustMinimumSize();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
