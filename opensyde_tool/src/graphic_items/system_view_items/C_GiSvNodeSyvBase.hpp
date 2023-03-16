//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVNODESYVBASE_HPP
#define C_GISVNODESYVBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeSyvBase :
   public C_GiNode
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvNodeSyvBase(const uint32_t ou32_ViewIndex, const int32_t & ors32_NodeIndex, const uint64_t & oru64_Id,
                     const float64_t & orf64_Width = mhf64_MIN_WIDTH_NODE,
                     const float64_t & orf64_Height = mhf64_MIN_HEIGHT_NODE, QGraphicsItem * const opc_Parent = NULL);

   virtual void SetViewConnected(const bool oq_Connected);

   void LoadData(void) override;

protected:
   uint32_t mu32_ViewIndex;
   bool mq_ViewConnected;

   bool m_UpdateError(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
