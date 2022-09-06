//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen EDS / DCF file handler

   see .cpp file for details

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSDCFH
#define C_OSCCANOPENEDSDCFH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>
#include <map>

#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"
#include "C_OSCCanOpenEdsInfoBlock.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///container for on object dictionary entry
class C_OSCCanOpenObject
{
private:
   stw_types::uint16 mu16_Size; //size in bytes

public:
   //data types according to DS302 V4.02
   static const stw_types::uint8 hu8_DATA_TYPE_INVALID          = 0x00U;
   static const stw_types::uint8 hu8_DATA_TYPE_BOOLEAN          = 0x01U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER8         = 0x02U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER16        = 0x03U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER32        = 0x04U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED8        = 0x05U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED16       = 0x06U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED32       = 0x07U;
   static const stw_types::uint8 hu8_DATA_TYPE_REAL32           = 0x08U;
   static const stw_types::uint8 hu8_DATA_TYPE_VISIBLE_STRING   = 0x09U;
   static const stw_types::uint8 hu8_DATA_TYPE_OCTET_STRING     = 0x0AU;
   static const stw_types::uint8 hu8_DATA_TYPE_UNICODE_STRING   = 0x0BU;
   static const stw_types::uint8 hu8_DATA_TYPE_TIME_OF_DAY      = 0x0CU;
   static const stw_types::uint8 hu8_DATA_TYPE_TIME_DIFFERENCE  = 0x0DU;
   static const stw_types::uint8 hu8_DATA_TYPE_DOMAIN           = 0x0FU;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER24        = 0x10U;
   static const stw_types::uint8 hu8_DATA_TYPE_REAL64           = 0x11U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER40        = 0x12U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER48        = 0x13U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER56        = 0x14U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER64        = 0x15U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED24       = 0x16U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED40       = 0x18U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED48       = 0x19U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED56       = 0x1AU;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED64       = 0x1BU;
   static const stw_types::uint8 hu8_DATA_TYPE_PDO_COMMUNICATION_PARAMETER  = 0x20U;
   static const stw_types::uint8 hu8_DATA_TYPE_PDO_MAPPING      = 0x21U;
   static const stw_types::uint8 hu8_DATA_TYPE_SDO_PARAMETER    = 0x22U;
   static const stw_types::uint8 hu8_DATA_TYPE_IDENTITY         = 0x23U;

   static const stw_types::uint8 hu8_NUM_SUBS_WE_ARE_A_SUB      = 0xFFU;

   stw_scl::C_SCLString c_Name;   //object name
   stw_types::uint16 u16_Index;   //object index
   stw_types::uint8 u8_SubIndex;  //object subindex
   stw_types::uint8 u8_DataType;  //CANopen data type
   stw_scl::C_SCLString c_Access; //access permission ("RW", "RO", "RWW", "CONST", "WO")
   stw_types::uint8 u8_NumSubs;   //number of sub-indexes (0xFF: we are a sub)
   stw_scl::C_SCLString c_DefaultValue;
   stw_scl::C_SCLString c_ParameterValue;
   stw_scl::C_SCLString c_LowLimit;
   stw_scl::C_SCLString c_HighLimit;
   stw_scl::C_SCLString c_Denotation;
   bool q_IsMappableIntoPDO;

   C_OSCCanOpenObject();

   bool IsReadable(void) const;  //do the access rights permit reading ?
   bool IsWriteable(void) const; //do the access rights permit writing ?
   bool IsMappableIntoPDO(void) const;

   bool IsIntegerDataType(void) const;  //INTEGER (signed)
   bool IsUnsignedDataType(void) const; //BOOLEAN, UNSIGNED
   bool IsFloatDataType(void) const;    //REAL32 or REAL64
   bool IsStringDataType(void) const;   //VISIBLE_STRING, UNICODE_STRING

   void DataTypeToTextAndSize(stw_scl::C_SCLString * const opc_Text, stw_types::uint8 * const opu8_Size) const;

   void SetSize(const stw_types::uint16 ou16_Size);
   stw_types::uint16 GetSize(void) const;
   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   //less than operator for sorting:
   bool operator < (const C_OSCCanOpenObject & orc_Object) const;
};

//----------------------------------------------------------------------------------------------------------------------

