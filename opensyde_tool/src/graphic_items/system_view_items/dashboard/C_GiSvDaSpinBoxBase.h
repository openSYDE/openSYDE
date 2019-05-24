//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard spin box item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDASPINBOXBASE_H
#define C_GISVDASPINBOXBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiDashboardSpinBoxGroup.h"
#include "C_GiSvDaRectBaseGroup.h"

#include "C_PuiSvDbSpinBox.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaSpinBoxBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaSpinBoxBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                       const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                       QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaSpinBoxBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual void SendCurrentValue(void);
   virtual bool CallProperties(void) override;

   void UpdateTypePe(const stw_opensyde_gui_logic::C_PuiSvDbSpinBox::E_Type oe_Type, const bool oq_ShowUnit);
   void SetValuePe(const stw_types::sint32 os32_Value);
   void SetUnitPe(const QString & orc_Value);

private:
   bool mq_Connected;
   stw_opensyde_gui_elements::C_OgeWiDashboardSpinBoxGroup * mpc_SpinBoxWidget;

   //Avoid call
   C_GiSvDaSpinBoxBase(const C_GiSvDaSpinBoxBase &);
   C_GiSvDaSpinBoxBase & operator =(const C_GiSvDaSpinBoxBase &);

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
