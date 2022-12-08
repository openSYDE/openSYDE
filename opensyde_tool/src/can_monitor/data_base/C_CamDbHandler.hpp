//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for data base storage (non permanent) and access (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBHANDLER_H
#define C_CAMDBHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "C_CamDbOsy.hpp"
#include "C_CamDbDbc.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscNodeDataPoolList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbHandler
{
public:
   //Delete
   void RemoveAllFiles(void);
   void RemoveDbcFile(const QString & orc_File);
   void RemoveOsyFile(const QString & orc_File);

   //Add
   void AddDbcFile(const QString & orc_File, const C_CieConverter::C_CieCommDefinition & orc_Data);
   void AddOsyFile(const QString & orc_File, const stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig & orc_Data);

   //Search
   bool GetContainsDbc(const QString & orc_File) const;
   bool GetContainsOsy(const QString & orc_File) const;
   void FindAllMessages(void);
   int32_t FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_File,
                           QString & orc_Message) const;
   int32_t FindOsyMessage(const QString & orc_File, const QString & orc_Message);
   int32_t FindDbcMessage(const QString & orc_File, const QString & orc_Message);

   //Get
   const stw::opensyde_core::C_OscCanMessage * GetOscMessage(const QString & orc_File, const QString & orc_Message,
                                                             const bool oq_UseHash, const uint32_t ou32_Hash,
                                                             stw::opensyde_core::C_OscCanProtocol::E_Type * const ope_ProtocolType = NULL)
   const;
   const stw::opensyde_core::C_OscNodeDataPoolList * GetOscList(const QString & orc_File, const QString & orc_Message,
                                                                const bool oq_UseHash, const uint32_t ou32_Hash) const;
   const C_CieConverter::C_CieCanMessage * GetDbcMessage(const QString & orc_File, const QString & orc_Message,
                                                         const bool oq_UseHash, const uint32_t ou32_Hash) const;
   const QMap<QString, C_CamDbDbc> & GetDbcFiles(void) const;
   const QMap<QString, C_CamDbOsy> & GetOsyFiles(void) const;

   //Set
   int32_t SetDbcActive(const QString & orc_File, const bool oq_Active);
   int32_t SetOsyActive(const QString & orc_File, const bool oq_Active);
   int32_t ReplaceOsyBusIndex(const QString & orc_File, const uint32_t ou32_BusIndex, bool & orq_Change);

   //Util
   bool CheckHashForMessage(const QString & orc_File, const QString & orc_Message, const uint32_t ou32_Hash) const;

   //Access
   static C_CamDbHandler * h_GetInstance(void);

private:
   QMap<QString, C_CamDbDbc> mc_DbcFiles;
   QMap<QString, C_CamDbOsy> mc_OsyFiles;
   static C_CamDbHandler mhc_Instance;

   C_CamDbHandler(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
