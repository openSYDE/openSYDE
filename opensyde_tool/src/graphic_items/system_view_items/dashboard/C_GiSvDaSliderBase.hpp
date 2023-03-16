//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard slider item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDASLIDERBASE_H
#define C_GISVDASLIDERBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_SyvDaItDashboardSliderWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaSliderBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaSliderBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                      const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                      QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaSliderBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void ConnectionActiveChanged(const bool oq_Active) override;
   void SendCurrentValue(void) override;
   bool CallProperties(void) override;

   void UpdateTypePe(const stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type, const bool oq_ShowMinMax);
   void SetValuePe(const int32_t os32_Value, const int32_t os32_Min, const int32_t os32_Max);

protected:
   void m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex) override;

private:
   C_SyvDaItDashboardSliderWidget * mpc_SliderWidget;
   float64_t mf64_SliderFactor;
   float64_t mf64_UnscaledMinValue;

   //Avoid call
   C_GiSvDaSliderBase(const C_GiSvDaSliderBase &);
   C_GiSvDaSliderBase & operator =(const C_GiSvDaSliderBase &) &; //lint !e1511 //we want to hide the base func.

   void m_UpdateStaticValues(void);
   bool m_IsOnChange(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
