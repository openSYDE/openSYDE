//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Show the brightness of the current color.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYCOLORBRIGHTNESSPICKER_H
#define C_GISYCOLORBRIGHTNESSPICKER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDesktopWidget>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSyColorBrightnessPicker :
   public QWidget
{
   Q_OBJECT

public:
   C_GiSyColorBrightnessPicker(QWidget * const opc_Parent = NULL);
   ~C_GiSyColorBrightnessPicker();

   void SetColorWithHSV(const stw_types::sintn osn_Hue, const stw_types::sintn osn_Sat,
                        const stw_types::sintn osn_Value);
   void SetColorWithHS(const stw_types::sintn osn_Hue, const stw_types::sintn osn_Sat);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigNewHsv(const stw_types::sintn osn_Hue, const stw_types::sintn osn_Sat, const stw_types::sintn osn_Value);
   void SigColorSelected(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;

private:
   stw_types::sintn msn_Hue;
   stw_types::sintn msn_Sat;
   stw_types::sintn msn_Value;
   QPixmap * mp_Pixmap;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
