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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorPicker :
   public QFrame
{
   Q_OBJECT

public:
   C_GiSyColorPicker(QWidget * const opc_Parent);
   ~C_GiSyColorPicker(void);

   void SetCircleVisible(const bool oq_Visible);
   void SetColor(const stw_types::sintn osn_H, const stw_types::sintn osn_S);

   virtual QSize sizeHint(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void NewColor(stw_types::sintn oc_h, stw_types::sintn oc_s);
   void ColorSelected(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   bool mq_CircleVisible;
   stw_types::sintn msn_Hue;
   stw_types::sintn msn_Sat;
   QPixmap mc_Pixmap;

   void m_SetColorFromPosition(const QPoint & orc_Point);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
