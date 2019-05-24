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
#include "C_OgeWiDashboardPieChart.h"
#include "C_GiSvDaRectBaseGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaPieChartBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaPieChartBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                        const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                        QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaPieChartBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;

   virtual void UpdateShowValue(void) override;
   virtual void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex,
                                   const stw_types::sintn osn_Value) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual bool CallProperties(void) override;

   void UpdateTypePe(const bool oq_ShowUnit, const bool oq_ShowValue);
   void SetValuePe(const QString & orc_Value, const stw_types::sint32 os32_Progress2000000);
   void SetUnitPe(const QString & orc_Value);

private:
   stw_opensyde_gui_elements::C_OgeWiDashboardPieChart * mpc_PieChartWidget;
   stw_types::float64 mf64_UnscaledMin;
   stw_types::float64 mf64_UnscaledMax;

   //Avoid call
   C_GiSvDaPieChartBase(const C_GiSvDaPieChartBase &);
   C_GiSvDaPieChartBase & operator =(const C_GiSvDaPieChartBase &);

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
