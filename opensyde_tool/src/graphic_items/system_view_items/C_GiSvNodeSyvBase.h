//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVNODESYVBASE_H
#define C_GISVNODESYVBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvNodeSyvBase :
   public C_GiNode
{
public:
   //lint -e{1938} TODO
   C_GiSvNodeSyvBase(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_NodeIndex,
                     const stw_types::uint64 & oru64_ID, const stw_types::float64 & orf64_Width = mhf64_MinWidthNode,
                     const stw_types::float64 & orf64_Height = mhf64_MinHeightNode,
                     QGraphicsItem * const opc_Parent = NULL);

   virtual void SetViewConnected(const bool oq_Connected);

   virtual void LoadData(void) override;

protected:
   stw_types::uint32 mu32_ViewIndex;
   bool mq_ViewConnected;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
