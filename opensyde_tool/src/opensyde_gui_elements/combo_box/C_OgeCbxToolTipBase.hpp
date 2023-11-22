//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTOOLTIPBASE_HPP
#define C_OGECBXTOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QComboBox>
#include "stwtypes.hpp"
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxToolTipBase :
   public QComboBox,
   public C_OgeToolTipBase
{
public:
   C_OgeCbxToolTipBase(QWidget * const opc_Parent = NULL);

   bool event(QEvent * const opc_Event) override;

   void SetItemState(const int32_t os32_Index, const bool oq_Status) const;
   void SetItemVisible(const int32_t os32_Index, const bool oq_Visible) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
