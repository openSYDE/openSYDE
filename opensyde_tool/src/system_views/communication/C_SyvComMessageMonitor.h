//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for preparing CAN message data for showing on GUI (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMMESSAGEMONITOR_H
#define C_SYVCOMMESSAGEMONITOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QList>
#include <QVector>

#include "stwtypes.h"
#include "stw_can.h"

#include "TGLTasks.h"

#include "C_OSCComMessageLogger.h"
#include "C_OSCComMessageLoggerData.h"

#include "C_SyvComDriverThread.h"
#include "C_OSCSystemBus.h"
#include "C_CieConverter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComMessageMonitor :
   public stw_opensyde_core::C_OSCComMessageLogger
{
public:
   C_SyvComMessageMonitor(void);
   virtual ~C_SyvComMessageMonitor(void);

   virtual void Start(void) override;
   virtual void Stop(void) override;

   // openSYDE system definition handling
   stw_types::sint32 StartAddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition);
   stw_types::sint32 StartAddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                       const stw_types::uint32 ou32_BusIndex);

   virtual stw_types::sint32 SetOsySysDefBus(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                             const stw_types::uint32 ou32_BusIndex) override;
   virtual stw_types::sint32 GetOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                          stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_SystemDefinition)
   override;

   // DBC handling
   stw_types::sint32 StartAddDbcFile(const stw_scl::C_SCLString & orc_PathDbc);
   stw_types::sint32 GetDbcFile(const stw_scl::C_SCLString & orc_PathDbc,
                                C_CieConverter::C_CIECommDefinition & orc_DbcDefinition);

   // Generic database handling
   virtual stw_types::sint32 RemoveDatabase(const stw_scl::C_SCLString & orc_Path) override;
   virtual stw_types::sint32 ActivateDatabase(const stw_scl::C_SCLString & orc_Path, const bool oq_Active) override;

   // Logging handling
   virtual stw_types::sint32 AddLogFileAsc(const stw_scl::C_SCLString & orc_FilePath, const bool oq_HexActive,
                                           const bool oq_RelativeTimeStampActive) override;
   virtual stw_types::sint32 AddLogFileBlf(const stw_scl::C_SCLString & orc_FilePath);
   virtual stw_types::sint32 RemoveLogFile(const stw_scl::C_SCLString & orc_FilePath) override;
   virtual void RemoveAllLogFiles(void) override;

   // Filter handling
   virtual void AddFilter(const stw_opensyde_core::C_OSCComMessageLoggerFilter & orc_Filter) override;
   virtual void RemoveFilter(const stw_opensyde_core::C_OSCComMessageLoggerFilter & orc_Filter) override;
   virtual void RemoveAllFilter(void) override;
   virtual stw_types::uint32 GetFilteredMessages(void) const override;

   // CAN bus handling
   virtual stw_types::sint32 HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx) override;
   virtual void ResetCounter(void) override;
   virtual void UpdateBusLoad(const stw_types::uint8 ou8_BusLoad) override;
   virtual void UpdateTxCounter(const stw_types::uint32 ou32_TxCount) override;
   virtual void UpdateTxErrors(const stw_types::uint32 ou32_TxErrors) override;

   stw_types::uint8 GetBusLoad(void) const;
   stw_types::uint32 GetTxCount(void) const;
   stw_types::uint32 GetTxErrors(void) const;

   stw_types::sint32 GetResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetResultBusses(std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses) const;

protected:
   stw_types::sint32 m_GetCanMessage(stw_opensyde_core::C_OSCComMessageLoggerData & orc_Message);

   void m_UpdateProtocolString(stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) const;

   virtual bool m_CheckFilter(const stw_can::T_STWCAN_Msg_RX & orc_Msg) override;

   virtual void m_InsertOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const stw_opensyde_core::C_OSCSystemDefinition & orc_OsySysDef,
                                  const stw_types::uint32 ou32_BusIndex) override;
   virtual bool m_CheckSysDef(const stw_can::T_STWCAN_Msg_RX & orc_Msg) override;
   virtual bool m_InterpretSysDef(stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) const override;

   virtual bool m_CheckInterpretation(stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) override;
   virtual stw_scl::C_SCLString m_GetProtocolStringHexHook(void) const override;
   virtual stw_scl::C_SCLString m_GetProtocolStringDecHook(void) const override;

private:
   enum E_LoadingActivity
   {
      eNOT_ACTIVE,
      eADD_OSY_SYSDEF_WITHOUT_BUSINDEX,
      eADD_OSY_SYSDEF_WITH_BUSINDEX,
      eADD_DBC_FILE
   };

   stw_types::sint32 m_AddDbcFile(const stw_scl::C_SCLString & orc_PathDbc);
   const C_CieConverter::C_CIECanMessage * m_CheckDbcFile(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   bool m_InterpretDbcFile(const C_CieConverter::C_CIECanMessage * const opc_DbcMessage,
                           stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) const;
   static void mh_InterpretDbcFileCanSignal(stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData,
                                            const C_CieConverter::C_CIECanSignal & orc_DbcSignal);

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   C_SyvComDriverThread * mpc_LoadingThread;
   E_LoadingActivity me_LoadingActivity;

   // Loading execution parameter
   stw_scl::C_SCLString mc_Path;
   stw_types::uint32 mu32_BusIndex;

   // Loading execution result
   std::vector<stw_opensyde_core::C_OSCSystemBus> mc_Busses;
   stw_types::sint32 ms32_Result;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionMsg;
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionConfig;
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionMeta;
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionCounter;

   QList<stw_opensyde_core::C_OSCComMessageLoggerData> mc_ReceivedMessages;

   stw_types::uint8 mu8_BusLoad;
   stw_types::uint32 mu32_TxMessages;
   stw_types::uint32 mu32_TxErrors;

   // DBC files
   std::map<stw_scl::C_SCLString, C_CieConverter::C_CIECommDefinition> mc_DbcFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
