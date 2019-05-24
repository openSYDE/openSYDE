//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View (display) component for filter items table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERTABLEVIEW_H
#define C_CAMMOSFILTERTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewToolTipBase.h"
#include "C_TblDelegate.h"
#include "C_CamMosFilterTableModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosFilterTableView :
   public C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_CamMosFilterTableView(QWidget * const opc_Parent = NULL);

private:
   C_TblDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
