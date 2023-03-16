//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with tool tip and custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXDOUBLETOOLTIPBASE_H
#define C_OGESPXDOUBLETOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OgeSpxAllBase.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_OgeSpxDoubleDynamicDecimalsBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxDoubleToolTipBase :
   public C_OgeSpxDoubleDynamicDecimalsBase,
   public C_OgeSpxAllBase
{
   Q_OBJECT

public:
   C_OgeSpxDoubleToolTipBase(QWidget * const opc_Parent = NULL);

   bool event(QEvent * const opc_Event) override;

   void SetMinimumCustom(const float64_t of64_Value, const QString & orc_Display = "");
   void SetMaximumCustom(const float64_t of64_Value, const QString & orc_Display = "");

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
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
