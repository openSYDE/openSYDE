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
#include "stwtypes.h"
#include "C_OgeSpxAllBase.h"
#include "C_OgeContextMenu.h"
#include "C_OgeSpxDoubleDynamicDecimalsBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
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

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

   void SetMinimumCustom(const stw_types::float64 of64_Value);
   void SetMaximumCustom(const stw_types::float64 of64_Value);

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
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
