//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_BASICUPDATESEQUENCE_HPP
#define C_BASICUPDATESEQUENCE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscBuSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_BasicUpdateSequence :
   public stw::opensyde_core::C_OscBuSequences
{
public:
   C_BasicUpdateSequence();

protected:
   virtual void m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information);
   virtual void m_ReportFlashloaderInformationRead(const stw::scl::C_SclString & orc_DeviceName,
                                                   const stw::opensyde_core::C_OscComFlashloaderInformation & orc_Information);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
