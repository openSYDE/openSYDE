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

#include "stwtypes.h"

#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueNodeSelectorWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueNodeSelectorWidget();

   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void Refresh(void);

   void InitStaticNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnectNodeToProt(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex) const;
   void SigConnectNodeToProtAndAddDataPool(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_InterfaceIndex) const;
   void SigDisconnectNodeFromProt(const stw_types::uint32 ou32_NodeIndex,
                                  const stw_types::uint32 ou32_InterfaceIndex) const;

private:
   //Avoid call
   C_SdBueNodeSelectorWidget(const C_SdBueNodeSelectorWidget &);
   C_SdBueNodeSelectorWidget & operator =(const C_SdBueNodeSelectorWidget &);

   void m_NodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const bool oq_Checked) const;

   Ui::C_SdBueNodeSelectorWidget * mpc_Ui;
   stw_types::uint32 mu32_BusIndex;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_Protocol;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
