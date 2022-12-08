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

#include "stwtypes.hpp"
#include "stw_can.hpp"

#include "TglTasks.hpp"

#include "C_OscComMessageLogger.hpp"
#include "C_OscComMessageLoggerData.hpp"

#include "C_SyvComDriverThread.hpp"
#include "C_OscSystemBus.hpp"
#include "C_CieConverter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComMessageMonitor :
   public stw::opensyde_core::C_OscComMessageLogger
{
public:
   C_SyvComMessageMonitor(void);
   ~C_SyvComMessageMonitor(void) noexcept override;

   void Start(void) override;
   void Stop(void) override;

   // openSYDE system definition handling
   int32_t StartAddOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition);
   int32_t StartAddOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition, const uint32_t ou32_BusIndex);

   int32_t SetOsySysDefBus(const stw::scl::C_SclString & orc_PathSystemDefinition,
                           const uint32_t ou32_BusIndex) override;
   int32_t GetOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition,
                        stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig & orc_SystemDefinition)
   override;

   // DBC handling
   int32_t StartAddDbcFile(const stw::scl::C_SclString & orc_PathDbc);
   int32_t GetDbcFile(const stw::scl::C_SclString & orc_PathDbc,
                      C_CieConverter::C_CieCommDefinition & orc_DbcDefinition);

   // Generic database handling
   int32_t RemoveDatabase(const stw::scl::C_SclString & orc_Path) override;
   int32_t ActivateDatabase(const stw::scl::C_SclString & orc_Path, const bool oq_Active) override;

   // Logging handling
   int32_t AddLogFileAsc(const stw::scl::C_SclString & orc_FilePath, const bool oq_HexActive,
                         const bool oq_RelativeTimeStampActive) override;
   virtual int32_t AddLogFileBlf(const stw::scl::C_SclString & orc_FilePath);
   int32_t RemoveLogFile(const stw::scl::C_SclString & orc_FilePath) override;
   void RemoveAllLogFiles(void) override;

   // Filter handling
   void AddFilter(const stw::opensyde_core::C_OscComMessageLoggerFilter & orc_Filter) override;
   void RemoveFilter(const stw::opensyde_core::C_OscComMessageLoggerFilter & orc_Filter) override;
   void RemoveAllFilter(void) override;
   uint32_t GetFilteredMessages(void) const override;

   // CAN bus handling
   int32_t HandleCanMessage(const stw::can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx) override;
   void ResetCounter(void) override;
   void UpdateBusLoad(const uint8_t ou8_BusLoad) override;
   void UpdateTxCounter(const uint32_t ou32_TxCount) override;
   void UpdateTxErrors(const uint32_t ou32_TxErrors) override;

   uint8_t GetBusLoad(void) const;
   uint32_t GetTxCount(void) const;
   uint32_t GetTxErrors(void) const;

   int32_t GetResults(int32_t & ors32_Result) const;
   int32_t GetResultBusses(std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses) const;

protected:
   int32_t m_GetCanMessage(stw::opensyde_core::C_OscComMessageLoggerData & orc_Message);

   void m_UpdateProtocolString(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) const;

   bool m_CheckFilter(const stw::can::T_STWCAN_Msg_RX & orc_Msg) override;

   void m_InsertOsySysDef(const stw::scl::C_SclString & orc_PathSystemDefinition,
                          const stw::opensyde_core::C_OscSystemDefinition & orc_OsySysDef,
                          const uint32_t ou32_BusIndex) override;
   bool m_CheckSysDef(const stw::can::T_STWCAN_Msg_RX & orc_Msg) override;
   bool m_InterpretSysDef(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) const override;

   bool m_CheckInterpretation(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) override;
   stw::scl::C_SclString m_GetProtocolStringHexHook(void) const override;
   stw::scl::C_SclString m_GetProtocolStringDecHook(void) const override;

private:
   enum E_LoadingActivity
   {
      eNOT_ACTIVE,
      eADD_OSY_SYSDEF_WITHOUT_BUSINDEX,
      eADD_OSY_SYSDEF_WITH_BUSINDEX,
      eADD_DBC_FILE
   };

   int32_t m_AddDbcFile(const stw::scl::C_SclString & orc_PathDbc);
   const C_CieConverter::C_CieCanMessage * m_CheckDbcFile(const stw::can::T_STWCAN_Msg_RX & orc_Msg);
   bool m_InterpretDbcFile(const C_CieConverter::C_CieCanMessage * const opc_DbcMessage,
                           stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) const;
   static void mh_InterpretDbcFileCanSignal(stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData,
                                            const C_CieConverter::C_CieCanSignal & orc_DbcSignal);

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   C_SyvComDriverThread * mpc_LoadingThread;
   E_LoadingActivity me_LoadingActivity;

   // Loading execution parameter
   stw::scl::C_SclString mc_Path;
   uint32_t mu32_BusIndex;

   // Loading execution result
   std::vector<stw::opensyde_core::C_OscSystemBus> mc_Busses;
   int32_t ms32_Result;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionMsg;
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionConfig;
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionMeta;
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSectionCounter;

   QList<stw::opensyde_core::C_OscComMessageLoggerData> mc_ReceivedMessages;

   uint8_t mu8_BusLoad;
   uint32_t mu32_TxMessages;
   uint32_t mu32_TxErrors;

   // DBC files
   std::map<stw::scl::C_SclString, C_CieConverter::C_CieCommDefinition> mc_DbcFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
