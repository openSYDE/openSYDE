//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data handler for can monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROHANDLER_HPP
#define C_CAMPROHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include <QString>
#include <QObject>

#include "C_CamProMessageData.hpp"
#include "C_CamProFilterData.hpp"
#include "C_CamProDatabaseData.hpp"
#include "C_CamProLoggingData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProHandler :
   public QObject
{
   Q_OBJECT

public:
   enum E_CanDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   //Get
   const std::vector<C_CamProMessageData> & GetMessages(void) const;
   const C_CamProMessageData * GetMessageConst(const uint32_t ou32_Index) const;
   const bool & GetCyclicMessageTransmitActive(void) const;
   const QString GetCanDllPath(void) const;
   const QString GetCustomCanDllPath(void) const;
   E_CanDllType GetCanDllType(void) const;
   const std::vector<C_CamProFilterData> & GetFilters(void) const;
   const bool & GetFilterWidgetEnabled(void) const;
   const std::vector<C_CamProDatabaseData> & GetDatabases(void) const;
   const C_CamProLoggingData & GetLoggingData(void) const;

   //Set
   void SetMessages(const std::vector<C_CamProMessageData> & orc_Messages);
   int32_t SetMessageName(const uint32_t ou32_Index, const QString & orc_Name);
   int32_t SetMessageUint32Value(const uint32_t ou32_Index,
                                 const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                                 const uint32_t ou32_Value);
   int32_t SetMessageBoolValue(const uint32_t ou32_Index,
                               const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector, const bool oq_Value);
   int32_t SetMessageKey(const uint32_t ou32_Index, const QString & orc_Key, const uint32_t ou32_Offset);
   int32_t SetMessageDataBytes(const uint32_t ou32_Index, const std::vector<uint8_t> & orc_DataBytes);
   void SetCyclicMessageTransmitActive(const bool oq_Active);
   void SetCustomCanDllPath(const QString & orc_CanDllPath);
   void SetCanDllType(const E_CanDllType oe_CanDllType);
   void SetFilters(const std::vector<C_CamProFilterData> & orc_Filters);
   int32_t SetFilter(const uint32_t ou32_Index, const C_CamProFilterData & orc_NewFilter);
   void SetFilterEnabled(const uint32_t ou32_Index, const bool oq_Enable);
   void SetFilterWidgetEnabled(const bool oq_Enable);
   void SetDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases);
   int32_t SetDatabaseName(const uint32_t ou32_Index, const QString & orc_NewName);
   int32_t SetDatabaseOsySysDefBus(const uint32_t ou32_Index, const int32_t os32_NewBusIndex);
   void SetDatabaseEnabled(const uint32_t ou32_Index, const bool oq_Enable);
   void SetLoggingData(const C_CamProLoggingData & orc_LoggingData);
   void SetLoggingDirectory(const QString & orc_NewDir);
   void SetLoggingFileName(const QString & orc_NewName);
   void SetLoggingEnabled(const bool oq_Enabled);
   void SetLoggingOverwriteMode(const C_CamProLoggingData::E_OverwriteMode oe_NewOverwriteMode);
   void SetLoggingFormat(const C_CamProLoggingData::E_Format oe_NewFormat);

   //Add
   void InsertMessage(const uint32_t ou32_Index, const C_CamProMessageData & orc_NewMessage,
                      const bool oq_AllowAdaption);
   void AddFilter(const C_CamProFilterData & orc_NewFilter);
   void AddDatabase(const C_CamProDatabaseData & orc_NewDatabase);

   //Move
   void ReplaceDatabaseName(const QString & orc_PrevName, const QString & orc_NewName);
   int32_t MoveMessage(const uint32_t ou32_StartIndex, const uint32_t ou32_TargetIndex);

   //Delete
   int32_t DeleteMessage(const uint32_t ou32_Index);
   int32_t DeleteFilter(const uint32_t ou32_Index);
   int32_t DeleteDatabase(const uint32_t ou32_Index);

   //Search
   void GetAllMessagesFromDatabase(const QString & orc_File, QStringList * const opc_CompleteMessageNames,
                                   std::vector<uint32_t> * const opc_MessageIndices) const;
   std::vector<uint32_t> GetInvalidMessagesFromDatabase(const QString & orc_File) const;
   std::vector<uint32_t> GetInvalidMessagesWithNoDatabase(void) const;
   bool CheckFilterNameAvailable(const QString & orc_Name) const;

   //Utility
   static QString h_GetCompleteMessageName(const C_CamProMessageData & orc_Message);

   //File access
   QString GetCurrentFilePath(void) const;
   QString GetCurrentProjDir(void) const;
   int32_t LoadFromFile(const stw::scl::C_SclString & orc_Path);
   int32_t SaveToFile(const stw::scl::C_SclString & orc_Path);
   bool CheckChanges(void) const;

   //General
   void Clear(const bool oq_UpdateUserSettings = true);
   static C_CamProHandler * h_GetInstance(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigNewConfiguration(void);
   void SigClearOldConfiguration(void);

private:
   static C_CamProHandler mhc_Instance;

   //File
   QString mc_File;
   uint32_t mu32_FileHash;

   //Content
   std::vector<C_CamProMessageData> mc_Messages;
   bool mq_CyclicMessageTransmitActive;
   std::vector<C_CamProFilterData> mc_Filters;
   bool mq_FiltersActive;
   std::vector<C_CamProDatabaseData> mc_Databases;
   QString mc_CustomCanDllPath;
   E_CanDllType me_CanDllType;
   C_CamProLoggingData mc_LoggingData;

   C_CamProHandler(void);
   uint32_t m_GetHash(void) const;
   void m_CalcHash(uint32_t & oru32_HashValue) const;
   std::map<stw::scl::C_SclString, bool> m_GetAllMessageNames(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
