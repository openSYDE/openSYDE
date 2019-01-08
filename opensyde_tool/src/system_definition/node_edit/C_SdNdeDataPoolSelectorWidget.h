//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all datapools of a node and a specific category

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORWIDGET_H
#define C_SDNDEDATAPOOLSELECTORWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"

#include "C_OgeWiWithToolTip.h"
#include "C_OSCNodeDataPool.h"
#include "C_OgeLabToolTipBase.h"

#include "C_SdNdeDataPoolSelectorUsageWidget.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeDataPoolSelectorWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolSelectorWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolSelectorWidget();

   void InitStaticNames(void) const;
   void InitWidget(const QString & orc_Title, const QString & orc_TooltipTitle, const QString & orc_TooltipContent,
                   const QString & orc_InstanceName);
   void SetActive(const bool oq_Active);
   void CheckDataPoolInteraction(void);
   bool IsActive(void) const;
   void SetMaximized(const bool oq_Maximized);
   bool SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                       const stw_types::uint32 ou32_NodeIndex, const bool oq_UsageViewActive);
   void SetActualDataPoolConflict(const bool oq_Active);
   void UpdateActualDataPool(void) const;
   bool SetDataPoolActive(const stw_types::uint32 ou32_DataPoolIndex) const;
   void ErrorCheck(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigWidgetFocused(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                         const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange = false);
   void SigChanged(void);
   void SigErrorCheck(void);
   void SigUpdateLists(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigNoDataPoolSelected(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdNdeDataPoolSelectorWidget(const C_SdNdeDataPoolSelectorWidget &);
   C_SdNdeDataPoolSelectorWidget & operator =(const C_SdNdeDataPoolSelectorWidget &);

   void m_UpdateWidget(void);
   void m_DpChangedUpdateUsageView(const bool oq_InitChange);
   void m_ListFocused(const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_ForceChange);
   void m_ButtonRightClicked(void) const;
   void m_ButtonLeftClicked(void) const;
   void m_OnErrorCheck(void);

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SetupContextMenu(void);
   void m_AddNewDatapool(void) const;
   void m_PasteDatapool(void) const;
   void m_UpdateErrorToolTip(void);

   Ui::C_SdNdeDataPoolSelectorWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw_opensyde_gui_elements::C_OgeLabToolTipBase * mpc_LabelStateImg;
   C_SdNdeDataPoolSelectorUsageWidget * mpc_UsageBar;
   bool mq_UsageViewActive;
   QPixmap mc_ConflictImg;
   stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_DataPoolType;
   QString mc_InstanceName;
   bool mq_Active;
   bool mq_AllowInteraction;
};
}

/* -- Extern Global Variables ---------------------------------------------- */
#endif // C_SDNDEDATAPOOLSELECTORWIDGET_H
