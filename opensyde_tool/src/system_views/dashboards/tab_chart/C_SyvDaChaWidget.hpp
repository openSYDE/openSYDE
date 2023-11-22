//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a dashboard with a specific chart

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHAWIDGET_HPP
#define C_SYVDACHAWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaDashboardContentBaseWidget.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"

#include "stwtypes.hpp"

#include "C_OgeContextMenu.hpp"
#include "C_PuiSvDbTabChart.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaChaWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaChaWidget :
   public C_SyvDaDashboardContentBaseWidget,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler
{
   Q_OBJECT

public:
   explicit C_SyvDaChaWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                             QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaWidget(void) override;

   // Functions of C_SyvDaDashboardContentBaseWidget
   void SetDashboardIndex(const uint32_t ou32_Value) override;

   void SetEditMode(const bool oq_Active) override;
   void SetDarkMode(const bool oq_Active) override;
   void SetDrawingActive(const bool oq_Active) override;
   void Save(void) override;

   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   void UpdateShowValues(void) override;
   void UpdateTransmissionConfiguration(void) override;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) override;
   void RegisterDataPoolElementCyclicError(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
      const uint8_t ou8_ErrorCode) override;

   void SetFocus(void) const override;

   // Functions of C_PuiSvDbDataElementHandler
   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;
   void SetErrorForInvalidDlc(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ElementId,
                              const uint8_t ou8_Dlc) override;
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataElementsChanged(void);

protected:
   void m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex) override;
   void m_DataPoolElementsChanged(void) override;

   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_SyvDaChaWidget * mpc_Ui;
   bool mq_InitialStyleCall;
   bool mq_DarkMode;
   bool mq_DrawingActive;
   bool mq_EditMode;
   bool mq_IsConnected;
   bool mq_ManualOperationAbortTriggered;
   uint32_t mu32_ManualOperationActionIndex;
   // Context menu
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionRemove;
   QAction * mpc_ActionRemoveAll;
   QAction * mpc_ActionConfigDataElement;

   static const uint32_t mhu32_MAXIMUM_DATA_ELEMENTS;

   QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString> mc_CommmunicationErrors;
   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, uint8_t> mc_InvalidDlcSignals;

   //Avoid call
   C_SyvDaChaWidget(const C_SyvDaChaWidget &);
   C_SyvDaChaWidget & operator =(const C_SyvDaChaWidget &) &; //lint !e1511 //we want to hide the base func.

   void m_LoadChartData(void);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   void m_AddNewDataElement(void);
   void m_RemoveDataElement(void);
   void m_RemoveAllDataElements(void);
   void m_RegisterDataElementRail(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const;
   void m_CallProperties(void);
   int32_t m_SetChangedChartData(void);
   void m_UpdateErrorIcon(void);

   void m_ManualReadTriggered(void);
   void m_ManualRead(void);
   void m_ManualReadAbortTriggered(void);
   void m_ManualReadFinished(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
