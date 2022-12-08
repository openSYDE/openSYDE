//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box with tool tip and custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXINT64TOOLTIPBASE_H
#define C_OGESPXINT64TOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpxInt64.hpp"
#include "C_OgeSpxAllBase.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxInt64ToolTipBase :
   public C_OgeSpxInt64,
   public C_OgeSpxAllBase
{
public:
   C_OgeSpxInt64ToolTipBase(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);

   bool event(QEvent * const opc_Event) override;

protected:
   bool m_IsEnabled(void) const override;
   QString m_GetMinimumRawString(void) const override;
   QString m_GetMaximumRawString(void) const override;
   void m_SetMouseTracking(const bool oq_Active) override;
   bool m_CallBaseEvent(QEvent * const opc_Event) override;

private:
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
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

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
