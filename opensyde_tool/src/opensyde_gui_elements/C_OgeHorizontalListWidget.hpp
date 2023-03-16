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

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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

   int32_t GetCountLines(void) const;
   int32_t GetActualLine(void) const;
   void SetActualLine(const int32_t os32_Line);
   void ScrollToItem(const int32_t os32_Index);

   static bool h_CheckValidMoveAction(const int32_t os32_StartIndex, const int32_t os32_EndIndex,
                                      const QListWidget & orc_Widget);

   void dropEvent(QDropEvent * const opc_Event) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigListChanged(void);

protected:
   void startDrag(const Qt::DropActions oc_SupportedActions) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   void wheelEvent(QWheelEvent * const opc_Event) override;

   virtual void m_MoveItem(const int32_t os32_SourceIndex, const int32_t os32_TargetIndex) = 0;
   virtual void m_UpdateNumbers(void) const = 0;
   virtual void m_DelegateStartPaint(void) = 0;
   virtual void m_DelegateStopPaint(void) = 0;

   int32_t ms32_DragItemIndex;
   int32_t ms32_ItemsPerLine;
   int32_t ms32_CountLines;
   int32_t ms32_ActualLine;

private:
   void m_DragTimeout(void);
   void m_StopDragTimer(void);

   QTimer mc_TimerDragMove;

   bool mq_DragTimeoutActiveLeft;
   bool mq_DragTimeoutActiveRight;

   static const int32_t mhs32_SCROLL_AREA;
   static const int32_t mhs32_DRAG_SCROLL_TIMER_INTERVAL_START;
   static const int32_t mhs32_DRAG_SCROLL_TIMER_INTERVAL;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
