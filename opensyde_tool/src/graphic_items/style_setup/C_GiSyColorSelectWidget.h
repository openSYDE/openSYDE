//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for color selection.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYCOLORSELECTWIDGET_H
#define C_GISYCOLORSELECTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include <QTimer>
#include <QWindow>
#include "C_ColorPickingEventFilter.h"
#include "C_ColorOptions.h"
#include "C_PlatformColorHelper.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyColorSelectWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSyColorSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_GiSyColorSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_GiSyColorSelectWidget(void);

   QColor m_ChooseSelectedColor(QColor);

//   enum SetColorMode {
//       ShowColor = 0x1,
//       SelectColor = 0x2,
//       SetColorAll = ShowColor | SelectColor
//   };

//   enum {
//       colorColumns = 8,
//       standardColorRows = 6,
//       customColorRows = 2
//   };

////   C_GiSyColorSelectWidget() : options(new C_ColorOptions)
////#ifdef Q_OS_WIN32
////      , updateTimer(0)
////#endif
////    {}

//   C_PlatformColorHelper *platformHelper() const;
//   C_PlatformColorHelper *platformColorHelper() const
//       { return static_cast<C_PlatformColorHelper *>(platformHelper()); }

//   bool nativeDialogInUse;
////   QTimer * updateTimer;
////   QWindow dummyTransparentWindow;
//   C_ColorPickingEventFilter *colorPickingEventFilter;
//   void m_pickScreenColor();
//   void setCurrentColor(const QColor &color,  SetColorMode setColorMode = SetColorAll);
//   QColor grabScreenColor(const QPoint &p);
//   void m_updateColorPicking();
//   void updateColorPicking(const QPoint &globalPos);
//   void setCurrentRgbColor(QRgb rgb);
//   void m_newColorTypedIn(QRgb rgb);
//   inline void setCurrentAlpha(int a);
//   bool selectColor(const QColor &col);
//   static inline void rgb2hsv(QRgb rgb, int &h, int &s, int &v);
//   QColorShower *cs;
//   QRgb beforeScreenColorPicking;
//   QSharedPointer<C_ColorOptions> options;
//   bool crossVisible;
//   bool setCrossVisible(bool visible);

//   bool handleColorPickingMouseMove(QMouseEvent *event);
//   bool handleColorPickingMouseButtonRelease(QMouseEvent *event);
//   bool handleColorPickingKeyPress(QKeyEvent *event);

//#ifdef Q_OS_WIN32
//    QTimer *updateTimer;
//    QWindow dummyTransparentWindow;
//#endif

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private Q_SLOTS:
   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_LightBlueClicked(void);
   void m_GreenClicked(void);
   void m_GoldClicked(void);
   void m_OrangeClicked(void);
   void m_RedClicked(void);
   void m_PurpleClicked(void);
   void m_LightGreenClicked(void);
   void m_DarkBlueGreenClicked(void);
   void m_LavendelClicked(void);

   void m_HMTLClicked(void);
   void m_RGBAClicked(void);
   void m_LineEditTextChanged();

private:
   Ui::C_GiSyColorSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   //Avoid call
   C_GiSyColorSelectWidget(const C_GiSyColorSelectWidget &);
   C_GiSyColorSelectWidget & operator =(const C_GiSyColorSelectWidget &);

   QColor * lightBlue;
   QColor * green;
   QColor * gold;
   QColor * orange;
   QColor * red;
   QColor * purple;
   QColor * lightGreen;
   QColor * darkBlueGreen;
   QColor * lavendel;

   void InitStaticNames(void) const;
   void InitStandardColors(void);
   void InitElements(void) const;
   void m_PushButtonHTML(void) const;
   void m_PushButtonRGBA(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
