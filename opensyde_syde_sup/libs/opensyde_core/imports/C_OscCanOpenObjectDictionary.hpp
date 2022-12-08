//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen EDS / DCF file handler

   see .cpp file for details

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENOBJECTDICTIONARYHPP
#define C_OSCCANOPENOBJECTDICTIONARYHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>
#include <map>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "C_OscCanOpenEdsInfoBlock.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///container for on object dictionary entry
class C_OscCanOpenObject
{
private:
   uint16_t mu16_Size; //size in bytes

public:
   //data types according to DS302 V4.02
   static const uint8_t hu8_DATA_TYPE_INVALID          = 0x00U;
   static const uint8_t hu8_DATA_TYPE_BOOLEAN          = 0x01U;
   static const uint8_t hu8_DATA_TYPE_INTEGER8         = 0x02U;
   static const uint8_t hu8_DATA_TYPE_INTEGER16        = 0x03U;
   static const uint8_t hu8_DATA_TYPE_INTEGER32        = 0x04U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED8        = 0x05U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED16       = 0x06U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED32       = 0x07U;
   static const uint8_t hu8_DATA_TYPE_REAL32           = 0x08U;
   static const uint8_t hu8_DATA_TYPE_VISIBLE_STRING   = 0x09U;
   static const uint8_t hu8_DATA_TYPE_OCTET_STRING     = 0x0AU;
   static const uint8_t hu8_DATA_TYPE_UNICODE_STRING   = 0x0BU;
   static const uint8_t hu8_DATA_TYPE_TIME_OF_DAY      = 0x0CU;
   static const uint8_t hu8_DATA_TYPE_TIME_DIFFERENCE  = 0x0DU;
   static const uint8_t hu8_DATA_TYPE_DOMAIN           = 0x0FU;
   static const uint8_t hu8_DATA_TYPE_INTEGER24        = 0x10U;
   static const uint8_t hu8_DATA_TYPE_REAL64           = 0x11U;
   static const uint8_t hu8_DATA_TYPE_INTEGER40        = 0x12U;
   static const uint8_t hu8_DATA_TYPE_INTEGER48        = 0x13U;
   static const uint8_t hu8_DATA_TYPE_INTEGER56        = 0x14U;
   static const uint8_t hu8_DATA_TYPE_INTEGER64        = 0x15U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED24       = 0x16U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED40       = 0x18U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED48       = 0x19U;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED56       = 0x1AU;
   static const uint8_t hu8_DATA_TYPE_UNSIGNED64       = 0x1BU;
   static const uint8_t hu8_DATA_TYPE_PDO_COMMUNICATION_PARAMETER  = 0x20U;
   static const uint8_t hu8_DATA_TYPE_PDO_MAPPING      = 0x21U;
   static const uint8_t hu8_DATA_TYPE_SDO_PARAMETER    = 0x22U;
   static const uint8_t hu8_DATA_TYPE_IDENTITY         = 0x23U;

   static const uint8_t hu8_NUM_SUBS_WE_ARE_A_SUB      = 0xFFU;

   stw::scl::C_SclString c_Name;   //object name
   uint16_t u16_Index;             //object index
   uint8_t u8_SubIndex;            //object subindex
   uint8_t u8_DataType;            //CANopen data type
   stw::scl::C_SclString c_Access; //access permission ("RW", "RO", "RWW", "CONST", "WO")
   uint8_t u8_NumSubs;             //number of sub-indexes (0xFF: we are a sub)
   stw::scl::C_SclString c_DefaultValue;
   stw::scl::C_SclString c_ParameterValue;
   stw::scl::C_SclString c_LowLimit;
   stw::scl::C_SclString c_HighLimit;
   stw::scl::C_SclString c_Denotation;
   bool q_IsMappableIntoPdo;

   C_OscCanOpenObject();

   bool IsReadable(void) const;  //do the access rights permit reading ?
   bool IsWriteable(void) const; //do the access rights permit writing ?
   bool IsMappableIntoPdo(void) const;
   bool IsMappableIntoTxPdo(void) const;
   bool IsMappableIntoRxPdo(void) const;

   bool IsIntegerDataType(void) const;  //INTEGER (signed)
   bool IsUnsignedDataType(void) const; //BOOLEAN, UNSIGNED
   bool IsFloatDataType(void) const;    //REAL32 or REAL64
   bool IsStringDataType(void) const;   //VISIBLE_STRING, UNICODE_STRING

   void DataTypeToTextAndSize(stw::scl::C_SclString * const opc_Text, uint8_t * const opu8_Size) const;

