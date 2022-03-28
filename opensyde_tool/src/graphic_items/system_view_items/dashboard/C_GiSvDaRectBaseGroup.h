//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all system view dashboard items which are rectangle based (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDARECTBASEGROUP_H
#define C_GISVDARECTBASEGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>

#include "C_PuiSvDbWidgetBase.h"
#include "C_GiBiRectBaseGroup.h"
#include "C_PuiSvDbDataElementHandler.h"
#include "C_GiWiProxyBase.h"
#include "C_SyvDaItDashboardBaseWidget.h"
#include "C_SyvDaContextMenuManager.h"
#include "C_PuiSvDashboard.h"
#include "C_GiSvgGraphicsItem.h"
#include "C_GiBiCustomToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaRectBaseGroup :
   public C_GiBiRectBaseGroup,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   C_GiSvDaRectBaseGroup(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                         const stw_types::sint32 & ors32_DataIndex, const C_PuiSvDbDataElement::E_Type & ore_Type,
                         const stw_types::uint32 ou32_MaximumDataElements, const stw_types::uint64 & oru64_ID,
                         const stw_types::float64 of64_MinWidth, const stw_types::float64 of64_MinHeight,
                         const stw_types::float64 of64_InitWidth, const stw_types::float64 of64_InitHeight,
                         const bool oq_KeepAspectRatio, const bool oq_ReadItem, QGraphicsItem * const opc_Parent = NULL,
                         const QPointF & orc_PosOffset = QPointF(-1.0, -1.0));
   virtual ~C_GiSvDaRectBaseGroup(void);

   bool IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos) const;

   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode);
   virtual void SetDefaultCursor(const QCursor & orc_Value) override;
   virtual void ReInitializeSize(void) = 0;

   void LoadSvBasicData(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase & orc_Data);
   void UpdateSvBasicData(stw_opensyde_gui_logic::C_PuiSvDbWidgetBase & orc_Data,
                          const bool oq_SkipDataElements = false) const;
   virtual void DeleteData(void) override;

   virtual void RegisterDataPoolElementCyclicError(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
      const stw_types::uint8 ou8_ErrorCode);

   virtual void UpdateShowValue(void);
   virtual void UpdateTransparency(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value);
   virtual void ConnectionActiveChanged(const bool oq_Active);
   virtual void EditModeActiveChanged(const bool oq_Active);
   virtual void SendCurrentValue(void);
   virtual bool CallProperties(void);
   virtual bool EnableEditContent(void);
   virtual void DisableEditContent(void);
   bool IsEditContentEnabled(void) const;
   virtual void HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);
   virtual void SetErrorForInvalidDlc(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ElementId,
                                      const stw_types::uint8 ou8_DLC);
   virtual void SetDrawingActive(const bool oq_Active);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;

   virtual void ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager, const bool oq_Active);

   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style GetDisplayStyleType(void) const;

   virtual void GenerateHint(void);

   //GI base
   virtual void SetZValueCustom(const stw_types::float64 of64_ZValue) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigWidgetHandling(const bool oq_State);
   void SigSelected(QGraphicsItem * const opc_Item, const bool oq_ClearSelection);
   void SigDataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                         const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void SigDataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataElementsChanged(void);
   void SigHideToolTip(void);

protected:
   const stw_opensyde_gui_logic::C_PuiSvDashboard * m_GetSvDashboard(void) const;

   virtual stw_types::sint32 m_GetLastValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                            std::vector<stw_types::float64> & orc_Values,
                                            std::vector<QString> * const opc_StringifiedValues,
                                            const bool oq_UseScaling) override;
   virtual stw_types::sint32 m_GetLastValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                            stw_types::float64 & orf64_Value, const bool oq_UseScaling) override;
   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth, const stw_types::float64 of64_DiffHeight);
   virtual void m_ForceWidgetResize(const QSizeF & orc_NewSize);

   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void keyReleaseEvent(QKeyEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void wheelEvent(QGraphicsSceneWheelEvent * const opc_Event) override;
   virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

   virtual void m_DataPoolElementsChanged(void) override;
   virtual bool m_AllowRefreshButton(void) const;
   virtual void m_UpdateErrorIcon(void);
   virtual bool m_AllowWarningIcon(void) const;
   virtual bool m_HasEditContentMode(void) const;
   virtual QString m_GetCommonToolTipContent(void) const;

   C_SyvDaItDashboardBaseWidget * mpc_Widget;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style me_Style;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode me_WriteMode;
   bool mq_DarkMode;
   stw_types::float64 mf64_WriteValue;
   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString> mc_CommmunicationErrors;
   QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, stw_types::uint8> mc_InvalidDlcSignals;
   bool mq_InitialStyleCall;
   bool mq_EditModeActive;         // Edit mode of dashboard
   bool mq_EditContentModeEnabled; // Edit content mode of specific widget

private:
   C_GiWiProxyBase * mpc_ProxyWidget;
   C_GiSvgGraphicsItem * mpc_WarningIcon;
   C_GiSvgGraphicsItem * mpc_ConflictIcon;
   C_GiSvgGraphicsItem * mpc_RefreshIcon;
   QGraphicsItemGroup * mpc_ButtonGroup;
   stw_types::sint32 ms32_IconSize;
   static const stw_types::float64 mhf64_ACTION_POINT_OFFSET;
   QSizeF mc_CurrentSize;
   bool mq_ProxyWidgetInteractionActive;
   std::vector<stw_types::sintn> mc_LastTransparencyValue;
   static const stw_types::uint8 mhu8_START_GREY_TIMEOUT_PERCENTAGE;
   bool mq_ConnectionActive;
   bool mq_ShowButton;
   bool mq_AbortTriggered;
   stw_types::uint32 mu32_NextManualActionIndex; ///< 0: Inactive
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
   C_GiSvDaRectBaseGroup & operator =(const C_GiSvDaRectBaseGroup &); //lint !e1511 //we want to hide the base func.

   void m_InitConflictIcon(void);
   void m_InitButton(void);
   void m_ManualRead(void);
   void m_HandleGenericButtonClick(void);
   void m_UpdateErrorIconToolTip(void);
   void m_ManualOperationStarted(void);
   void m_ManualOperationFinished(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
