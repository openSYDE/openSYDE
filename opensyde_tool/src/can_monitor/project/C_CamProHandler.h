//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data handler for can monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROHANDLER_H
#define C_CAMPROHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include <QString>
#include <QObject>

#include "C_CamProMessageData.h"
#include "C_CamProFilterData.h"
#include "C_CamProDatabaseData.h"
#include "C_CamProLoggingData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProHandler :
   public QObject
{
   Q_OBJECT

public:
   enum E_CANDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   //Get
   const std::vector<C_CamProMessageData> & GetMessages(void) const;
   const C_CamProMessageData * GetMessageConst(const stw_types::uint32 ou32_Index) const;
   const QString GetCANDllPath(void) const;
   const QString GetCustomCANDllPath(void) const;
   E_CANDllType GetCANDllType(void) const;
   const std::vector<C_CamProFilterData> & GetFilters(void) const;
   const bool & GetFilterWidgetEnabled(void) const;
   const std::vector<C_CamProDatabaseData> & GetDatabases(void) const;
   const C_CamProLoggingData & GetLoggingData(void) const;

   //Set
   void SetMessages(const std::vector<C_CamProMessageData> & orc_Messages);
   stw_types::sint32 SetMessageName(const stw_types::uint32 ou32_Index, const QString & orc_Name);
   stw_types::sint32 SetMessageUint32Value(const stw_types::uint32 ou32_Index,
                                           const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                                           const stw_types::uint32 ou32_Value);
   stw_types::sint32 SetMessageBoolValue(const stw_types::uint32 ou32_Index,
                                         const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector,
                                         const bool oq_Value);
   stw_types::sint32 SetMessageKey(const stw_types::uint32 ou32_Index, const QString & orc_Key,
                                   const stw_types::uint32 ou32_Offset);
   stw_types::sint32 SetMessageDataBytes(const stw_types::uint32 ou32_Index,
                                         const std::vector<stw_types::uint8> & orc_DataBytes);
   void SetCustomCANDllPath(const QString & orc_CANDllPath);
   void SetCANDllType(const E_CANDllType oe_CANDllType);
   void SetFilters(const std::vector<C_CamProFilterData> & orc_Filters);
   stw_types::sint32 SetFilter(const stw_types::uint32 ou32_Index, const C_CamProFilterData & orc_NewFilter);
   void SetFilterEnabled(const stw_types::uint32 ou32_Index, const bool oq_Enable);
   void SetFilterWidgetEnabled(const bool oq_Enable);
   void SetDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases);
   stw_types::sint32 SetDatabaseName(const stw_types::uint32 ou32_Index, const QString & orc_NewName);
   stw_types::sint32 SetDatabaseOsySysDefBus(const stw_types::uint32 ou32_Index,
                                             const stw_types::sint32 os32_NewBusIndex);
   void SetDatabaseEnabled(const stw_types::uint32 ou32_Index, const bool oq_Enable);
   void SetLoggingData(const C_CamProLoggingData & orc_LoggingData);
   void SetLoggingDirectory(const QString & orc_NewDir);
   void SetLoggingFileName(const QString & orc_NewName);
   void SetLoggingEnabled(const bool oq_Enabled);
   void SetLoggingOverwriteMode(const C_CamProLoggingData::E_OverwriteMode oe_NewOverwriteMode);
   void SetLoggingFormat(const C_CamProLoggingData::E_Format oe_NewFormat);

   //Add
   void InsertMessage(const stw_types::uint32 ou32_Index, const C_CamProMessageData & orc_NewMessage,
                      const bool oq_AllowAdaption);
   void AddFilter(const C_CamProFilterData & orc_NewFilter);
   void AddDatabase(const C_CamProDatabaseData & orc_NewDatabase);

   //Move
   void ReplaceDatabaseName(const QString & orc_PrevName, const QString & orc_NewName);
   stw_types::sint32 MoveMessage(const stw_types::uint32 ou32_StartIndex, const stw_types::uint32 ou32_TargetIndex);

   //Delete
   stw_types::sint32 DeleteMessage(const stw_types::uint32 ou32_Index);
   stw_types::sint32 DeleteFilter(const stw_types::uint32 ou32_Index);
   stw_types::sint32 DeleteDatabase(const stw_types::uint32 ou32_Index);

   //Search
   void GetAllMessagesFromDatabase(const QString & orc_File, QStringList * const opc_CompleteMessageNames,
                                   std::vector<stw_types::uint32> * const opc_MessageIndices) const;
   bool CheckFilterNameAvailable(const QString & orc_Name) const;

   //Utility
   static QString h_GetCompleteMessageName(const C_CamProMessageData & orc_Message);

   //File access
   QString GetCurrentFilePath(void) const;
   QString GetCurrentProjDir(void) const;
   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_Path);
   stw_types::sint32 SaveToFile(const stw_scl::C_SCLString & orc_Path);
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
   stw_types::uint32 mu32_FileHash;

   //Content
   std::vector<C_CamProMessageData> mc_Messages;
   std::vector<C_CamProFilterData> mc_Filters;
   bool mq_FiltersActive;
   std::vector<C_CamProDatabaseData> mc_Databases;
   QString mc_CustomCANDllPath;
   E_CANDllType me_CANDllType;
   C_CamProLoggingData mc_LoggingData;

   C_CamProHandler(void);
   stw_types::uint32 m_GetHash(void) const;
   void m_CalcHash(stw_types::uint32 & oru32_HashValue) const;
   std::map<stw_scl::C_SCLString, bool> m_GetAllMessageNames(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
