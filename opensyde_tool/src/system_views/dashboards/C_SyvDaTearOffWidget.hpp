//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a concrete instance of C_SyvDaDashboardWidget in a seperated window

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDATEAROFFWIDGET_H
#define C_SYVDATEAROFFWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_SyvDaDashboardWidget.hpp"
#include "C_OgeWiDashboardTab.hpp"
#include "C_SyvComDriverDiag.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaTearOffWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaTearOffWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaTearOffWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DataIndex, const QString & orc_Name,
                                 C_SyvDaDashboardWidget * const opc_Widget, QWidget * const opc_Parent = NULL);
   ~C_SyvDaTearOffWidget() override;

   void InitText() const;

   uint32_t GetIndex(void) const;
   void SetDataIndex(const uint32_t ou32_Value);
   QString GetName(void) const;

   C_SyvDaDashboardWidget * GetWidget(void);

   void SetEditMode(const bool oq_Active);
   void SetDarkMode(const bool oq_Active);
   void SetEnabled(const bool oq_Enabled) const;
   void Save(void) const;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) const;
   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active, const bool oq_WidgetTabOnly = false) const;
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConfirmClicked(void);
   void SigCancelClicked(void);
   void SigChanged(void);
   void SigErrorChange(void);
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex);
   void SigDataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index);
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);

   void SigWidgetComeBack(C_SyvDaTearOffWidget * const opc_Widget);
   void SigMySig(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void closeEvent(QCloseEvent * const opc_Event) override;

private:
   void m_OnActiveChange(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source,
                         const bool oq_Active);
   void m_OnCopy(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source) const;
   void m_OnEditProperties(stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_CheckError(void) const;
   void m_AdaptSpaceHolderWidgetColor(void) const;

   //Avoid call
   C_SyvDaTearOffWidget(const C_SyvDaTearOffWidget &);
   C_SyvDaTearOffWidget & operator =(const C_SyvDaTearOffWidget &) &;

   Ui::C_SyvDaTearOffWidget * mpc_Ui;
   C_SyvDaDashboardWidget * mpc_Dashboard;
   bool mq_EditModeActive;
   bool mq_DarkModeActive;
};
}
}

#endif
