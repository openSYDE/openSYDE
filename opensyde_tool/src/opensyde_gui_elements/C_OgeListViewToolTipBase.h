//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List view base with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELISTVIEWTOOLTIPBASE_H
#define C_OGELISTVIEWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListView>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeListViewToolTipBase :
   public QListView,
   public C_OgeToolTipBase
{
public:
   C_OgeListViewToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;

   virtual void m_HideToolTip(void) override;

private:
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
