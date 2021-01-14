//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       GUI Integration class for system update sequences (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSUSEQUENCES_H
#define C_SYVSUSEQUENCES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "C_OSCSuSequences.h"
#include "C_SyvComDriverThread.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "TGLTasks.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpSequences :
   public QObject,
   public stw_opensyde_core::C_OSCSuSequences
{
   Q_OBJECT

public:
   C_SyvUpSequences(void);
   virtual ~C_SyvUpSequences(void);

   stw_types::sint32 InitUpSequences(const stw_types::uint32 ou32_ViewIndex);
   stw_types::sint32 ReinitDispatcher(void);
   void CloseDispatcher(void);
   stw_types::sint32 SyvUpCreateTemporaryFolder(const stw_scl::C_SCLString & orc_TargetPath,
                                                std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                                QString & orc_ErrorPath) const;

   stw_types::sint32 StartActivateFlashloader(void);
   stw_types::sint32 StartReadDeviceInformation(void);
   stw_types::sint32 StartUpdateSystem(const std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                       const std::vector<stw_types::uint32> & orc_NodesOrder);
   stw_types::sint32 StartResetSystem(void);

   stw_types::sint32 GetResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetLastUpdatePosition(stw_types::uint32 & oru32_NodeIndex,
                                           stw_types::uint32 & oru32_FileIndex) const;

   void GetOsyDeviceInformation(std::vector<stw_types::uint32> & orc_OsyNodeIndexes,
                                std::vector<C_OsyDeviceInformation> & orc_OsyDeviceInformation);
   void GetXflDeviceInformation(std::vector<stw_types::uint32> & orc_XflNodeIndexes,
                                std::vector<C_XflDeviceInformation> & orc_XflDeviceInformation);
   QString GetStepName(const E_ProgressStep oe_Step) const;

   void AbortCurrentProgress(void);

   enum E_Sequence
   {
      eNOT_ACTIVE,
      eACTIVATE_FLASHLOADER,
      eREAD_DEVICE_INFORMATION,
      eUPDATE_SYSTEM,
      eRESET_SYSTEM
   };

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigReportProgress(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                          const stw_types::uint8 ou8_Progress) const;

   void SigReportProgressForServer(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                   const stw_types::uint8 ou8_Progress, const stw_types::uint8 ou8_BusIdentifier,
                                   const stw_types::uint8 ou8_NodeIdentifier) const;

   void SigReportOpenSydeFlashloaderInformationRead(void);
   void SigReportStwFlashloaderInformationRead(void);

protected:
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress,
                                 const stw_scl::C_SCLString & orc_Information) override;
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress,
                                 const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_Server,
                                 const stw_scl::C_SCLString & orc_Information) override;
   virtual void m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                           const stw_types::uint32 ou32_NodeIndex) override;
   virtual void m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                      const stw_types::uint32 ou32_NodeIndex) override;

private:
   //Avoid call
   C_SyvUpSequences(const C_SyvUpSequences &);
   C_SyvUpSequences & operator =(const C_SyvUpSequences &); //lint !e1511 //we want to hide the base func.

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   static void mh_WriteLog(const C_OSCSuSequences::E_ProgressStep oe_Step, const stw_scl::C_SCLString & orc_Text);

   C_SyvComDriverThread * mpc_Thread;
   stw_tgl::C_TGLCriticalSection * mpc_Lock;
   stw_can::C_CAN * mpc_CanDllDispatcher;
   stw_opensyde_core::C_OSCIpDispatcherWinSock * mpc_EthernetDispatcher;
   bool mq_AbortFlag;
   stw_types::uint32 mu32_ViewIndex;

   // Sequence execution parameter
   E_Sequence me_Sequence;
   // Input parameter for sequence
   std::vector<C_DoFlash> mc_NodesToFlash;
   std::vector<stw_types::uint32> mc_NodesOrder;

   // Service execution result
   stw_types::sint32 ms32_Result;

   //results of openSYDE device information scans:
   std::vector<stw_types::uint32> mc_ReportOsyDeviceInformationNodeIndex;
   std::vector<C_OsyDeviceInformation> mc_ReportOsyDeviceInformation;

   //results of STW Flashloader device information scans:
   std::vector<stw_types::uint32> mc_ReportXflDeviceInformationNodeIndex;
   std::vector<C_XflDeviceInformation> mc_ReportXflDeviceInformation;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
