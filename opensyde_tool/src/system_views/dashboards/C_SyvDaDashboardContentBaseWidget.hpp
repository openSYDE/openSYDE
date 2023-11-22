//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDCONTENTBASEWIDGET_HPP
#define C_SYVDADASHBOARDCONTENTBASEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_SyvComDriverDiag.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardContentBaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardContentBaseWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardContentBaseWidget(void) override;

   virtual void SetDashboardIndex(const uint32_t ou32_Value) = 0;

   virtual void SetEditMode(const bool oq_Active) = 0;
   virtual void SetDarkMode(const bool oq_Active) = 0;
   virtual void SetDrawingActive(const bool oq_Active) = 0;
   virtual void Save(void) = 0;

   virtual void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) = 0;
   virtual void ConnectionActiveChanged(const bool oq_Active) = 0;
   virtual void UpdateShowValues(void) = 0;
   virtual void UpdateTransmissionConfiguration(void) = 0;
   virtual void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) = 0;

   virtual void SetFocus(void) const = 0;

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

private:
   //Avoid call
   C_SyvDaDashboardContentBaseWidget(const C_SyvDaDashboardContentBaseWidget &);
   C_SyvDaDashboardContentBaseWidget & operator =(const C_SyvDaDashboardContentBaseWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
