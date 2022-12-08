//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI node: stores additional UI information (header)

   UI node: stores additional UI information (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEH
#define C_PUISDNODEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QPointF>
#include "C_OscNode.hpp"
#include "C_PuiBsBox.hpp"
#include "C_PuiSdNodeConnectionId.hpp"
#include "C_PuiSdNodeDataPool.hpp"
#include "C_PuiSdNodeCanProtocol.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeConnection ///< contains only GUI specific information (therefore does not inherit)
{
public:
   C_PuiSdNodeConnection();

   void CalcHash(uint32_t & oru32_HashValue) const;

   C_PuiSdNodeConnectionId c_ConnectionId;
   std::vector<QPointF> c_UiNodeConnectionInteractionPoints;
};

class C_PuiSdNode :
   public C_PuiBsBox
{
public:
   C_PuiSdNode();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   void DeleteConnection(const C_PuiSdNodeConnectionId & orc_ConnectionId);

   std::vector<C_PuiSdNodeConnection> c_UiBusConnections; ///< GUI information about this node's bus connections
   std::vector<C_PuiSdNodeDataPool> c_UiDataPools;
   std::vector<C_PuiSdNodeCanProtocol> c_UiCanProtocols;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
