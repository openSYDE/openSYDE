//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLVIEWTOOLTIPBASE_HPP
#define C_TBLVIEWTOOLTIPBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTableView>
#include "stwtypes.hpp"
#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblViewToolTipBase :
   public QTableView
{
public:
   C_TblViewToolTipBase(QWidget * const opc_Parent = NULL);
   ~C_TblViewToolTipBase(void) override;

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   virtual void m_HideToolTip(void);

   std::vector<int32_t> m_GetColumnWidths(void) const;
   bool m_SetColumnWidths(const std::vector<int32_t> & orc_ColumnWidths);

private:
   C_NagToolTip * mpc_ToolTip;
   int32_t ms32_HoveredRow;
   int32_t ms32_HoveredCol;
   int32_t ms32_HoveredVertHeader;
   int32_t ms32_HoveredHorzHeader;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
