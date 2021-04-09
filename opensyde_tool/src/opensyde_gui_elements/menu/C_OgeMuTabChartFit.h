//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Menu for tab chart fit selections
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEMUTABCHARTFIT_H
#define C_OGEMUTABCHARTFIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMenu>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeMuTabChartFit :
   public QMenu
{
   Q_OBJECT

public:
   explicit C_OgeMuTabChartFit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
