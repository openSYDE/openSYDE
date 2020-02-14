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
#include "C_OSCNode.h"
#include "C_PuiBsBox.h"
#include "C_PuiSdNodeConnectionId.h"
#include "C_PuiSdNodeDataPool.h"
#include "C_PuiSdNodeCanProtocol.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeConnection ///< contains only GUI specific information (therefore does not inherit)
{
public:
   C_PuiSdNodeConnection();

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   C_PuiSdNodeConnectionId c_ConnectionID;
   std::vector<QPointF> c_UINodeConnectionInteractionPoints;
};

class C_PuiSdNode :
   public C_PuiBsBox
{
public:
   C_PuiSdNode();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void DeleteConnection(const C_PuiSdNodeConnectionId & orc_ConnectionID);

   std::vector<C_PuiSdNodeConnection> c_UIBusConnections; ///< GUI information about this node's bus connections
   std::vector<C_PuiSdNodeDataPool> c_UIDataPools;
   std::vector<C_PuiSdNodeCanProtocol> c_UICanProtocols;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
