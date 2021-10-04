//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an connected node entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCCONNECTEDNODEWIDGET_H
#define C_SYVDCCONNECTEDNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QListWidgetItem>
#include "stwtypes.h"
#include "C_SyvDcSequences.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcConnectedNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcConnectedNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcConnectedNodeWidget(QListWidgetItem * const opc_Item,
                                       const stw_opensyde_gui_logic::C_SyvDcDeviceInformation & orc_Info,
                                       const std::set<stw_types::uint8> & orc_SubNodeIds,
                                       QWidget * const opc_Parent = NULL);
   ~C_SyvDcConnectedNodeWidget(void);

   QString GetPlainSerialNumberString(void) const;
   bool GetExtFormat(void) const;
   stw_types::uint8 GetManufacturerFormat(void) const;
   QString GetDeviceName(void) const;
   bool GetDeviceNameValid(void) const;
   bool CompareSerialNumber(const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber) const;

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_SyvDcConnectedNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   const stw_opensyde_gui_logic::C_SyvDcDeviceInformation mc_Info;
   const std::set<stw_types::uint8> mc_SubNodeIds;

   void m_Init(void);
   //Avoid call
   C_SyvDcConnectedNodeWidget(const C_SyvDcConnectedNodeWidget &);
   C_SyvDcConnectedNodeWidget & operator =(const C_SyvDcConnectedNodeWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
