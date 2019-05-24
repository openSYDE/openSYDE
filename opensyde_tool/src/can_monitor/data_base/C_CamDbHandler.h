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
#include "C_CamDbOsy.h"
#include "C_CamDbDbc.h"
#include "C_OSCCanMessage.h"
#include "C_OSCNodeDataPoolList.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   void AddDbcFile(const QString & orc_File, const C_CieConverter::C_CIECommDefinition & orc_Data);
   void AddOsyFile(const QString & orc_File, const stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_Data);

   //Search
   bool GetContainsDbc(const QString & orc_File) const;
   bool GetContainsOsy(const QString & orc_File) const;
   void FindAllMessages(void);
   stw_types::sint32 FindMessageById(const stw_types::uint32 ou32_Id, QString & orc_File, QString & orc_Message) const;
   stw_types::sint32 FindOsyMessage(const QString & orc_File, const QString & orc_Message);
   stw_types::sint32 FindDbcMessage(const QString & orc_File, const QString & orc_Message);

   //Get
   const stw_opensyde_core::C_OSCCanMessage * GetOSCMessage(const QString & orc_File,
                                                            const QString & orc_Message) const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCList(const QString & orc_File,
                                                               const QString & orc_Message) const;
   const C_CieConverter::C_CIECanMessage * GetDbcMessage(const QString & orc_File, const QString & orc_Message) const;
   const QMap<QString, C_CamDbDbc> & GetDBCFiles(void);
   const QMap<QString, C_CamDbOsy> & GetOSYFiles(void);

   //Set
   stw_types::sint32 SetDBCActive(const QString & orc_File, const bool oq_Active);
   stw_types::sint32 SetOsyActive(const QString & orc_File, const bool oq_Active);
   stw_types::sint32 ReplaceOsyBusIndex(const QString & orc_File, const stw_types::uint32 ou32_BusIndex,
                                        bool & orq_Change);

   //Access
   static C_CamDbHandler * h_GetInstance(void);

private:
   QMap<QString, C_CamDbDbc> mc_DBCFiles;
   QMap<QString, C_CamDbOsy> mc_OSYFiles;
   static C_CamDbHandler mhc_Instance;

   C_CamDbHandler(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
