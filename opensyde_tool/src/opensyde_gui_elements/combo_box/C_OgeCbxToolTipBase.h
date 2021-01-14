//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTOOLTIPBASE_H
#define C_OGECBXTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QComboBox>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxToolTipBase :
   public QComboBox,
   public C_OgeToolTipBase
{
public:
   C_OgeCbxToolTipBase(QWidget * const opc_Parent = NULL);

   virtual bool event(QEvent * const opc_Event) override;

   void SetItemState(const stw_types::sint32 os32_Index, const bool oq_Status) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
