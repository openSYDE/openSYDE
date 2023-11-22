//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SVG icon only push button (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBSVGICONONLY_HPP
#define C_OGEPUBSVGICONONLY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSvgRenderer>
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubSvgIconOnly :
   public C_OgePubToolTipBase
{
public:
   C_OgePubSvgIconOnly(QWidget * const opc_Parent = NULL);
   ~C_OgePubSvgIconOnly(void) override;

   void SetSvg(const QString & orc_PathEnabled, const QString & orc_PathDisabled = "",
               const QString & orc_PathHovered = "", const QString & orc_PathCheckedEnabled = "",
               const QString & orc_PathCheckedDisabled = "", const QString & orc_PathCheckedHovered = "",
               const QString & orc_PathPressed = "", const QString & orc_PathCheckedPressed = "");

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   bool mq_Hovered;
   bool mq_Pressed;
   QSvgRenderer * mpc_SvgRendererEnabled;
   QSvgRenderer * mpc_SvgRendererDisabeld;
   QSvgRenderer * mpc_SvgRendererHovered;
   QSvgRenderer * mpc_SvgRendererPressed;
   QSvgRenderer * mpc_SvgRendererCheckedEnabled;
   QSvgRenderer * mpc_SvgRendererCheckedDisabeld;
   QSvgRenderer * mpc_SvgRendererCheckedHovered;
   QSvgRenderer * mpc_SvgRendererCheckedPressed;

private:
   //Avoid call
   C_OgePubSvgIconOnly(const C_OgePubSvgIconOnly &);
   C_OgePubSvgIconOnly & operator =(const C_OgePubSvgIconOnly &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
