//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all system view dashboard items which are rectangle based (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDARECTBASEGROUP_HPP
#define C_GISVDARECTBASEGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>

#include "C_PuiSvDbWidgetBase.hpp"
#include "C_GiBiRectBaseGroup.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"
#include "C_GiWiProxyBase.hpp"
#include "C_SyvDaItDashboardBaseWidget.hpp"
#include "C_SyvDaContextMenuManager.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_GiSvgGraphicsItem.hpp"
#include "C_GiBiCustomToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaRectBaseGroup :
   public C_GiBiRectBaseGroup,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   C_GiSvDaRectBaseGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                         const int32_t & ors32_DataIndex, const C_PuiSvDbDataElement::E_Type & ore_Type,
                         const uint32_t ou32_MaximumDataElements, const uint64_t & oru64_Id,
                         const float64_t of64_MinWidth, const float64_t of64_MinHeight, const float64_t of64_InitWidth,
                         const float64_t of64_InitHeight, const bool oq_KeepAspectRatio, const bool oq_ReadItem,
                         QGraphicsItem * const opc_Parent = NULL, const QPointF & orc_PosOffset = QPointF(-1.0, -1.0));
   ~C_GiSvDaRectBaseGroup(void) override;

   bool IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos) const;
   bool IsOverrideCursorNecessary(const QPointF & orc_ScenePos, Qt::CursorShape & ore_Cursor) const;

   virtual void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode);
   void SetDefaultCursor(const QCursor & orc_Value) override;
   virtual void ReInitializeSize(void) = 0;

   void LoadSvBasicData(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase & orc_Data);
   void UpdateSvBasicData(stw::opensyde_gui_logic::C_PuiSvDbWidgetBase & orc_Data,
                          const bool oq_SkipDataElements = false) const;
   void DeleteData(void) override;

   void RegisterDataPoolElementCyclicError(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
      const uint8_t ou8_ErrorCode) override;

   virtual void UpdateShowValue(void);
   virtual void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value);
   virtual void ConnectionActiveChanged(const bool oq_Active);
   virtual void EditModeActiveChanged(const bool oq_Active);
   virtual void SendCurrentValue(void);
   virtual bool CallProperties(void);
   virtual bool EnableEditContent(void);
   virtual void DisableEditContent(void);
   bool IsEditContentEnabled(void) const;
   virtual void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc);
   void SetErrorForInvalidDlc(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ElementId,
                              const uint8_t ou8_Dlc) override;
   virtual void SetDrawingActive(const bool oq_Active);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

   virtual void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager, const bool oq_Active);

   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style GetDisplayStyleType(void) const;

   void GenerateHint(void) override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigWidgetHandling(const bool oq_State);
   void SigSelected(QGraphicsItem * const opc_Item, const bool oq_ClearSelection);
   void SigDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex);
   void SigDataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index);
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataElementsChanged(void);
   void SigHideToolTip(void);

protected:
   const stw::opensyde_gui_logic::C_PuiSvDashboard * m_GetSvDashboard(void) const;

   int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, QString & orc_ScaledValue,
                          float64_t * const opf64_UnscaledValueAsFloat) override;
   int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, std::vector<QString> & orc_ScaledValues,
                          std::vector<float64_t> & orc_UnscaledValues) override;

   int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, std::vector<float64_t> & orc_Values,
                          const bool oq_UseScaling) override;
   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;
   virtual void m_ForceWidgetResize(const QSizeF & orc_NewSize);

   QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void keyReleaseEvent(QKeyEvent * const opc_Event) override;
   void focusOutEvent(QFocusEvent * const opc_Event) override;
   void wheelEvent(QGraphicsSceneWheelEvent * const opc_Event) override;
   void hoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

   void m_DataPoolElementsChanged(void) override;
   virtual bool m_AllowRefreshButton(void) const;
   virtual void m_UpdateErrorIcon(void);
   virtual bool m_AllowWarningIcon(void) const;
   virtual bool m_HasEditContentMode(void) const;
   virtual QString m_GetCommonToolTipContent(void) const;

   C_SyvDaItDashboardBaseWidget * mpc_Widget;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode me_WriteMode;
   bool mq_DarkMode;
   float64_t mf64_WriteValue;
   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString> mc_CommmunicationErrors;
   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, uint8_t> mc_InvalidDlcSignals;
   bool mq_InitialStyleCall;
   bool mq_EditModeActive;         // Edit mode of dashboard
   bool mq_EditContentModeEnabled; // Edit content mode of specific widget

private:
   C_GiWiProxyBase * mpc_ProxyWidget;
   C_GiSvgGraphicsItem * mpc_WarningIcon;
   C_GiSvgGraphicsItem * mpc_ConflictIcon;
   C_GiSvgGraphicsItem * mpc_RefreshIcon;
   QGraphicsItemGroup * mpc_ButtonGroup;
   int32_t ms32_IconSize;
   static const float64_t mhf64_ACTION_POINT_OFFSET;
   QSizeF mc_CurrentSize;
   bool mq_ProxyWidgetInteractionActive;
   std::vector<int32_t> mc_LastTransparencyValue;
   static const uint8_t mhu8_START_GREY_TIMEOUT_PERCENTAGE;
   bool mq_ConnectionActive;
   bool mq_ShowButton;
   bool mq_AbortTriggered;
   uint32_t mu32_NextManualActionIndex; ///< 0: Inactive
   // Else: If current is finished this is the next index to try,
   // else reset

   static const QString mhc_ICON_READ;
   static const QString mhc_ICON_READ_ABORT;
   static const QString mhc_ICON_READ_DISABLED;
   static const QString mhc_ICON_WRITE;
   static const QString mhc_ICON_WRITE_ABORT;
   static const QString mhc_ICON_WRITE_DISABLED;

   //Avoid call
   C_GiSvDaRectBaseGroup(const C_GiSvDaRectBaseGroup &);
   C_GiSvDaRectBaseGroup & operator =(const C_GiSvDaRectBaseGroup &) &; //lint !e1511 //we want to hide the base func.

   void m_InitConflictIcon(void);
   void m_InitButton(void);
   void m_ManualRead(void);
   void m_HandleGenericButtonClick(void);
   void m_UpdateErrorIconToolTip(void);
   void m_ManualOperationStarted(void);
   void m_ManualOperationFinished(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
