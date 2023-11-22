//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard progress bar item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAPROGRESSBARBASE_HPP
#define C_GISVDAPROGRESSBARBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_SyvDaItDashboardProgressBarWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaProgressBarBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaProgressBarBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                           const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                           QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaProgressBarBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void UpdateShowValue(void) override;
   void ConnectionActiveChanged(const bool oq_Active) override;

   bool CallProperties(void) override;

   void UpdateTypePe(const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type oe_Type,
                     const stw::opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                     const bool oq_ShowMinMax);
   void SetValuePe(const int32_t os32_Value);
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value) override;

private:
   C_SyvDaItDashboardProgressBarWidget * const mpc_ProgressBarWidget;
   float64_t mf64_UnscaledMin;
   float64_t mf64_UnscaledMax;

   //Avoid call
   C_GiSvDaProgressBarBase(const C_GiSvDaProgressBarBase &);
   C_GiSvDaProgressBarBase & operator =(const C_GiSvDaProgressBarBase &) &; //lint !e1511 //we want to hide the base
                                                                            // func.

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