///container for a CANopen object dictionary
class C_OSCCanOpenObjectDictionary
{
private:
   stw_types::sint32 m_AppendEDSBlock(const stw_scl::C_SCLString & orc_Blockname, stw_scl::C_SCLIniFile & orc_IniFile);
   stw_types::sint32 m_GetObjectDescription(const stw_types::uint16 ou16_Index, const stw_types::uint8 ou8_SubIndex,
                                            const bool oq_IsSubIndex,
                                            const stw_scl::C_SCLStringList & orc_SectionValues,
                                            C_OSCCanOpenObject & orc_Object);

   stw_scl::C_SCLString mc_LastError;

   stw_types::sint32 m_IsSectionRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                   const stw_types::uint8 ou8_OdSubIndex, bool & orq_IsRo) const;
   bool m_DoesSectionExist(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                           const stw_types::uint8 ou8_OdSubIndex) const;

public:
   static const stw_types::uint16 hu16_OD_INDEX_EMCY = 0x1014U;
   static const stw_types::uint16 hu16_OD_INDEX_HEARTBEAT_CONSUMER = 0x1016U;
   static const stw_types::uint16 hu16_OD_INDEX_HEARTBEAT_PRODUCER = 0x1017U;
   static const stw_types::uint16 hu16_OD_INDEX_FIRST_RX_PDO = 0x1400U;
   static const stw_types::uint16 hu16_OD_INDEX_FIRST_TX_PDO = 0x1800U;
   static const stw_types::uint8 hu8_OD_SUB_INDEX_COB_ID = 0x1U;
   static const stw_types::uint8 hu8_OD_SUB_INDEX_TRANSMISSION_TYPE = 0x2U;
   static const stw_types::uint8 hu8_OD_SUB_INDEX_INHIBIT_TIME = 0x3U;
   static const stw_types::uint8 hu8_OD_SUB_INDEX_EVENT_TIMER = 0x5U;
   static const stw_types::uint16 hu16_OD_INDEX_FIRST_RX_PDO_MAPPING = 0x1600U;
   static const stw_types::uint16 hu16_OD_INDEX_FIRST_TX_PDO_MAPPING = 0x1A00U;
   static const stw_types::uint16 hu16_OD_PDO_MAPPING_OFFSET = 0x200U;

   C_OSCCanOpenEdsInfoBlock c_InfoBlock;
   stw_scl::SCLDynamicArray<C_OSCCanOpenObject> c_Objects;

   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_File);
   stw_scl::C_SCLString GetLastErrorText(void) const;
   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   //General
   stw_types::uint8 GetNumHeartbeatConsumers(void) const;
   bool IsHeartbeatProducerSupported(void) const;
   bool IsEMCYSupported(void) const;
   stw_types::uint8 GetGranularity(void) const;
   std::set<stw_types::uint8> GetAllAvailableFactorySettingsSubIndices(void) const;
   std::set<stw_types::uint8> GetAllAvailableSubIndices(const stw_types::uint16 ou16_OdIndex) const;

   //Message
   bool DoesInhibitTimeSectionExist(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx) const;
   bool DoesEventTimerSectionExist(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx) const;
   stw_types::sint32 IsCobIdRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx, bool & orq_IsRo) const;
   stw_types::sint32 IsInhibitTimeRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                     bool & orq_IsRo) const;
   stw_types::sint32 IsEventTimerRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                    bool & orq_IsRo) const;
   stw_types::sint32 IsTransmissionTypeRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                          bool & orq_IsRo) const;
   stw_types::sint32 IsPDOMappingRo(const stw_types::uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                    bool & orq_IsRo) const;

   //Util
   const C_OSCCanOpenObject * GetCanOpenObject(const stw_types::uint16 ou16_OdIndex) const;
   const C_OSCCanOpenObject * GetCanOpenSubIndexObject(const stw_types::uint16 ou16_OdIndex,
                                                       const stw_types::uint8 ou8_OdSubIndex) const;
   bool CheckObjectPresentByIndex(const stw_types::uint16 ou16_OdIndex, const stw_types::uint8 ou8_OdSubIndex) const;
   void GetMappableObjects(std::map<stw_types::uint32, std::vector<stw_types::uint32> > & orc_SubIndices)
   const;

   static stw_types::uint16 h_GetCanOpenObjectDictionaryIndexForPdo(const stw_types::uint16 ou16_PdoIndex,
                                                                    const bool oq_MessageIsTx);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
