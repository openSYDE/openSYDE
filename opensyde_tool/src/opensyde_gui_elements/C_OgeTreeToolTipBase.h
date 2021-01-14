//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all tree tool tips
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREETOOLTIPBASE_H
#define C_OGETREETOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QHeaderView>

#include "stwtypes.h"
#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeToolTipBase :
   public C_OgeToolTipBase
{
public:
   C_OgeTreeToolTipBase(void);

protected:
   void m_CallAfterMouseMove(const QMouseEvent * const opc_Event);
   bool m_CallForEvent(QEvent * const opc_Event);
   void m_CallBeforeEventFilter(const QEvent * const opc_Event);

   virtual void m_HideToolTip(void) override;

   virtual void m_LastMinuteToolTipUpdate(void);
   virtual QPoint m_MapToGlobal(const QPoint & orc_LocalPos) const = 0;
   virtual const QHeaderView * m_Header(void) const = 0;
   virtual const QAbstractItemModel * m_Model(void) const = 0;
   virtual bool m_BaseEvent(QEvent * const opc_Event) = 0;
   virtual void m_SetMouseTracking(const bool oq_Active) = 0;
   virtual QModelIndex m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const = 0;

private:
   stw_types::sint32 ms32_HoveredNumberOfParents;
   stw_types::sint32 ms32_HoveredRow;
   stw_types::sint32 ms32_HoveredCol;
   stw_types::sint32 ms32_HoveredHorzHeader;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
   static stw_types::sint32 mh_CountParents(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
