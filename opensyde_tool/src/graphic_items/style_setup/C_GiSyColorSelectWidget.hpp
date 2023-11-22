//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for color selection.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYCOLORSELECTWIDGET_HPP
#define C_GISYCOLORSELECTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>
#include <QWidget>
#include <QTimer>
#include <QWindow>
#include <QScreen>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_GiSyScreenColorPickingEventFilter.hpp"
#include "C_GiSyColorPicker.hpp"
#include "C_GiSyColorBrightnessPicker.hpp"
#include "C_OgePubColor.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyColorSelectWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_GiSyColorSelectWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QColor oc_Color);
   ~C_GiSyColorSelectWidget(void) override;

   QColor ChooseSelectedColor(void) const;
   bool HandleColorPickingMouseMove(const QMouseEvent * const opc_MouseEvent);
   bool HandleColorPickingMouseButtonRelease(const QMouseEvent * const opc_MouseEvent);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_GiSyColorSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   //Avoid call
   C_GiSyColorSelectWidget(const C_GiSyColorSelectWidget &);
   C_GiSyColorSelectWidget & operator =(const C_GiSyColorSelectWidget &) &;

   QColor mc_PreviousColor;
   bool mq_ColorPickingActive;
   C_GiSyScreenColorPickingEventFilter * mpc_ScreenColorPickingEventFilter;
   mutable int32_t ms32_NextRecentButton;
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
   QVector<stw::opensyde_gui_elements::C_OgePubColor *> mc_StandardColorButtons;

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
   void m_StandardColorClicked(const stw::opensyde_gui_elements::C_OgePubColor * const opc_Button);
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
   void m_HtmlClicked(void) const;
   void m_RgbaClicked(void) const;
   void m_PushButtonHtml(void) const;
   void m_PushButtonRgba(void) const;
   void m_UpdateLineEditColor(void) const;
   void m_NewColor(void);
   void m_AutomaticallyFillOutLineEditColor(void);
   void m_SetRecentColorToRecentButton(stw::opensyde_gui_elements::C_OgePubColor * const opc_Button,
                                       const QColor oc_Color) const;
   void m_AddRecentColors(void);
   void m_NewHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value);
   void m_PickScreenColor(void);
   void m_UpdatePositionWhileScreenColorPicking(void);
   void m_UpdateColorPicking(const QPoint & orc_GlobalPosition);
   void m_SetCurrentRgbColor(const uint32_t ou32_Rgb); // see QRgb
   QColor m_GrabScreenColor(const QPoint & orc_Position) const;
   void m_NewColorTypedIn(const uint32_t ou32_Rgb); // see QRgb
   void m_LeaveColorPicking(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