   void SetSize(const uint16_t ou16_Size);
   uint16_t GetSize(void) const;
   void CalcHash(uint32_t & oru32_HashValue) const;

   //less than operator for sorting:
   bool operator < (const C_OscCanOpenObject & orc_Object) const;
};

//----------------------------------------------------------------------------------------------------------------------

///container for a CANopen object dictionary
class C_OscCanOpenObjectDictionary
{
private:
   int32_t m_AppendEdsBlock(const stw::scl::C_SclString & orc_Blockname, stw::scl::C_SclIniFile & orc_IniFile);
   int32_t m_GetObjectDescription(const uint16_t ou16_Index, const uint8_t ou8_SubIndex, const bool oq_IsSubIndex,
                                  const stw::scl::C_SclStringList & orc_SectionValues, C_OscCanOpenObject & orc_Object);

   stw::scl::C_SclString mc_LastError;

   int32_t m_IsSectionRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, const uint8_t ou8_OdSubIndex,
                         bool & orq_IsRo) const;
   bool m_DoesSectionExist(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, const uint8_t ou8_OdSubIndex) const;

public:
   static const uint16_t hu16_OD_INDEX_SYNC = 0x1005U;
   static const uint16_t hu16_OD_INDEX_CYCLE_PERIOD = 0x1006U;
   static const uint16_t hu16_OD_INDEX_SYNC_WINDOW_LENGTH = 0x1007U;
   static const uint16_t hu16_OD_INDEX_EMCY = 0x1014U;
   static const uint16_t hu16_OD_INDEX_HEARTBEAT_CONSUMER = 0x1016U;
   static const uint16_t hu16_OD_INDEX_HEARTBEAT_PRODUCER = 0x1017U;
   static const uint16_t hu16_OD_INDEX_FIRST_RX_PDO = 0x1400U;
   static const uint16_t hu16_OD_INDEX_FIRST_TX_PDO = 0x1800U;
   static const uint8_t hu8_OD_SUB_INDEX_COB_ID = 0x1U;
   static const uint8_t hu8_OD_SUB_INDEX_TRANSMISSION_TYPE = 0x2U;
   static const uint8_t hu8_OD_SUB_INDEX_INHIBIT_TIME = 0x3U;
   static const uint8_t hu8_OD_SUB_INDEX_EVENT_TIMER = 0x5U;
   static const uint8_t hu8_OD_SUB_INDEX_SYNC_START_VALUE = 0x6U;
   static const uint16_t hu16_OD_INDEX_FIRST_RX_PDO_MAPPING = 0x1600U;
   static const uint16_t hu16_OD_INDEX_FIRST_TX_PDO_MAPPING = 0x1A00U;
   static const uint16_t hu16_OD_PDO_MAPPING_OFFSET = 0x200U;

   C_OscCanOpenEdsInfoBlock c_InfoBlock;
   stw::scl::C_SclDynamicArray<C_OscCanOpenObject> c_Objects;

   int32_t LoadFromFile(const stw::scl::C_SclString & orc_File);
   stw::scl::C_SclString GetLastErrorText(void) const;
   void CalcHash(uint32_t & oru32_HashValue) const;

   //General
   uint8_t GetNumHeartbeatConsumers(void) const;
   int32_t IsHeartbeatConsumerRo(bool & orq_IsRo) const;
   bool IsHeartbeatProducerSupported(void) const;
   int32_t IsHeartbeatProducerRo(bool & orq_IsRo) const;
   bool IsEmcySupported(void) const;
   uint8_t GetGranularity(void) const;
   std::set<uint8_t> GetAllAvailableFactorySettingsSubIndices(void) const;
   std::set<uint8_t> GetAllAvailableSubIndices(const uint16_t ou16_OdIndex) const;

   //Message
   bool DoesInhibitTimeSectionExist(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx) const;
   bool DoesEventTimerSectionExist(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx) const;
   int32_t IsCobIdRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   int32_t IsInhibitTimeRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   int32_t IsEventTimerRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   int32_t IsTransmissionTypeRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   int32_t IsSyncStartRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   int32_t IsPdoMappingRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;

   //Util
   const C_OscCanOpenObject * GetCanOpenObject(const uint16_t ou16_OdIndex) const;
   const C_OscCanOpenObject * GetCanOpenSubIndexObject(const uint16_t ou16_OdIndex, const uint8_t ou8_OdSubIndex) const;
   bool CheckObjectPresentByIndex(const uint16_t ou16_OdIndex, const uint8_t ou8_OdSubIndex) const;
   void GetMappableObjects(std::map<uint32_t, std::vector<uint32_t> > & orc_SubIndices)
   const;

   static uint16_t h_GetCanOpenObjectDictionaryIndexForPdo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
