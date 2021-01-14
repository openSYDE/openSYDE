//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Slider part of dashboard slider item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESLIDASHBOARD_H
#define C_OGESLIDASHBOARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSvgRenderer>
#include "C_OgeSliToolTipBase.h"
#include "C_PuiSvDbSlider.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSliDashboard :
   public C_OgeSliToolTipBase
{
   Q_OBJECT

public:
   C_OgeSliDashboard(QWidget * const opc_Parent = NULL);
   ~C_OgeSliDashboard(void);

   void SetCurrentStyle(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Style oe_Style, const bool oq_IsDark);
   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type);
   void HandleResize(void);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QSvgRenderer * mpc_SvgRenderer;
   bool mq_IsDark;
   stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Style me_Style;
   stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type me_Type;

   void m_SetSvg(const QString & orc_Path);
   void m_DrawHandle(const QRect & orc_Rect);
   void m_ReinitStyle(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
