//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for an connected node entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCCONNECTEDNODEWIDGET_HPP
#define C_SYVDCCONNECTEDNODEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QListWidgetItem>
#include "stwtypes.hpp"
#include "C_OscDcDeviceInformation.hpp"

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
                                       const stw::opensyde_core::C_OscDcDeviceInformation & orc_Info,
                                       const std::map<uint8_t,
                                                      stw::opensyde_core::C_OscDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds,
                                       QWidget * const opc_Parent = NULL);
   ~C_SyvDcConnectedNodeWidget(void) noexcept override;

   QString GetPlainSerialNumberString(void) const;
   bool GetExtFormat(void) const;
   uint8_t GetManufacturerFormat(void) const;
   QString GetDeviceName(void) const;
   bool GetDeviceNameValid(void) const;
   std::map<uint8_t, stw::opensyde_core::C_OscDcDeviceOldComConfig> GetSubNodeIdsToOldNodeIds(void) const;
   bool CompareSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;
   void SetWidth(const int32_t os32_WidgetWidth);
   uint8_t GetNodeId(void) const;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_SyvDcConnectedNodeWidget * mpc_Ui;
   QListWidgetItem * mpc_ListWidgetItem;
   const stw::opensyde_core::C_OscDcDeviceInformation mc_Info;
   const std::map<uint8_t, stw::opensyde_core::C_OscDcDeviceOldComConfig> mc_SubNodeIdsToOldNodeIds;

   static const int32_t mhs32_HEIGHT;
   static const int32_t mhs32_WIDTH;
   static const int32_t mhs32_SCROLLBAR_WIDTH;

   void m_Init(void);

   //Avoid call
   C_SyvDcConnectedNodeWidget(const C_SyvDcConnectedNodeWidget &);
   C_SyvDcConnectedNodeWidget & operator =(const C_SyvDcConnectedNodeWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
