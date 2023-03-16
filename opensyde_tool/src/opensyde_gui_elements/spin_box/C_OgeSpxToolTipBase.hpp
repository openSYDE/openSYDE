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
#include "stwtypes.hpp"
#include "C_OgeSpxAllBase.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxToolTipBase :
   public QSpinBox,
   public C_OgeSpxAllBase
{
public:
   C_OgeSpxToolTipBase(QWidget * const opc_Parent = NULL);

   void SetMinimumCustom(const int32_t os32_Value, const bool oq_ShowSpecial = false,
                         const int32_t os32_SpecialValue = 0);
   void SetMaximumCustom(const int32_t os32_Value, const bool oq_ShowSpecial = false,
                         const int32_t os32_SpecialValue = 0);
   // The access have to be changed because in base class it's protected but we need a public one
   //lint -save -e1768
   QString textFromValue(const int32_t os32_Value) const override;
   //lint -restore

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

   QString m_ConvertNumToString(const int32_t os32_Value) const;

   bool mq_ShowSpecialMin;
   int32_t ms32_SpecialMinValue;
   bool mq_ShowSpecialMax;
   int32_t ms32_SpecialMaxValue;

protected:
   QString m_GetMinimumString(void) const override;
   QString m_GetMaximumString(void) const override;

   bool event(QEvent * const opc_Event) override;

   bool m_IsEnabled(void) const override;
   QString m_GetMinimumRawString(void) const override;
   QString m_GetMaximumRawString(void) const override;
   void m_SetMouseTracking(const bool oq_Active) override;
   bool m_CallBaseEvent(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
