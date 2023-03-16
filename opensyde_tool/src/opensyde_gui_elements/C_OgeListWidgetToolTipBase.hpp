//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELISTWIDGETTOOLTIPBASE_H
#define C_OGELISTWIDGETTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListWidget>
#include "stwtypes.hpp"
#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeListWidgetToolTipBase :
   public QListWidget,
   public C_OgeToolTipBase
{
public:
   C_OgeListWidgetToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   void m_HideToolTip(void) override;

private:
   int32_t ms32_HoveredRow;
   int32_t ms32_HoveredCol;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
