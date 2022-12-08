//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting nodes (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUENODESELECTORWIDGET_H
#define C_SDBUENODESELECTORWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueNodeSelectorWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueNodeSelectorWidget() override;

   void SetBusId(const uint32_t ou32_BusIndex);
   void SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void Refresh(void) const;

   void InitStaticNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnectNodeToProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
   void SigConnectNodeToProtAndAddDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
   void SigDisconnectNodeFromProt(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
   void SigSwitchToCoManager(const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                             const uint8_t ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SdBueNodeSelectorWidget(const C_SdBueNodeSelectorWidget &);
   C_SdBueNodeSelectorWidget & operator =(const C_SdBueNodeSelectorWidget &) &;

   void m_NodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const bool oq_Checked) const;

   Ui::C_SdBueNodeSelectorWidget * mpc_Ui;
   uint32_t mu32_BusIndex;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_Protocol;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
