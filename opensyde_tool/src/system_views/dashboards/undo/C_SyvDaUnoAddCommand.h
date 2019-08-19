//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDCOMMAND_H
#define C_SYVDAUNOADDCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbDataElement.h"
#include "C_SyvDaUnoAddBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddCommand :
   public C_SyvDaUnoAddBaseCommand
{
public:
   C_SyvDaUnoAddCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                        const C_PuiSvDbDataElement::E_Type & ore_Type, const QPointF & orc_NewPos,
                        const stw_types::float64 of64_ZValue, const QString & orc_AdditionalInformation = "",
                        QUndoCommand * const opc_Parent = NULL, const bool & orq_ForceUseAdditionalInformation = false,
                        const bool & orq_DarkModeDefault = false);
   virtual ~C_SyvDaUnoAddCommand(void);

protected:
   virtual void m_AddNew(void);

private:
   const C_PuiSvDbDataElement::E_Type me_Type;
   const QPointF mc_NewPos;
   const stw_types::float64 mf64_ZValue;
   const QString mc_AdditionalInformation;
   const bool mq_ForceUseAdditionalInformation;
   const bool mq_DarkModeDefault;

   static void mh_DecodeTheme(C_PuiSvDbWidgetBase & orc_Base, const QString & orc_Theme);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
