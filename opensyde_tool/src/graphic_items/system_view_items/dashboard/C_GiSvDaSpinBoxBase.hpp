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
#include "C_OgeWiDashboardSpinBoxGroup.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"

#include "C_PuiSvDbSpinBox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaSpinBoxBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaSpinBoxBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                       QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaSpinBoxBase(void) override;

   int32_t type(void) const override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void ConnectionActiveChanged(const bool oq_Active) override;
   void EditModeActiveChanged(const bool oq_Active) override;
   void SendCurrentValue(void) override;
   bool CallProperties(void) override;

   void UpdateTypePe(const stw::opensyde_gui_logic::C_PuiSvDbSpinBox::E_Type oe_Type, const bool oq_ShowUnit);
   void SetValuePe(const int32_t os32_Value);
   void SetUnitPe(const QString & orc_Value);

private:
   bool mq_Connected;
   stw::opensyde_gui_elements::C_OgeWiDashboardSpinBoxGroup * mpc_SpinBoxWidget;

   //Avoid call
   C_GiSvDaSpinBoxBase(const C_GiSvDaSpinBoxBase &);
   C_GiSvDaSpinBoxBase & operator =(const C_GiSvDaSpinBoxBase &) &; //lint !e1511 //we want to hide the base func.

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
