//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a dashboard with a specific chart

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHAWIDGET_H
#define C_SYVDACHAWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaDashboardContentBaseWidget.h"
#include "C_PuiSvDbDataElementHandler.h"

#include "stwtypes.h"

#include "C_OgeContextMenu.h"
#include "C_PuiSvDbTabChart.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaChaWidget :
   public C_SyvDaDashboardContentBaseWidget,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler
{
   Q_OBJECT

public:
   explicit C_SyvDaChaWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_DashboardIndex,
                             QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaWidget(void);

   // virtual Functions of C_SyvDaDashboardContentBaseWidget
   virtual void SetDashboardIndex(const stw_types::uint32 ou32_Value) override;

   virtual void SetEditMode(const bool oq_Active) override;
   virtual void SetDarkMode(const bool oq_Active) override;
   virtual void SetDrawingActive(const bool oq_Active) override;
   virtual void Save(void) override;

   virtual void RegisterWidgets(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual void UpdateShowValues(void) override;
   virtual void UpdateTransmissionConfiguration(void) override;
   virtual void HandleManualOperationFinished(const stw_types::sint32 os32_Result,
                                              const stw_types::uint8 ou8_NRC) override;
   virtual void RegisterDataPoolElementCyclicError(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
      const stw_types::uint8 ou8_ErrorCode) override;

   virtual void SetFocus(void) const override;

   // virtual Functions of C_PuiSvDbDataElementHandler
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
   virtual void SetErrorForInvalidDlc(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ElementId,
                                      const stw_types::uint8 ou8_DLC) override;
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataElementsChanged(void);

protected:
   virtual void m_DataPoolElementsChanged(void) override;

   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaWidget * mpc_Ui;
   bool mq_InitialStyleCall;
   bool mq_DarkMode;
   bool mq_DrawingActive;
   bool mq_EditMode;
   bool mq_IsConnected;
   bool mq_ManualOperationAbortTriggered;
   stw_types::uint32 mu32_ManualOperationActionIndex;
   // Context menu
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionRemove;
   QAction * mpc_ActionRemoveAll;
   QAction * mpc_ActionConfigDataElement;

   static const stw_types::uint32 mhu32_MAXIMUM_DATA_ELEMENTS;

   QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString> mc_CommmunicationErrors;
   QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, stw_types::uint8> mc_InvalidDlcSignals;

   //Avoid call
   C_SyvDaChaWidget(const C_SyvDaChaWidget &);
   C_SyvDaChaWidget & operator =(const C_SyvDaChaWidget &); //lint !e1511 //we want to hide the base func.

   void m_LoadChartData(void);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   void m_AddNewDataElement(void);
   void m_RemoveDataElement(void);
   void m_RemoveAllDataElements(void);
   void m_RegisterDataElementRail(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   void m_CallProperties(void);
   stw_types::sint32 m_SetChangedChartData(void);
   void m_UpdateErrorIcon(void);

   void m_ManualReadTriggered(void);
   void m_ManualRead(void);
   void m_ManualReadAbortTriggered(void);
   void m_ManualReadFinished(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
