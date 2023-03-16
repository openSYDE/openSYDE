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
#include "stwtypes.hpp"

#include <QModelIndex>
#include <QVector>

#include "C_OgeTreeViewToolTipBaseCheckable.hpp"
#include "C_SdCodeGenerationModel.hpp"
#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeViewCheckable :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBaseCheckable
{
   Q_OBJECT

public:
   C_OgeTreeViewCheckable(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeTreeViewCheckable(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
