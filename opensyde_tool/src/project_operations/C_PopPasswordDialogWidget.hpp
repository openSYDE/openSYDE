//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog is shown if a encrypted service project is loaded
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPPASSWORDDIALOGWIDGET_H
#define C_POPPASSWORDDIALOGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDialog>
#include "C_OgePopUpDialog.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_PopPasswordDialogWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopPasswordDialogWidget :
   public QDialog
{
   Q_OBJECT

public:
   explicit C_PopPasswordDialogWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_PopPasswordDialogWidget(void);

   void HandleMousePressEvent(const QMouseEvent * const opc_Event);
   void HandleMouseReleaseEvent(const QMouseEvent * const opc_Event);
   void HandleMouseMoveEvent(const QMouseEvent * const opc_Event);

   void InitStaticNames(void) const;
   QString GetPassword(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_PopPasswordDialogWidget * mpc_Ui;
   bool mq_Pressed;   ///< Indicator for mouse currently being pressed to allow specific actions
   QPoint mc_LastPos; ///< Last known position of this widget
   int32_t ms32_MinHeight;
   int32_t ms32_MinWidth;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ShowPassword(void);
   void m_HidePassword(void);

   //Avoid call
   C_PopPasswordDialogWidget(const C_PopPasswordDialogWidget &);
   C_PopPasswordDialogWidget & operator =(const C_PopPasswordDialogWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
