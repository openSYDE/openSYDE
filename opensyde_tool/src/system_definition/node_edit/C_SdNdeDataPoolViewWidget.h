//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all available datapools of a node

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLVIEWWIDGET_H
#define C_SDNDEDATAPOOLVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

#include "stwtypes.h"

#include "C_SdNdeDataPoolSelectorWidget.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeDataPoolViewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDataPoolViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolViewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolViewWidget();

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void CheckDataPoolInteraction(void) const;
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
   C_SdNdeDataPoolViewWidget(const C_SdNdeDataPoolViewWidget &);
   C_SdNdeDataPoolViewWidget & operator =(const C_SdNdeDataPoolViewWidget &);

   void m_SubWidgetFocused(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                           const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange);
   void m_MinMaxClicked(void);
   void m_EmitActualDataPool(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                             const stw_types::sintn osn_DataPoolWidgetIndex);
   void m_NoDataPoolSelected(void);
   void m_ErrorCheck(void) const;
   void m_StoreToUserSettings(void) const;

   Ui::C_SdNdeDataPoolViewWidget * mpc_Ui;
   // array for more easy use of sub widgets
   C_SdNdeDataPoolSelectorWidget * mapc_Selectors[static_cast<stw_types::sintn>(stw_opensyde_core::C_OSCNodeDataPool::
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

/* -- Extern Global Variables ---------------------------------------------- */
#endif // C_SDNDEDATAPOOLVIEWWIDGET_H
