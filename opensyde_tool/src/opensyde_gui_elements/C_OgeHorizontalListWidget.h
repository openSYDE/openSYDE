//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for horizontal orientated list widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEHORIZONTALLISTWIDGET_H
#define C_OGEHORIZONTALLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>
#include <QListWidget>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeHorizontalListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   C_OgeHorizontalListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeHorizontalListWidget();

   stw_types::sintn GetCountLines(void) const;
   stw_types::sintn GetActualLine(void) const;
   void SetActualLine(const stw_types::sintn osn_Line);
   void ScrollToItem(const stw_types::sintn osn_Index);

   static bool h_CheckValidMoveAction(const stw_types::sintn osn_StartIndex, const stw_types::sint32 os32_EndIndex,
                                      const QListWidget & orc_Widget);

   virtual void dropEvent(QDropEvent * const opc_Event) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigListChanged(void);

protected:
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   virtual void wheelEvent(QWheelEvent * const opc_Event) override;

   virtual void m_MoveItem(const stw_types::sintn osn_SourceIndex, const stw_types::sintn osn_TargetIndex) = 0;
   virtual void m_UpdateNumbers(void) const = 0;
   virtual void m_DelegateStartPaint(void) = 0;
   virtual void m_DelegateStopPaint(void) = 0;

   stw_types::sintn msn_DragItemIndex;
   stw_types::sintn msn_ItemsPerLine;
   stw_types::sintn msn_CountLines;
   stw_types::sintn msn_ActualLine;

private:
   void m_DragTimeout(void);
   void m_StopDragTimer(void);

   QTimer mc_TimerDragMove;

   bool mq_DragTimeoutActiveLeft;
   bool mq_DragTimeoutActiveRight;

   static const stw_types::sintn mhsn_SCROLL_AREA;
   static const stw_types::sintn mhsn_DRAG_SCROLL_TIMER_INTERVAL_START;
   static const stw_types::sintn mhsn_DRAG_SCROLL_TIMER_INTERVAL;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
