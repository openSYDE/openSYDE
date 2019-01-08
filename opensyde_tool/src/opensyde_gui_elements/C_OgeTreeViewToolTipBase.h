//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETREEVIEWTOOLTIPBASE_H
#define C_OGETREEVIEWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QTreeView>
#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTreeViewToolTipBase :
   public QTreeView,
   public C_OgeToolTipBase
{
public:
   C_OgeTreeViewToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;
   //lint -restore

private:
   stw_types::sint32 ms32_HoveredNumberOfParents;
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;
   stw_types::sint32 ms32_HoveredHorzHeader;

   void m_HideTooltip(void);
   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
   static stw_types::sint32 mh_CountParents(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
