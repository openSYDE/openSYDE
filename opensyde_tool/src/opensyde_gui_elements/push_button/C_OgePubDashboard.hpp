//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button part of push button item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXDASHBOARD_H
#define C_OGECHXDASHBOARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>
#include <QSvgRenderer>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubDashboard :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubDashboard(QWidget * const opc_Parent = NULL);
   ~C_OgePubDashboard(void) override;

   void SetFiles(const QString & orc_SvgActivated, const QString & orc_SvgDeactivated);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QSvgRenderer * mpc_RendererActivated;
   QSvgRenderer * mpc_RendererDeactivated;

   static QRectF mh_AdaptToAspcetRatio(const QRect & orc_Target, const QRect & orc_Rect);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
