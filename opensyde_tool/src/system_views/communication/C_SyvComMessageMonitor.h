//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class for preparing CAN message data for showing on GUI (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.09.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVCOMMESSAGEMONITOR_H
#define C_SYVCOMMESSAGEMONITOR_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QList>
#include <QVector>

#include "stwtypes.h"
#include "stw_can.h"

#include "TGLTasks.h"

#include "C_OSCComMessageLogger.h"
#include "C_SyvComDriverThread.h"
#include "C_OSCSystemBus.h"
#include "C_CieConverter.h"
#include "C_OSCCanSignal.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvComMessageMonitorDataSignal
{
public:
   C_SyvComMessageMonitorDataSignal(void);

   QString c_Name;
   QString c_Value;
   QString c_Unit;
   QString c_RawValueDec;
   QString c_RawValueHex;
   QString c_Comment;
   bool q_DlcError;

   stw_opensyde_core::C_OSCCanSignal c_OscSignal;
};

class C_SyvComMessageMonitorData
{
public:
   C_SyvComMessageMonitorData(void);

   QString c_TimeStampRelative;
   QString c_TimeStampAbsolute;
   QString c_CanIdDec;
   QString c_CanIdHex;
   QString c_CanDlc;
   QString c_CanDataDec;
   QString c_CanDataHex;
   QString c_Name;
   QString c_ProtocolTextDec;
   QString c_ProtocolTextHex;
   QString c_Counter;

   std::vector<C_SyvComMessageMonitorDataSignal> c_Signals;

   stw_can::T_STWCAN_Msg_RX c_CanMsg;
   bool q_IsTx;
};

class C_SyvComMessageMonitor :
   public stw_opensyde_core::C_OSCComMessageLogger
{
public:
   C_SyvComMessageMonitor(void);
   virtual ~C_SyvComMessageMonitor(void);

   virtual void Stop(void) override;

   // openSYDE system definition handling
   stw_types::sint32 StartAddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                       const stw_scl::C_SCLString & orc_PathDeviceDefinitions);
   stw_types::sint32 StartAddOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                       const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                       const stw_types::uint32 ou32_BusIndex);

   virtual stw_types::sint32 SetOsySysDefBus(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                             const stw_types::uint32 ou32_BusIndex);
   virtual stw_types::sint32 RemoveOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition);

   // DBC handling
   stw_types::sint32 StartAddDbcFile(const stw_scl::C_SCLString & orc_PathDbc);
   stw_types::sint32 RemoveDbcFile(const stw_scl::C_SCLString & orc_PathDbc);

   // Filter handling
   virtual void AddFilter(const stw_opensyde_core::C_OSCComMessageLoggerFilter & orc_Filter) override;
   virtual void RemoveFilter(const stw_opensyde_core::C_OSCComMessageLoggerFilter & orc_Filter) override;

   virtual stw_types::sint32 HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx) override;

   stw_types::sint32 GetResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetResultBusses(std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses) const;

protected:
   stw_types::sint32 m_GetCanMessage(C_SyvComMessageMonitorData & orc_Message);
   void m_GetCanMessages(QVector<C_SyvComMessageMonitorData> & orc_Messages);

   void m_UpdateProtocolString(C_SyvComMessageMonitorData & orc_MessageData) const;

   virtual bool m_CheckFilter(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const override;

   virtual void m_InsertOsySysDef(const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                  const stw_opensyde_core::C_OSCSystemDefinition & orc_OsySysDef,
                                  const stw_types::uint32 ou32_BusIndex) override;
   virtual bool m_CheckSysDef(const stw_can::T_STWCAN_Msg_RX & orc_Msg) override;

   virtual void m_CheckInterpretation(const stw_can::T_STWCAN_Msg_RX & orc_Msg) override;
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

   //Avoid call
   C_SyvComMessageMonitor(const C_SyvComMessageMonitor &);
   C_SyvComMessageMonitor & operator =(const C_SyvComMessageMonitor &);

   bool m_InterpretSysDef(C_SyvComMessageMonitorData & orc_MessageData) const;

   stw_types::sint32 m_AddDbcFile(const stw_scl::C_SCLString & orc_PathDbc);
   void m_CheckDbcFile(const stw_can::T_STWCAN_Msg_RX & orc_Msg);
   bool m_InterpretDbcFile(C_SyvComMessageMonitorData & orc_MessageData) const;

   static void mh_InterpretCanSignalValue(C_SyvComMessageMonitorDataSignal & orc_Signal,
                                          const stw_types::uint8(&orau8_CanDb)[8], const stw_types::uint8 ou8_CanDlc,
                                          const stw_opensyde_core::C_OSCCanSignal & orc_OscSignal,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OscValue,
                                          const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset);

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   C_SyvComDriverThread * mpc_LoadingThread;
   E_LoadingActivity me_LoadingActivity;

   // Loading execution parameter
   stw_scl::C_SCLString mc_Path;
   stw_scl::C_SCLString mc_PathDeviceDefinitions;
   stw_types::uint32 mu32_BusIndex;

   // Loading execution result
   std::vector<stw_opensyde_core::C_OSCSystemBus> mc_Busses;
   stw_types::sint32 ms32_Result;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionMsg;
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSectionConfig;

   QList<C_SyvComMessageMonitorData> mc_ReceivedMessages;
   stw_types::uint64 mu64_LastTimeStamp;

   // DBC files
   std::map<stw_scl::C_SCLString, C_CieConverter::C_CIECommDefinition> mc_DbcFiles;

   // DBC file interpretation check result
   const C_CieConverter::C_CIECanMessage * mpc_DbcMessage;

   // Message counting
   std::vector<stw_types::uint32> mc_MsgCounterStandardId;
   std::map<stw_types::uint32, stw_types::uint32> mc_MsgCounterExtendedId;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
