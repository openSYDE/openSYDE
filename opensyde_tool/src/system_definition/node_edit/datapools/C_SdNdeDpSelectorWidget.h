//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all datapools of a node and a specific category

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORWIDGET_H
#define C_SDNDEDPSELECTORWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"

#include "C_OgeWiWithToolTip.h"
#include "C_OSCNodeDataPool.h"
#include "C_OgeLabToolTipBase.h"

#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpSelectorWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpSelectorWidget();

   void InitStaticNames(void) const;
   void InitWidget(const QString & orc_Title, const QString & orc_TooltipTitle, const QString & orc_TooltipContent,
                   const QString & orc_InstanceName);
   void SetSelected(const bool oq_Selected);
   bool IsActive(void) const;
   bool SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                       const stw_types::uint32 ou32_NodeIndex, const bool oq_UsageViewActive,
                       const bool oq_AddBtnVisible);
   void SetCurrentDataPoolConflict(const stw_types::sintn osn_DataPoolWidgetIndex, const bool oq_Active);
   void UpdateActualDataPool(void) const;
   void ErrorCheck(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigWidgetFocused(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType);
   void SigOpenDataPoolContent(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                               const stw_types::sintn osn_DataPoolWidgetIndex);
   void SigDataPoolChanged(void);
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
   C_SdNdeDpSelectorWidget(const C_SdNdeDpSelectorWidget &);
   C_SdNdeDpSelectorWidget & operator =(const C_SdNdeDpSelectorWidget &);

   void m_UpdateWidget(void);
   void m_ListFocused(void);
   void m_OpenDataPoolItemContent(const stw_types::sintn osn_DataPoolWidgetIndex);
   void m_ButtonRightClicked(void) const;
   void m_ButtonLeftClicked(void) const;
   void m_OnErrorCheck(void);

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SetupContextMenu(void);
   void m_AddNewDatapool(void) const;
   void m_JumpToEdit(void) const;
   void m_PasteDatapool(void) const;
   void m_UpdateErrorToolTip(void);

   Ui::C_SdNdeDpSelectorWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw_opensyde_gui_elements::C_OgeLabToolTipBase * mpc_LabelStateImg;
   bool mq_UsageViewActive;
   QPixmap mc_ConflictImg;
   stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_DataPoolType;
   QString mc_InstanceName;
   bool mq_Selected;
};
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif // C_SDNDEDPSELECTORWIDGET_H
