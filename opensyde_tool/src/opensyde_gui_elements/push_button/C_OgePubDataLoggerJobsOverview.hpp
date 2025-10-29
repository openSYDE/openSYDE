//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overview button in DataLogger Jobs Screen.
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBDATALOGGERJOBSOVERVIEW_H
#define C_OGEPUBDATALOGGERJOBSOVERVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubDataLoggerJobsOverview :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubDataLoggerJobsOverview(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
