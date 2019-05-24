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

#include "stwtypes.h"

#include "C_OSCSystemBus.h"
#include "C_GiLiBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiEthernetBus :
   public C_GiLiBus
{
public:
   C_GiLiEthernetBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                     C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                     const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiEthernetBus();

   virtual stw_types::sintn type() const override;
   virtual bool OpenStyleDialog(void);
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;
   virtual stw_opensyde_core::C_OSCSystemBus::E_Type GetType() const;

private:
   //Avoid call
   C_GiLiEthernetBus(const C_GiLiEthernetBus &);
   C_GiLiEthernetBus & operator =(const C_GiLiEthernetBus &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
