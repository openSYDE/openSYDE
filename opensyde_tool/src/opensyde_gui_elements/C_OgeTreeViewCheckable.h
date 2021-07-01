//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view with checkable items (for style only)

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEVIEWCHECKABLE_H
#define C_OGETREEVIEWCHECKABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include <QModelIndex>
#include <QVector>

#include "C_OgeTreeViewToolTipBaseCheckable.h"
#include "C_SdCodeGenerationModel.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeViewCheckable :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBaseCheckable
{
   Q_OBJECT

public:
   C_OgeTreeViewCheckable(QWidget * const opc_Parent = NULL);
   ~C_OgeTreeViewCheckable(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
