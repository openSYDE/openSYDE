//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Default spin box with tool tip and custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXTOOLTIPBASE_H
#define C_OGESPXTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSpinBox>
#include "stwtypes.h"
#include "C_OgeSpxAllBase.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxToolTipBase :
   public QSpinBox,
   public C_OgeSpxAllBase
{
public:
   C_OgeSpxToolTipBase(QWidget * const opc_Parent = NULL);

   void SetMinimumCustom(const stw_types::sintn osn_Value, const bool oq_ShowSpecial = false,
                         const stw_types::sintn osn_SpecialValue = 0);
   void SetMaximumCustom(const stw_types::sintn osn_Value, const bool oq_ShowSpecial = false,
                         const stw_types::sintn osn_SpecialValue = 0);

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

   QString m_ConvertNumToString(const stw_types::sintn osn_Value) const;

   bool mq_ShowSpecialMin;
   stw_types::sintn msn_SpecialMinValue;
   bool mq_ShowSpecialMax;
   stw_types::sintn msn_SpecialMaxValue;

protected:
   virtual QString m_GetMinimumString(void) const;
   virtual QString m_GetMaximumString(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

   virtual bool m_IsEnabled(void) const override;
   virtual QString m_GetMinimumRawString(void) const override;
   virtual QString m_GetMaximumRawString(void) const override;
   virtual void m_SetMouseTracking(const bool oq_Active) override;
   virtual bool m_CallBaseEvent(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
