//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard pie chart item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAPIECHARTBASE_H
#define C_GISVDAPIECHARTBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiDashboardPieChart.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaPieChartBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaPieChartBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                        const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                        QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaPieChartBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;

   void UpdateShowValue(void) override;
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   bool CallProperties(void) override;

   void UpdateTypePe(const bool oq_ShowUnit, const bool oq_ShowValue);
   void SetValuePe(const QString & orc_Value, const int32_t os32_Progress2000000);
   void SetUnitPe(const QString & orc_Value);

private:
   stw::opensyde_gui_elements::C_OgeWiDashboardPieChart * mpc_PieChartWidget;
   float64_t mf64_UnscaledMin;
   float64_t mf64_UnscaledMax;

   //Avoid call
   C_GiSvDaPieChartBase(const C_GiSvDaPieChartBase &);
   C_GiSvDaPieChartBase & operator =(const C_GiSvDaPieChartBase &) &; //lint !e1511 //we want to hide the base func.

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
