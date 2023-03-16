//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for a drop area (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODEDROPAREAWIDGET_H
#define C_SYVDCEXISTINGNODEDROPAREAWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OscProtocolSerialNumber.hpp"
#include "C_SyvDcSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDcExistingNodeDropAreaWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDcExistingNodeDropAreaWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcExistingNodeDropAreaWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDcExistingNodeDropAreaWidget(void) noexcept override;

   void InitStaticNames(void) const;
   void SetContent(const bool oq_ValidSerialNumber,
                   const stw::opensyde_core::C_OscProtocolSerialNumber & orc_PureSerialNumber, const std::map<uint8_t,
                                                                                                              stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);
   bool IsAssigned(void) const;
   void GetContent(stw::opensyde_core::C_OscProtocolSerialNumber & orc_PureSerialNumber, std::map<uint8_t,
                                                                                                  stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> * const opc_SubNodeIdsToOldNodeIds)
   const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDisconnect(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_SyvDcExistingNodeDropAreaWidget * mpc_Ui;
   stw::opensyde_core::C_OscProtocolSerialNumber mc_PureSerialNumber;
   bool mq_Assigned;
   std::map<uint8_t, stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> mc_SubNodeIdsToOldNodeIds;

   void m_OnDisconnectRequest(void);

   //Avoid call
   C_SyvDcExistingNodeDropAreaWidget(const C_SyvDcExistingNodeDropAreaWidget &);
   C_SyvDcExistingNodeDropAreaWidget & operator =(const C_SyvDcExistingNodeDropAreaWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
