//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box with tool tip and custom context menu (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXINT64TOOLTIPBASE_H
#define C_OGESPXINT64TOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpxInt64.h"
#include "C_OgeSpxAllBase.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxInt64ToolTipBase :
   public C_OgeSpxInt64,
   public C_OgeSpxAllBase
{
public:
   C_OgeSpxInt64ToolTipBase(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

protected:
   virtual bool m_IsEnabled(void) const override;
   virtual QString m_GetMinimum(void) const override;
   virtual QString m_GetMaximum(void) const override;
   virtual void m_SetMouseTracking(const bool oq_Active) override;
   virtual bool m_CallBaseEvent(QEvent * const opc_Event) override;

private:
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   void m_Undo() const;
   void m_Redo() const;
   void m_Cut() const;
   void m_Copy() const;
   void m_Paste() const;
   void m_Delete() const;

   void m_ActivateDefaultToolTip(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
