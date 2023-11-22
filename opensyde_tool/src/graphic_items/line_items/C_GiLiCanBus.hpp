//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN bus (header)

   CAN bus (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILICANBUS_HPP
#define C_GILICANBUS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

#include "C_GiLiBus.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiCanBus :
   public C_GiLiBus
{
public:
   C_GiLiCanBus(const int32_t & ors32_Index, const uint64_t & oru64_Id, C_GiTextElementBus * const opc_TextElementName,
                const bool oq_DoErrorCheck, const std::vector<QPointF> * const opc_Points = NULL,
                QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiCanBus() override;

   int32_t type() const override;
   bool OpenStyleDialog(void) override;
   stw::opensyde_core::C_OscSystemBus::E_Type GetType() const override;

private:
   //Avoid call
   C_GiLiCanBus(const C_GiLiCanBus &);
   C_GiLiCanBus & operator =(const C_GiLiCanBus &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
