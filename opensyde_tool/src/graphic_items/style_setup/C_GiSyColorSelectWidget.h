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
#include <QVector>
#include <QWidget>
#include <QTimer>
#include <QWindow>
#include <QScreen>

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_GiSyScreenColorPickingEventFilter.h"
#include "C_GiSyColorPicker.h"
#include "C_GiSyColorBrightnessPicker.h"
#include "C_OgePubColor.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyColorSelectWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
//lint -e{1960}  Forward declarations necessary for automatic registration
class C_GiSyColorSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_GiSyColorSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QColor oc_Color);
   ~C_GiSyColorSelectWidget(void);

   QColor ChooseSelectedColor(void) const;
   bool HandleColorPickingMouseMove(const QMouseEvent * const opc_MouseEvent);
   bool HandleColorPickingMouseButtonRelease(const QMouseEvent * const opc_MouseEvent);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_GiSyColorSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   //Avoid call
   C_GiSyColorSelectWidget(const C_GiSyColorSelectWidget &);
   C_GiSyColorSelectWidget & operator =(const C_GiSyColorSelectWidget &);

   QColor mc_PreviousColor;
   bool mq_ColorPickingActive;
   C_GiSyScreenColorPickingEventFilter * mpc_ScreenColorPickingEventFilter;
   mutable stw_types::sintn msn_NextRecentButton;
   QTimer * mpc_UpdateTimer;
   QWindow mc_DummyTransparentWindow;
   C_GiSyColorPicker * mpc_ColorPicker;
   C_GiSyColorBrightnessPicker * mpc_ColorBrightnessPicker;
   QColor mc_LightBlue;
   QColor mc_Green;
   QColor mc_Gold;
   QColor mc_Orange;
   QColor mc_Red;
   QColor mc_Purple;
   QColor mc_LightGreen;
   QColor mc_DarkBlueGreen;
   QColor mc_Lavendel;
   QVector<stw_opensyde_gui_elements::C_OgePubColor *> mc_StandardColorButtons;

   QLabel * mpc_LabelTransparentColorShower;
   QLabel * mpc_LabelTransparentRecentColorNr1;
   QLabel * mpc_LabelTransparentRecentColorNr2;
   QLabel * mpc_LabelTransparentRecentColorNr3;
   QLabel * mpc_LabelTransparentRecentColorNr4;
   QLabel * mpc_LabelTransparentRecentColorNr5;
   QLabel * mpc_LabelTransparentRecentColorNr6;

   QColor mc_RecentColorNr1;
   QColor mc_RecentColorNr2;
   QColor mc_RecentColorNr3;
   QColor mc_RecentColorNr4;
   QColor mc_RecentColorNr5;
   QColor mc_RecentColorNr6;

   QColor mc_ColorBeforeScreenColorPicking;

   void m_InitStaticNames(void) const;
   void m_InitStandardColors(void);
   void m_InitElements(void) const;
   void m_InitStandardColorButtons(void);
   void m_SetTransparentBackground(void);
   void m_InitConnects(void) const;
   void m_RepaintColorShowerButton(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_StandardColorClicked(const stw_opensyde_gui_elements::C_OgePubColor * const opc_Button);
   void m_LightBlueClicked(void);
   void m_GreenClicked(void);
   void m_GoldClicked(void);
   void m_OrangeClicked(void);
   void m_RedClicked(void);
   void m_PurpleClicked(void);
   void m_LightGreenClicked(void);
   void m_DarkBlueGreenClicked(void);
   void m_LavendelClicked(void);
   void m_RecentColorNr1Clicked(void);
   void m_RecentColorNr2Clicked(void);
   void m_RecentColorNr3Clicked(void);
   void m_RecentColorNr4Clicked(void);
   void m_RecentColorNr5Clicked(void);
   void m_RecentColorNr6Clicked(void);
   void m_HMTLClicked(void) const;
   void m_RGBAClicked(void) const;
   void m_PushButtonHTML(void) const;
   void m_PushButtonRGBA(void) const;
   void m_UpdateLineEditColor(void) const;
   void m_NewColor(void);
   void m_AutomaticallyFillOutLineEditColor(void);
   void m_SetRecentColorToRecentButton(stw_opensyde_gui_elements::C_OgePubColor * const opc_Button,
                                       const QColor oc_Color) const;
   void m_AddRecentColors(void);
   void m_NewHsv(const stw_types::sintn osn_Hue, const stw_types::sintn osn_Sat, const stw_types::sintn osn_Value);
   void m_PickScreenColor(void);
   void m_UpdatePositionWhileScreenColorPicking(void);
   void m_UpdateColorPicking(const QPoint & orc_GlobalPosition);
   void m_SetCurrentRgbColor(const QRgb oun_Rgb);
   QColor m_GrabScreenColor(const QPoint & orc_Position) const;
   void m_NewColorTypedIn(const QRgb oun_Rgb);
   void m_LeaveColorPicking(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
