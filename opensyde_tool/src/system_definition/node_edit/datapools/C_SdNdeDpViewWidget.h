//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all available datapools of a node

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPVIEWWIDGET_H
#define C_SDNDEDPVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_SdNdeDpSelectorWidget.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpViewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpViewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpViewWidget();

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void SetActualDataPoolConflict(const bool oq_Active) const;
   void UpdateActualDataPool(void) const;
   void SetActualDataPool(const stw_types::uint32 ou32_DataPoolIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataPoolSelected(const stw_types::uint32 ou32_DataPoolIndex);
   void SigChanged(void);
   void SigErrorChange(void) const;
   void SigUpdateLists(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigNoDataPoolSelected(void);

private:
   //Avoid call
   C_SdNdeDpViewWidget(const C_SdNdeDpViewWidget &);
   C_SdNdeDpViewWidget & operator =(const C_SdNdeDpViewWidget &);

   void m_SubWidgetFocused(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                           const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange);
   void m_MinMaxClicked(void);
   void m_EmitActualDataPool(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                             const stw_types::sintn osn_DataPoolWidgetIndex);
   void m_NoDataPoolSelected(void);
   void m_ErrorCheck(void) const;
   void m_StoreToUserSettings(void) const;

   Ui::C_SdNdeDpViewWidget * mpc_Ui;
   // array for more easy use of sub widgets
   C_SdNdeDpSelectorWidget * mapc_Selectors[static_cast<stw_types::sintn>(stw_opensyde_core::C_OSCNodeDataPool::
                                                                          eCOM) + 1];
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_LastKnownDataPoolIndex;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_ActiveDataPoolType;
   stw_types::sintn msn_ActiveDataPoolWidget;
   bool mq_Maximized;
   bool mq_IndexValid;

   static const bool mhaq_StorageIndicatorActive[3];
};
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif // C_SDNDEDPVIEWWIDGET_H
