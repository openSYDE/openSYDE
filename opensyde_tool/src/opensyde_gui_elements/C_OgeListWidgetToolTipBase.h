//-----------------------------------------------------------------------------
/*!
   \file
   \brief       List widget with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELISTWIDGETTOOLTIPBASE_H
#define C_OGELISTWIDGETTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QListWidget>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeListWidgetToolTipBase :
   public QListWidget,
   public C_OgeToolTipBase
{
public:
   C_OgeListWidgetToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;

   void m_HideTooltip(void);
   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
