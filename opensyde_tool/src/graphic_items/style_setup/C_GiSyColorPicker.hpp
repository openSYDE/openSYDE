//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYCOLORPICKER_H
#define C_GISYCOLORPICKER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorPicker :
   public QFrame
{
   Q_OBJECT

public:
   C_GiSyColorPicker(QWidget * const opc_Parent = NULL);
   ~C_GiSyColorPicker(void) override;

   void SetCircleVisible(const bool oq_Visible);
   void SetColor(const int32_t os32_Hue, const int32_t os32_Saturation);

   QSize sizeHint(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigNewColor(int32_t os32_Hue, int32_t os32_Saturation);
   void SigColorSelected(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   bool mq_CircleVisible;
   int32_t ms32_Hue;
   int32_t ms32_Sat;
   QPixmap mc_Pixmap;

   void m_SetColorFromPosition(const QPoint & orc_Point);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
