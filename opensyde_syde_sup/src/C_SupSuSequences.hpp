//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SYDEsup integration class for system update sequences (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SUPSUSEQUENCES_H
#define C_SUPSUSEQUENCES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscSuSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SupSuSequences :
   public stw::opensyde_core::C_OscSuSequences
{
public:
   C_SupSuSequences(void);
   void SetQuiet(const bool & orq_Quiet);
   const std::vector<C_OsyDeviceInformation> & GetActiveOsyDeviceInformation(std::vector<uint16_t> & orc_Indexes) const;
   const std::vector<C_XflDeviceInformation> & GetActiveXflDeviceInformation(std::vector<uint16_t> & orc_Indexes) const;
   void ClearActiveDeviceInformation(void); //call before ReadDeviceInformation(...)
protected:
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                 const uint8_t ou8_Progress,
                                 const stw::scl::C_SclString & orc_Information);
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                 const uint8_t ou8_Progress,
                                 const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_Server,
                                 const stw::scl::C_SclString & orc_Information);
   virtual void m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                            const uint32_t ou32_NodeIndex);
   virtual void m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                      const uint32_t ou32_NodeIndex);

private:
   bool mq_Quiet;
   bool m_CheckErrorCase(const C_OscSuSequences::E_ProgressStep oe_Step) const;
   stw::scl::C_SclString m_GetStepName(const E_ProgressStep oe_Step) const;
   void m_WriteLog(const stw::scl::C_SclString & orc_Text, const bool & orq_IsError = false) const;

   std::vector<uint16_t> mc_OsyDeviceInformationIndexes; //absolute index of device as reported by virtual function
   std::vector<C_OsyDeviceInformation> mc_ActiveOsyDeviceInformation;
   std::vector<uint16_t> mc_XflDeviceInformationIndexes; //absolute index of device as reported by virtual function
   std::vector<C_XflDeviceInformation> mc_ActiveXflDeviceInformation;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
