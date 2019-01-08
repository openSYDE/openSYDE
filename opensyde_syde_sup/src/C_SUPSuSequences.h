//-----------------------------------------------------------------------------
/*!
   \file
   \brief       SYDEsup integration class for system update sequences (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SUPSUSEQUENCES_H
#define C_SUPSUSEQUENCES_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCSuSequences.h"

/* -- Namespace ------------------------------------------------------------ */

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SUPSuSequences :
   public stw_opensyde_core::C_OSCSuSequences
{
public:
   C_SUPSuSequences();
   void SetQuiet(const bool & orq_Quiet);

protected:
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress,
                                 const stw_scl::C_SCLString & orc_Information);
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress,
                                 const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_Server,
                                 const stw_scl::C_SCLString & orc_Information);
   virtual void m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                            const stw_types::uint32 ou32_NodeIndex);
   virtual void m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                      const stw_types::uint32 ou32_NodeIndex);

private:
   bool mq_Quiet;
   bool m_CheckErrorCase(const C_OSCSuSequences::E_ProgressStep oe_Step) const;
   stw_scl::C_SCLString m_GetStepName(const E_ProgressStep oe_Step) const;
   void m_WriteLog(const stw_scl::C_SCLString & orc_Text, const bool & orq_IsError = false) const;
};

/* -- Extern Global Variables ---------------------------------------------- */

#endif
