//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard slider item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVDASLIDERBASE_H
#define C_GISVDASLIDERBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiSvDaRectBaseGroup.h"
#include "C_SyvDaItDashboardSliderWidget.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvDaSliderBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaSliderBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                      const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                      QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaSliderBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode);
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual void SendCurrentValue(void);
   virtual bool CallProperties(void) override;

   void UpdateTypePe(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type, const bool oq_ShowMinMax);
   void SetValuePe(const stw_types::sintn osn_Value, const stw_types::sintn osn_Min, const stw_types::sintn osn_Max);

private:
   C_SyvDaItDashboardSliderWidget * mpc_SliderWidget;
   stw_types::sint32 ms32_SliderMin;
   stw_types::float64 mf64_SliderFactor;
   stw_types::float64 mf64_UnscaledMinValue;

   //Avoid call
   C_GiSvDaSliderBase(const C_GiSvDaSliderBase &);
   C_GiSvDaSliderBase & operator =(const C_GiSvDaSliderBase &);

   void m_UpdateStaticValues(void);
   bool m_IsOnChange(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
