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
#include "stwtypes.hpp"
#include "C_SyvDcSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcConnectedNodeWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcConnectedNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcConnectedNodeWidget(QListWidgetItem * const opc_Item,
                                       const stw::opensyde_gui_logic::C_SyvDcDeviceInformation & orc_Info,
                                       const std::map<uint8_t,
                                                      stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds,
                                       QWidget * const opc_Parent = NULL);
   ~C_SyvDcConnectedNodeWidget(void) override;

   QString GetPlainSerialNumberString(void) const;
   bool GetExtFormat(void) const;
   uint8_t GetManufacturerFormat(void) const;
   QString GetDeviceName(void) const;
   bool GetDeviceNameValid(void) const;
   std::map<uint8_t, stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> GetSubNodeIdsToOldNodeIds(void) const;
   bool CompareSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_SyvDcConnectedNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   const stw::opensyde_gui_logic::C_SyvDcDeviceInformation mc_Info;
   const std::map<uint8_t, stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> mc_SubNodeIdsToOldNodeIds;

   void m_Init(void);
   //Avoid call
   C_SyvDcConnectedNodeWidget(const C_SyvDcConnectedNodeWidget &);
   C_SyvDcConnectedNodeWidget & operator =(const C_SyvDcConnectedNodeWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
