//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A datapool item widget for the list widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORITEMWIDGET_H
#define C_SDNDEDPSELECTORITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"

#include "C_OgeWiWithToolTip.hpp"
#include "C_OscNodeDataPoolId.hpp"
#include "C_OgeLabToolTipBase.hpp"
#include "C_SdNdeDpSelectorItemUsageWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpSelectorItemWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpSelectorItemWidget :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorItemWidget(const bool oq_UsageViewActive, QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpSelectorItemWidget() override;

   void InitWidget(void);

   void SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content);
   void SetNumber(const uint32_t ou32_Number);
   void SetData(const stw::opensyde_core::C_OscNodeDataPoolId & orc_OscDataPoolId);
   void SetData(const QString & orc_DatapoolName, const QString & orc_DatapoolComment, const bool oq_Safety,
                const uint32_t ou32_Size, const uint32_t ou32_Used, const uint32_t ou32_Reserved);
   void UpdateData(void);
   void SetSelected(const bool oq_Selected);
   void SetStateConflict(const bool oq_Active);
   void SetShareDatapool(const bool oq_Shared);
   const stw::opensyde_core::C_OscNodeDataPoolId & GetDatapoolId(void) const;
   QString GetDatapoolName(void) const;
   QString GetDatapoolComment(void) const;
   bool GetStateSafety(void) const;
   bool GetSelected(void) const;
   bool GetStateConflict(void) const;
   bool GetShareDatapool(void) const;
   bool GetUsageViewActive(void) const;
   uint32_t GetDataPoolSize(void) const;
   uint32_t GetDataPoolUsedSize(void) const;
   uint32_t GetDataPoolReservedSize(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigUpdateErrorToolTip(const uint32_t ou32_Index);
   void SigClicked(const uint32_t ou32_Index);
   void SigDoubleClicked(const uint32_t ou32_Index);
   void SigContextMenuRequested(const QPoint & orc_Pos, const uint32_t ou32_Index);
   void SigHoverStateChanged(const uint32_t ou32_DataPoolIndex, const bool oq_Hovered);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void showEvent(QShowEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeDpSelectorItemWidget(const C_SdNdeDpSelectorItemWidget &);
   C_SdNdeDpSelectorItemWidget & operator =(const C_SdNdeDpSelectorItemWidget &) &;

   void m_UpdateLabel(void);
   void m_UpdateName(void) const;
   void m_SetSelectColor(const bool oq_Active);
   void m_UpdateErrorToolTip(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   Ui::C_SdNdeDpSelectorItemWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeLabToolTipBase * mpc_LabelStateImg;
   QLabel * mpc_LabelShareImg;
   QLabel * mpc_LabelShareImgHovered;
   C_SdNdeDpSelectorItemUsageWidget * mpc_UsageBar;
   QPixmap mc_ConflictImg;

   stw::opensyde_core::C_OscNodeDataPoolId mc_DatapoolId;
   bool mq_StateSafety;
   bool mq_StateConflict;
   bool mq_Shared;
   bool mq_Selected;
   const bool mq_UsageViewActive;
   uint32_t mu32_Size;
   uint32_t mu32_Used;
   uint32_t mu32_Reserved;
   uint32_t mu32_Number;
   QString mc_Name;
};
}
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif // C_SDNDEDPSELECTORITEMWIDGET_H
