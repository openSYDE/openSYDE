//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLVIEWTOOLTIPBASE_H
#define C_TBLVIEWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTableView>
#include "stwtypes.h"
#include "C_NagToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblViewToolTipBase :
   public QTableView
{
public:
   C_TblViewToolTipBase(QWidget * const opc_Parent = NULL);
   ~C_TblViewToolTipBase(void);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;

   virtual void m_HideToolTip(void);

   std::vector<stw_types::sint32> m_GetColumnWidths(void) const;
   bool m_SetColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);

private:
   C_NagToolTip * mpc_ToolTip;
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;
   stw_types::sint32 ms32_HoveredVertHeader;
   stw_types::sint32 ms32_HoveredHorzHeader;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
