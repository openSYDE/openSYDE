//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard progress bar item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAPROGRESSBARBASE_H
#define C_GISVDAPROGRESSBARBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiSvDaRectBaseGroup.h"
#include "C_SyvDaItDashboardProgressBarWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaProgressBarBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaProgressBarBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                           const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                           QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaProgressBarBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void UpdateShowValue(void) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;

   virtual bool CallProperties(void) override;

   void UpdateTypePe(const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type oe_Type,
                     const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                     const bool oq_ShowMinMax);
   void SetValuePe(const stw_types::sintn osn_Value);
   virtual void UpdateTransparency(const stw_types::uint32 ou32_DataElementIndex,
                                   const stw_types::sintn osn_Value) override;

private:
   C_SyvDaItDashboardProgressBarWidget * const mpc_ProgressBarWidget;
   stw_types::float64 mf64_UnscaledMin;
   stw_types::float64 mf64_UnscaledMax;

   //Avoid call
   C_GiSvDaProgressBarBase(const C_GiSvDaProgressBarBase &);
   C_GiSvDaProgressBarBase & operator =(const C_GiSvDaProgressBarBase &); //lint !e1511 //we want to hide the base func.

   void m_UpdateStaticValues(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
