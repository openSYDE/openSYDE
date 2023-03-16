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

#include "stwtypes.hpp"

#include "C_OgeWiWithToolTip.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OgeLabToolTipBase.hpp"

#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpSelectorWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorWidget :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpSelectorWidget() override;

   void InitStaticNames(void) const;
   void InitWidget(const QString & orc_Title, const QString & orc_TooltipTitle, const QString & orc_TooltipContent,
                   const QString & orc_InstanceName);
   void SetSelected(const bool oq_Selected);
   bool IsActive(void) const;
   bool SetTypeAndNode(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_Type, const uint32_t ou32_NodeIndex,
                       const bool oq_UsageViewActive, const bool oq_AddBtnVisible);
   void SetSelectedDataPool(const uint32_t ou32_Index) const;
   void SetCurrentDataPoolConflict(const int32_t os32_DataPoolWidgetIndex, const bool oq_Active);
   void ReloadDataPools(void) const;
   void UpdateDataPools(void);
   void UpdateActualDataPool(void) const;
   void ErrorCheck(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigWidgetFocused(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType);
   void SigOpenDataPoolContent(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType,
                               const int32_t os32_DataPoolWidgetIndex);
   void SigDataPoolChanged(void);
   void SigErrorCheck(void);
   void SigUpdateFollowingLists(stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType);
   void SigUpdateLists(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigNoDataPoolSelected(void);
   void SigDataPoolHoverStateChanged(const uint32_t ou32_DataPoolIndex, const bool oq_Hovered);

protected:
   void focusInEvent(QFocusEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeDpSelectorWidget(const C_SdNdeDpSelectorWidget &);
   C_SdNdeDpSelectorWidget & operator =(const C_SdNdeDpSelectorWidget &) &;

   void m_UpdateWidget(void);
   void m_ListFocused(void);
   void m_OpenDataPoolItemContent(const int32_t os32_DataPoolWidgetIndex);
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
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   stw::opensyde_gui_elements::C_OgeLabToolTipBase * mpc_LabelStateImg;
   bool mq_UsageViewActive;
   QPixmap mc_ConflictImg;
   uint32_t mu32_NodeIndex;
   stw::opensyde_core::C_OscNodeDataPool::E_Type me_DataPoolType;
   QString mc_InstanceName;
   bool mq_Selected;
};
}
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif // C_SDNDEDPSELECTORWIDGET_H
