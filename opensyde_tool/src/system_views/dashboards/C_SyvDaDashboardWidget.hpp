//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a concrete dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDWIDGET_HPP
#define C_SYVDADASHBOARDWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_SyvDaDashboardContentBaseWidget.hpp"
#include "C_SyvComDriverDiag.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaDashboardWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   const QString & orc_Name, const bool oq_Window, QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardWidget() override;

   uint32_t GetViewIndex(void) const;
   uint32_t GetDashboardIndex(void) const;
   void SetDashboardIndex(const uint32_t ou32_Value);
   QString GetName(void) const;

   void SetEditMode(const bool oq_Active);
   void SetDarkMode(const bool oq_Active);
   void SetDrawingActive(const bool oq_Active) const;
   void Save(void) const;

   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active) const;
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void) const;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) const;

   void SetFocus(void) const;
   void SetCurrentDashboardTabName(const QString & orc_CurrentDashboardTabName) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex);
   void SigDataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index,
                        stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DashboardWidget);
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);
   void SigGetCurrentDashboardTabName(void);

private:
   //Avoid call
   C_SyvDaDashboardWidget(const C_SyvDaDashboardWidget &);
   C_SyvDaDashboardWidget & operator =(const C_SyvDaDashboardWidget &) &;

   void m_InitSceneWidget(void);
   void m_InitChartWidget(void);

   Ui::C_SyvDaDashboardWidget * mpc_Ui;
   C_SyvDaDashboardContentBaseWidget * mpc_Content; // Pointer to scene or chart
   const uint32_t mu32_ViewIndex;
   uint32_t mu32_DashboardIndex;
   const QString mc_Name;
};
}
}

#endif
