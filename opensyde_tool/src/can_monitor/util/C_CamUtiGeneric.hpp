//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic utility to be used within and outside CAN Monitor
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMUTIGENERIC_HPP
#define C_CAMUTIGENERIC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamUtiGeneric
{
public:
   static QString h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir);

private:
   C_CamUtiGeneric();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
