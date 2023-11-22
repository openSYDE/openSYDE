//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDCOMMAND_HPP
#define C_SYVDAUNOADDCOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbDataElement.hpp"
#include "C_SyvDaUnoAddBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddCommand :
   public C_SyvDaUnoAddBaseCommand
{
public:
   C_SyvDaUnoAddCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                        const C_PuiSvDbDataElement::E_Type & ore_Type, const QPointF & orc_NewPos,
                        const float64_t of64_ZetValue, const QString & orc_AdditionalInformation = "",
                        QUndoCommand * const opc_Parent = NULL, const bool & orq_ForceUseAdditionalInformation = false,
                        const bool & orq_DarkModeDefault = false);
   ~C_SyvDaUnoAddCommand(void) override;

protected:
   void m_AddNew(void) override;

private:
   const C_PuiSvDbDataElement::E_Type me_Type;
   const QPointF mc_NewPos;
   const float64_t mf64_ZetValue;
   const QString mc_AdditionalInformation;
   const bool mq_ForceUseAdditionalInformation;
   const bool mq_DarkModeDefault;

   static void mh_DecodeTheme(C_PuiSvDbWidgetBase & orc_Base, const QString & orc_Theme);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
