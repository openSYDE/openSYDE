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
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSyColorBrightnessPicker :
   public QWidget
{
   Q_OBJECT

public:
   C_GiSyColorBrightnessPicker(QWidget * const opc_Parent = NULL);
   ~C_GiSyColorBrightnessPicker() override;

   void SetColorWithHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value);
   void SetColorWithHs(const int32_t os32_Hue, const int32_t os32_Sat);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigNewHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value);
   void SigColorSelected(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;

private:
   int32_t ms32_Hue;
   int32_t ms32_Sat;
   int32_t ms32_Value;
   QPixmap * mpc_Pixmap;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
