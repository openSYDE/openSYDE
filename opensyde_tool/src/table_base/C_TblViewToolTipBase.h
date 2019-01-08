//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Table view with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_TBLVIEWTOOLTIPBASE_H
#define C_TBLVIEWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QTableView>
#include "stwtypes.h"
#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_TblViewToolTipBase :
   public QTableView
{
public:
   C_TblViewToolTipBase(QWidget * const opc_Parent = NULL);
   ~C_TblViewToolTipBase(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   C_NagToolTip * mpc_ToolTip;
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;
   stw_types::sint32 ms32_HoveredVertHeader;
   stw_types::sint32 ms32_HoveredHorzHeader;

   void m_HideTooltip(void);
   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
