//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Ethernet bus (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIETHERNETBUS_H
#define C_GILIETHERNETBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

#include "stwtypes.hpp"

#include "C_OscSystemBus.hpp"
#include "C_GiLiBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiEthernetBus :
   public C_GiLiBus
{
public:
   C_GiLiEthernetBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                     C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                     const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiEthernetBus() override;

   int32_t type() const override;
   bool OpenStyleDialog(void) override;
   void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;
   stw::opensyde_core::C_OscSystemBus::E_Type GetType() const override;

private:
   //Avoid call
   C_GiLiEthernetBus(const C_GiLiEthernetBus &);
   C_GiLiEthernetBus & operator =(const C_GiLiEthernetBus &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
