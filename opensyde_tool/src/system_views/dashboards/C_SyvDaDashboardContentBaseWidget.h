//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDCONTENTBASEWIDGET_H
#define C_SYVDADASHBOARDCONTENTBASEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

#include "C_SyvComDriverDiag.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardContentBaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardContentBaseWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardContentBaseWidget(void);

   virtual void SetDashboardIndex(const stw_types::uint32 ou32_Value) = 0;

   virtual void SetEditMode(const bool oq_Active) = 0;
   virtual void SetDarkMode(const bool oq_Active) = 0;
   virtual void SetDrawingActive(const bool oq_Active) = 0;
   virtual void Save(void) = 0;

   virtual void RegisterWidgets(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) = 0;
   virtual void ConnectionActiveChanged(const bool oq_Active) = 0;
   virtual void UpdateShowValues(void) = 0;
   virtual void UpdateTransmissionConfiguration(void) = 0;
   virtual void HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC) = 0;

   virtual void SetFocus(void) const = 0;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                         const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void SigDataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);
   void SigNvmReadList(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id);

private:
   //Avoid call
   C_SyvDaDashboardContentBaseWidget(const C_SyvDaDashboardContentBaseWidget &);
   C_SyvDaDashboardContentBaseWidget & operator =(const C_SyvDaDashboardContentBaseWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
