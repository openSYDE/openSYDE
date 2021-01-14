//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom pop-up dialog (header)

   Custom pop-up dialog (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPOPUPDIALOG_H
#define C_OGEPOPUPDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include <QDialog>
#include <QAbstractButton>
#include <QPointer>
#include "C_OgeOverlay.h"

namespace Ui
{
class C_OgePopUpDialog;
}

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePopUpDialog :
   public QDialog
{
   Q_OBJECT

public:
   explicit C_OgePopUpDialog(QWidget * const opc_Parent = NULL, QWidget * const opc_EffectTargetChild = NULL,
                             const bool oq_AddShadow = true);
   virtual ~C_OgePopUpDialog();

   void SetWidget(QWidget * const opc_Widget);
   void SetTitle(const QString & orc_Str);
   void SetSubTitle(const QString & orc_Str);

   void HandleMousePressEvent(const QMouseEvent * const opc_Event);
   void HandleMouseReleaseEvent(QMouseEvent * const opc_Event);
   void HandleMouseMoveEvent(const QMouseEvent * const opc_Event);
   void SetSize(const QSize & orc_Size);
   void ApplyMaximumSize(const QWidget * const opc_Widget = NULL);
   void HideTitle(void) const;
   void HideOverlay(void) const;
   void SetNotifyAndBlockClose(const bool oq_NotifyAndBlockClose);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigCloseIgnored();

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void closeEvent(QCloseEvent * const opc_Event) override;

private:
   Ui::C_OgePopUpDialog * mpc_Ui;     ///< Reference for variables defined in ui file
   bool mq_Pressed;                   ///< Indicator for mouse currently being pressed to allow specific actions
   QPoint mc_LastPos;                 ///< Last known position of this widget
   QPointer<C_OgeOverlay> mc_Overlay; ///< Overlay to display when pop up widget is triggered
   QWidget * mpc_EffectTargetChild;
   QString mc_Title;
   QString mc_Subtitle;
   QString mc_WidgetClassName;
   bool mq_NotifyAndBlockClose;

   void m_SetWindowTitle();

   //Avoid call
   C_OgePopUpDialog(const C_OgePopUpDialog &);
   C_OgePopUpDialog & operator =(const C_OgePopUpDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
