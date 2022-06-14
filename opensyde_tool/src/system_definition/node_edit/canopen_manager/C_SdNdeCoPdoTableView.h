//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOTABLEVIEW_H
#define C_SDNDECOPDOTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewScroll.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeCoPdoTableView(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
