//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (implementation)

   Handle save'n load for user settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_UsFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::scl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsFiler::C_UsFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user setting to default ini file

   \param[in]  orc_UserSettings     User settings to save
   \param[in]  orc_Path             File path
   \param[in]  orc_ActiveProject    Actual project to save project specific settings
                                    Empty string results in saving no informations

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsFiler::h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path,
                          const QString & orc_ActiveProject)
{
   int32_t s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      s32_Retval = C_NO_ERR;
      {
         //Helper to seperate path and file name
         const QFileInfo c_File(orc_Path);
         //Check if directory exists
         const QDir c_Dir(c_File.path());
         if (c_Dir.exists() == false)
         {
            c_Dir.mkpath(".");
         }
      }
      try
      {
         //Parse ini
         C_SclIniFile c_Ini(orc_Path.toStdString().c_str());
         mh_SaveCommon(orc_UserSettings, c_Ini);
         mh_SaveColors(orc_UserSettings, c_Ini);
         mh_SaveNextRecentColorButtonNumber(orc_UserSettings, c_Ini);
         mh_SaveRecentProjects(orc_UserSettings, c_Ini);
         mh_SaveProjectIndependentSection(orc_UserSettings, c_Ini);
         mh_SaveProjectDependentSection(orc_UserSettings, c_Ini, orc_ActiveProject);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all values of ini file

   If ini not existing set default values.

   \param[in,out]  orc_UserSettings    User settings to load
   \param[in]      orc_Path            File path
   \param[in]      orc_ActiveProject   Actual project to load project specific settings.
                                       Empty string results in default values

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsFiler::h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path, const QString & orc_ActiveProject)
{
   int32_t s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      try
      {
         C_SclIniFile c_Ini(orc_Path.toStdString().c_str());
         s32_Retval = C_NO_ERR;

         orc_UserSettings.SetDefault();

         mh_LoadCommon(orc_UserSettings, c_Ini);
         mh_LoadColors(orc_UserSettings, c_Ini);
         mh_LoadNextRecentColorButtonNumber(orc_UserSettings, c_Ini);
         mh_LoadScreenshotGifSucessTimeout(orc_UserSettings, c_Ini);
         if (orc_ActiveProject == "")
         {
            // load recent projects only if no active project is given
            // (else it was already added to RecentProjects and hence a call to LoadRecentProjects would overwrite it)
            mh_LoadRecentProjects(orc_UserSettings, c_Ini);
         }
         mh_LoadProjectIndependentSection(orc_UserSettings, c_Ini);
         mh_LoadProjectDependentSection(orc_UserSettings, c_Ini, orc_ActiveProject);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node part of user settings

   \param[in,out]  orc_Ini          Ini handler
   \param[in]      orc_SectionName  Section name
   \param[in]      orc_NodeIdBase   Node id base name
   \param[in]      orc_NodeName     Node name
   \param[in]      orc_Node         Node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_NodeIdBase,
                            const QString & orc_NodeName, const C_UsNode & orc_Node)
{
   const QString c_NodeIdName = static_cast<QString>("%1Name").arg(orc_NodeIdBase);
   const QString c_CanOpenOvColumnId = static_cast<QString>("%1CANopenOverview").arg(orc_NodeIdBase);
   const QString c_CanOpenPdoOvColumnId = static_cast<QString>("%1CANopenPdoOverview").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenManager = static_cast<QString>("%1CANopenManager").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceInterfaceNumber =
      static_cast<QString>("%1CANopenDeviceInterfaceNumber").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceNodeName = static_cast<QString>("%1CANopenDeviceNodeName").arg(
      orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceUseCaseIndex = static_cast<QString>("%1CANopenDeviceUseCase").arg(
      orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenManager =
      static_cast<QString>("%1CANopenManagerExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenManagerCounter =
      static_cast<QString>("%1CANopenManagerExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevices =
      static_cast<QString>("%1CANopenDevicesExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevicesCounter =
      static_cast<QString>("%1CANopenDevicesExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevice = static_cast<QString>("%1CANopenDeviceExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDeviceCounter =
      static_cast<QString>("%1CANopenDeviceExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedUseCaseOrInterface =
      static_cast<QString>("%1CANopenSelectedUseCaseOrInterface").arg(orc_NodeIdBase);
   const QString c_HalcOvColumnId = static_cast<QString>("%1HALCOverview").arg(orc_NodeIdBase);
   const QString c_HalcConfigColumnId = static_cast<QString>("%1HALCParamConfig").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedHalcDomain = static_cast<QString>("%1Selected_HALC_domain").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedHalcChannel = static_cast<QString>("%1Selected_HALC_channel").arg(orc_NodeIdBase);
   const QString c_NodeIdDatapoolCount = static_cast<QString>("%1Datapool_count").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedDatapoolName = static_cast<QString>("%1Selected_datapool_name").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedProtocol = static_cast<QString>("%1Selected_protocol").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedInterface = static_cast<QString>("%1Selected_interface").arg(orc_NodeIdBase);
   const QList<QString> c_DatapoolKeyList = orc_Node.GetDatapoolKeysInternal();
   int32_t s32_ItDatapool = 0;

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_NodeIdName.toStdString().c_str(),
                       orc_NodeName.toStdString().c_str());

   //Selected datapool name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_NodeIdSelectedDatapoolName.toStdString().c_str(),
                       orc_Node.GetSelectedDatapoolName().toStdString().c_str());

   //Selected protocol
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_NodeIdSelectedProtocol.toStdString().c_str(),
                        static_cast<int32_t>(orc_Node.GetSelectedProtocol()));

   //Selected interface
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_NodeIdSelectedInterface.toStdString().c_str(),
                        static_cast<int32_t>(orc_Node.GetSelectedInterface()));

   //CANopen colums
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_CanOpenOvColumnId, orc_Node.GetCanOpenOverviewColumnWidth());
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_CanOpenPdoOvColumnId,
                             orc_Node.GetCanOpenPdoOverviewColumnWidth());

   //CANopen
   uint32_t u32_InterfaceCounter = 0UL;
   const std::map<uint8_t, bool> c_Interfaces = orc_Node.GetExpandedCanOpenManager();
   for (std::map<uint8_t, bool>::const_iterator c_ItInterface = c_Interfaces.begin();
        c_ItInterface != c_Interfaces.end(); ++c_ItInterface)
   {
      orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                           (c_CanOpenExpandedCanOpenManager + QString::number(
                               u32_InterfaceCounter) + "InterfaceNumber").toStdString().c_str(),
                           c_ItInterface->first);
      orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                        (c_CanOpenExpandedCanOpenManager + QString::number(
                            u32_InterfaceCounter)).toStdString().c_str(), c_ItInterface->second);
      u32_InterfaceCounter++;
   }
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenExpandedCanOpenManagerCounter.toStdString().c_str(),
                        c_Interfaces.size());
   uint32_t u32_DevicesCounter = 0UL;
   std::map<uint8_t, bool> c_Devices = orc_Node.GetExpandedCanOpenDevices();
   for (std::map<uint8_t, bool>::const_iterator c_ItDevices = c_Devices.begin();
        c_ItDevices != c_Devices.end(); ++c_ItDevices)
   {
      orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                           (c_CanOpenExpandedCanOpenDevices + QString::number(
                               u32_DevicesCounter) + "InterfaceNumber").toStdString().c_str(),
                           c_ItDevices->first);
      orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                        (c_CanOpenExpandedCanOpenDevices + QString::number(
                            u32_DevicesCounter)).toStdString().c_str(), c_ItDevices->second);
      u32_DevicesCounter++;
   }
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenExpandedCanOpenDevicesCounter.toStdString().c_str(),
                        c_Devices.size());
   uint32_t u32_DeviceCounter = 0UL;
   std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >,
            bool> c_Device = orc_Node.GetExpandedCanOpenDevice();
   for (std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >,
                 bool>::const_iterator c_ItDevice = c_Device.begin();
        c_ItDevice != c_Device.end();
        ++c_ItDevice)
   {
      orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                           (c_CanOpenExpandedCanOpenDevice +
                            QString::number(u32_DeviceCounter) + "InterfaceNumber").toStdString().c_str(),
                           c_ItDevice->first.first);
      orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                           (c_CanOpenExpandedCanOpenDevice +
                            QString::number(u32_DeviceCounter) + "DeviceInterfaceNumber").toStdString().c_str(),
                           c_ItDevice->first.second.first);
      orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                          (c_CanOpenExpandedCanOpenDevice +
                           QString::number(u32_DeviceCounter) + "DeviceNodeName").toStdString().c_str(),
                          c_ItDevice->first.second.second.AsStdString()->c_str());
      orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                        (c_CanOpenExpandedCanOpenDevice +
                         QString::number(u32_DeviceCounter)).toStdString().c_str(),
                        c_ItDevice->second);
      u32_DeviceCounter++;
   }
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenExpandedCanOpenDeviceCounter.toStdString().c_str(),
                        c_Device.size());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenSelectedCanOpenManager.toStdString().c_str(),
                        static_cast<int32_t>(orc_Node.GetSelectedCanOpenManager()));
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenSelectedCanOpenDeviceInterfaceNumber.toStdString().c_str(),
                        static_cast<int32_t>(orc_Node.GetSelectedCanOpenDeviceInterfaceNumber()));
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_CanOpenSelectedCanOpenDeviceNodeName.toStdString().c_str(),
                       orc_Node.GetSelectedCanOpenDeviceNodeName().toStdString().c_str());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_CanOpenSelectedCanOpenDeviceUseCaseIndex.toStdString().c_str(),
                        static_cast<int32_t>(orc_Node.GetSelectedCanOpenDeviceUseCaseIndex()));

   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_CanOpenSelectedUseCaseOrInterface.toStdString().c_str(),
                     orc_Node.GetCanOpenSelectedUseCaseOrInterface());

   //Selected HALC domain & channel
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_NodeIdSelectedHalcDomain.toStdString().c_str(),
                       orc_Node.GetSelectedHalcDomainName().toStdString().c_str());

   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_NodeIdSelectedHalcChannel.toStdString().c_str(),
                       orc_Node.GetSelectedHalcChannel().toStdString().c_str());

   // HALC columns
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_HalcOvColumnId, orc_Node.GetHalcOverviewColumnWidth());
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_HalcConfigColumnId, orc_Node.GetHalcConfigColumnWidth());

   //Datapool count
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_NodeIdDatapoolCount.toStdString().c_str(),
                        c_DatapoolKeyList.size());
   for (QList<QString>::const_iterator c_ItDatapoolKey = c_DatapoolKeyList.begin();
        c_ItDatapoolKey != c_DatapoolKeyList.end(); ++c_ItDatapoolKey)
   {
      const QString c_DatapoolIdBase = static_cast<QString>("%1Datapool%2").arg(orc_NodeIdBase).arg(s32_ItDatapool);
      const QString c_DatapoolName = *c_ItDatapoolKey;
      const C_UsNodeDatapool c_Datapool = orc_Node.GetDatapool(c_DatapoolName);
      mh_SaveDatapool(orc_Ini, orc_SectionName, c_DatapoolIdBase, c_DatapoolName, c_Datapool);

      //Important iterator step
      ++s32_ItDatapool;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save bus part of user settings

   \param[in,out]  orc_Ini          Ini handler
   \param[in]      orc_SectionName  Section name
   \param[in]      orc_BusIdBase    Bus id base name
   \param[in]      orc_BusName      Bus name
   \param[in]      orc_Bus          Bus data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveBus(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_BusIdBase,
                           const QString & orc_BusName, const C_UsCommunication & orc_Bus)
{
   const QString c_BusIdName = static_cast<QString>("%1Name").arg(orc_BusIdBase);
   const QString c_BusIdSelectedComProtocol =
      static_cast<QString>("%1Selected_com_protocol").arg(orc_BusIdBase);
   const QString c_BusIdMessageOverview = static_cast<QString>("%1MessageOverview").arg(orc_BusIdBase);
   const QString c_BusIdSignalOverview = static_cast<QString>("%1SignalOverview").arg(orc_BusIdBase);
   const QString c_BusIdMessageSelected = static_cast<QString>("%1Message_selected").arg(orc_BusIdBase);
   const QString c_BusIdSelectedMessageName = static_cast<QString>("%1Selected_message_name").arg(orc_BusIdBase);
   const QString c_BusIdSignalSelected = static_cast<QString>("%1Signal_selected").arg(orc_BusIdBase);
   const QString c_BusIdSelectedSignalName = static_cast<QString>("%1Selected_signal_index").arg(orc_BusIdBase);

   stw::opensyde_core::C_OscCanProtocol::E_Type e_SelectedProtocol;
   bool q_MessageSelected;
   QString c_MessageName;
   bool q_SignalSelected;
   QString c_SignalName;

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_BusIdName.toStdString().c_str(), orc_BusName.toStdString().c_str());

   //Other
   orc_Bus.GetLastSelectedMessage(e_SelectedProtocol, q_MessageSelected, c_MessageName, q_SignalSelected, c_SignalName);

   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_BusIdMessageOverview, orc_Bus.GetMessageOverviewColumnWidth());
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, c_BusIdSignalOverview, orc_Bus.GetSignalOverviewColumnWidth());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_BusIdSelectedComProtocol.toStdString().c_str(),
                        static_cast<int32_t>(e_SelectedProtocol));
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_BusIdMessageSelected.toStdString().c_str(), q_MessageSelected);
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_BusIdSelectedMessageName.toStdString().c_str(),
                       c_MessageName.toStdString().c_str());
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_BusIdSignalSelected.toStdString().c_str(), q_SignalSelected);
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_BusIdSelectedSignalName.toStdString().c_str(),
                       c_SignalName.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save datapool part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_DatapoolIdBase  Node datapool id base name
   \param[in]      orc_DatapoolName    Node datapool name
   \param[in]      orc_Datapool        Node datapool data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveDatapool(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DatapoolIdBase, const QString & orc_DatapoolName,
                                const C_UsNodeDatapool & orc_Datapool)
{
   int32_t s32_ItBus = 0;
   int32_t s32_ItList = 0;
   const QString c_DatapoolIdName = static_cast<QString>("%1Name").arg(orc_DatapoolIdBase);
   const std::vector<QString> & rc_ExpandedListNames = orc_Datapool.GetExpandedListNames();
   const std::vector<QString> & rc_SelectedListNames = orc_Datapool.GetSelectedListNames();
   const std::vector<QString> & rc_SelectedVariableNames = orc_Datapool.GetSelectedVariableNames();
   const QList<QString> c_Interfaces = orc_Datapool.GetInterfaceSettingsKeysInternal();
   const QList<QString> & rc_Lists = orc_Datapool.GetListSettingsKeysInternal();
   const QString c_DatapoolIdExpandedListNameCount =
      static_cast<QString>("%1ExpandedListName_count").arg(orc_DatapoolIdBase);
   const QString c_DatapoolIdSelectedListNameCount =
      static_cast<QString>("%1SelectedListName_count").arg(orc_DatapoolIdBase);
   const QString c_DatapoolIdSelectedVariableNameCount =
      static_cast<QString>("%1SelectedVariableName_count").arg(orc_DatapoolIdBase);
   const QString c_DatapoolIdInterfaceCount =
      static_cast<QString>("%1Interface_count").arg(orc_DatapoolIdBase);
   const QString c_DatapoolIdListCount =
      static_cast<QString>("%1Lists_count").arg(orc_DatapoolIdBase);

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_DatapoolIdName.toStdString().c_str(),
                       orc_DatapoolName.toStdString().c_str());

   //Expanded list names
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DatapoolIdExpandedListNameCount.toStdString().c_str(),
                        rc_ExpandedListNames.size());

   for (uint32_t u32_ItExpandedList = 0; u32_ItExpandedList < rc_ExpandedListNames.size(); ++u32_ItExpandedList)
   {
      const QString c_DatapoolIdExpandedListNameBaseId = static_cast<QString>("%1ExpandedListName%2").arg(
         orc_DatapoolIdBase).arg(
         u32_ItExpandedList);
      const QString c_DatapoolIdExpandedListNameId = static_cast<QString>("%1Name").arg(
         c_DatapoolIdExpandedListNameBaseId);
      orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_DatapoolIdExpandedListNameId.toStdString().c_str(),
                          rc_ExpandedListNames[u32_ItExpandedList].toStdString().c_str());
   }

   //Selected list names
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedListNameCount.toStdString().c_str(),
                        rc_SelectedListNames.size());

   for (uint32_t u32_ItSelectedList = 0; u32_ItSelectedList < rc_SelectedListNames.size(); ++u32_ItSelectedList)
   {
      const QString c_DatapoolIdSelectedListNameBaseId = static_cast<QString>("%1SelectedListName%2").arg(
         orc_DatapoolIdBase).arg(
         u32_ItSelectedList);
      const QString c_DatapoolIdSelectedListNameId = static_cast<QString>("%1Name").arg(
         c_DatapoolIdSelectedListNameBaseId);
      orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedListNameId.toStdString().c_str(),
                          rc_SelectedListNames[u32_ItSelectedList].toStdString().c_str());
   }

   //Selected variable names
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_DatapoolIdSelectedVariableNameCount.toStdString().c_str(),
                        rc_SelectedVariableNames.size());

   for (uint32_t u32_ItSelectedVariable = 0; u32_ItSelectedVariable < rc_SelectedVariableNames.size();
        ++u32_ItSelectedVariable)
   {
      const QString c_DatapoolIdSelectedVariableNameBaseId =
         static_cast<QString>("%1SelectedVariableName%2").arg(orc_DatapoolIdBase).arg(
            u32_ItSelectedVariable);
      const QString c_DatapoolIdSelectedVariableNameId = static_cast<QString>("%1Name").arg(
         c_DatapoolIdSelectedVariableNameBaseId);
      orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                          c_DatapoolIdSelectedVariableNameId.toStdString().c_str(),
                          rc_SelectedVariableNames[u32_ItSelectedVariable].toStdString().c_str());
   }

   //Interfaces
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_DatapoolIdInterfaceCount.toStdString().c_str(), c_Interfaces.size());

   for (QList<QString>::const_iterator c_ItBusKey = c_Interfaces.begin(); c_ItBusKey != c_Interfaces.end();
        ++c_ItBusKey)
   {
      const QString c_BusIdBase = static_cast<QString>("%1Interface%2").arg(orc_DatapoolIdBase).arg(s32_ItBus);
      const C_UsCommunication c_Bus = orc_Datapool.GetCommList(*c_ItBusKey);
      mh_SaveBus(orc_Ini, orc_SectionName, c_BusIdBase, *c_ItBusKey, c_Bus);

      //Important iterator step
      ++s32_ItBus;
   }

   //Lists
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_DatapoolIdListCount.toStdString().c_str(), rc_Lists.size());

   for (QList<QString>::const_iterator c_ItListKey = rc_Lists.begin(); c_ItListKey != rc_Lists.end();
        ++c_ItListKey)
   {
      const QString c_ListIdBase = static_cast<QString>("%1List%2").arg(orc_DatapoolIdBase).arg(s32_ItList);
      const C_UsNodeDatapoolList c_List = orc_Datapool.GetOtherList(*c_ItListKey);
      mh_SaveList(orc_Ini, orc_SectionName, c_ListIdBase, *c_ItListKey, c_List);

      //Important iterator step
      ++s32_ItList;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save node datapool list part of user settings

   \param[in,out]  orc_Ini          Ini handler
   \param[in]      orc_SectionName  Section name
   \param[in]      orc_ListIdBase   List id base name
   \param[in]      orc_ListName     List name
   \param[in]      orc_List         List
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveList(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_ListIdBase,
                            const QString & orc_ListName, const C_UsNodeDatapoolList & orc_List)
{
   const QString c_ListIdName = static_cast<QString>("%1Name").arg(orc_ListIdBase);
   const std::vector<int32_t> & rc_ColumnWidths = orc_List.GetColumnWidths();

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_ListIdName.toStdString().c_str(), orc_ListName.toStdString().c_str());

   //ColumnNumber
   C_UsFiler::mh_SaveColumns(orc_Ini, orc_SectionName, orc_ListIdBase, rc_ColumnWidths);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view part of user settings

   \param[in,out]  orc_Ini          Ini handler
   \param[in]      orc_SectionName  Section name
   \param[in]      orc_ViewIdBase   View id base name
   \param[in]      orc_ViewName     View name
   \param[in]      orc_View         View data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveView(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_ViewIdBase,
                            const QString & orc_ViewName, const C_UsSystemView & orc_View)
{
   const QString c_ViewIdName = static_cast<QString>("%1Name").arg(orc_ViewIdBase);
   const QString c_ViewIdNavigationExpandedStatus =
      static_cast<QString>("%1_navigation_expanded_status").arg(orc_ViewIdBase);
   const QString c_ViewIdNodesCount = static_cast<QString>("%1Node_count").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupPosHorizontal = static_cast<QString>("%1_setup_x").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupPosVertical = static_cast<QString>("%1_setup_y").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupZoom = static_cast<QString>("%1_setup_zoom_value").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePosHorizontal = static_cast<QString>("%1_update_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePosVertical = static_cast<QString>("%1_update_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateZoom = static_cast<QString>("%1_update_zoom_value").arg(orc_ViewIdBase);
   const QString c_ViewIdParamExportPath = static_cast<QString>("%1_param_export_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamImportPath = static_cast<QString>("%1_param_import_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamRecordPath = static_cast<QString>("%1_param_record_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamRecordFileName = static_cast<QString>("%1_param_record_file_name").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateSplitterHorizontal = static_cast<QString>("%1_update_splitter_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateHorizontalSplitterVertical =
      static_cast<QString>("%1_update_horizontal_splitter_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogPositionHorizontal =
      static_cast<QString>("%1_update_progress_log_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogPositionVertical =
      static_cast<QString>("%1_update_progress_log_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogSizeWidth =
      static_cast<QString>("%1_update_progress_log_width").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogSizeHeight =
      static_cast<QString>("%1_update_progress_log_height").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogIsMaximized =
      static_cast<QString>("%1_update_progress_log_is_maximized").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateSummaryBig = static_cast<QString>("%1_update_summary_is_type_big").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateEmptyOptionalSectionsVisible =
      static_cast<QString>("%1_empty_optional_sections_visible").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxPositionHorizontal = static_cast<QString>("%1_toolbox_x").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxPositionVertical = static_cast<QString>("%1_toolbox_y").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxSizeWidth = static_cast<QString>("%1_toolbox_width").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxSizeHeight = static_cast<QString>("%1_toolbox_height").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxIsMaximized = static_cast<QString>("%1_toolbox_is_maximized").arg(
      orc_ViewIdBase);
   const QString c_ViewIdDashboardSelectedTabIndex = static_cast<QString>("%1_selected_tab_index").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardCount = static_cast<QString>("%1Dashboard_count").arg(orc_ViewIdBase);
   const QList<QString> & rc_DashboardKeyList = orc_View.GetDashboardKeysInternal();
   const QList<QString> & rc_NodesKeyList = orc_View.GetViewNodesKeysInternal();
   const QString c_ViewIdSetupPermission = static_cast<QString>("%1_setup_permission").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePermission = static_cast<QString>("%1_update_permission").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardPermission = static_cast<QString>("%1_dashboard_permission").arg(orc_ViewIdBase);
   int32_t s32_Iterator;

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_ViewIdName.toStdString().c_str(),
                       orc_ViewName.toStdString().c_str());
   //Navigation
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(), c_ViewIdNavigationExpandedStatus.toStdString().c_str(),
                     orc_View.GetNavigationExpandedStatus());
   //Setup pos
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdSetupPosHorizontal.toStdString().c_str(),
                        orc_View.c_SetupViewPos.x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdSetupPosVertical.toStdString().c_str(),
                        orc_View.c_SetupViewPos.y());
   //Setup zoom
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdSetupZoom.toStdString().c_str(),
                        orc_View.s32_SetupViewZoom);
   //Update pos
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdUpdatePosHorizontal.toStdString().c_str(),
                        orc_View.c_UpdateViewPos.x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdUpdatePosVertical.toStdString().c_str(),
                        orc_View.c_UpdateViewPos.y());
   //Update zoom
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdUpdateZoom.toStdString().c_str(),
                        orc_View.s32_UpdateViewZoom);

   //Param
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_ViewIdParamExportPath.toStdString().c_str(),
                       orc_View.c_ParamExportPath.toStdString().c_str());
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_ViewIdParamImportPath.toStdString().c_str(),
                       orc_View.c_ParamImportPath.toStdString().c_str());
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_ViewIdParamRecordPath.toStdString().c_str(),
                       orc_View.c_ParamRecordPath.toStdString().c_str());
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_ViewIdParamRecordFileName.toStdString().c_str(),
                       orc_View.c_ParamRecordFileName.toStdString().c_str());

   //Splitter
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdUpdateSplitterHorizontal.toStdString().c_str(),
                        orc_View.GetUpdateSplitterHorizontal());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdUpdateHorizontalSplitterVertical.toStdString().c_str(),
                        orc_View.GetUpdateHorizontalSplitterVertical());

   //Progress log
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdUpdateProgressLogPositionHorizontal.toStdString().c_str(),
                        orc_View.GetUpdateProgressLogPos().x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdUpdateProgressLogPositionVertical.toStdString().c_str(),
                        orc_View.GetUpdateProgressLogPos().y());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdUpdateProgressLogSizeWidth.toStdString().c_str(),
                        orc_View.GetUpdateProgressLogSize().width());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdUpdateProgressLogSizeHeight.toStdString().c_str(),
                        orc_View.GetUpdateProgressLogSize().height());
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdUpdateProgressLogIsMaximized.toStdString().c_str(),
                     orc_View.GetUpdateProgressLogMaximized());

   //Update summary style
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(), c_ViewIdUpdateSummaryBig.toStdString().c_str(),
                     orc_View.GetUpdateSummaryBig());

   // Update package sections visibility of empty optional sections
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdUpdateEmptyOptionalSectionsVisible.toStdString().c_str(),
                     orc_View.GetUpdatePackEmptyOptionalSectionsVisible());

   // View nodes
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdNodesCount.toStdString().c_str(),
                        rc_NodesKeyList.size());
   s32_Iterator = 0;
   for (QList<QString>::const_iterator c_ItNodesKey = rc_NodesKeyList.begin(); c_ItNodesKey != rc_NodesKeyList.end();
        ++c_ItNodesKey)
   {
      const QString c_NodeIdBase = static_cast<QString>("%1Node%2").arg(orc_ViewIdBase).arg(s32_Iterator);
      const C_UsSystemViewNode c_Node = orc_View.GetSvNode(*c_ItNodesKey);
      mh_SaveViewNode(orc_Ini, orc_SectionName, c_NodeIdBase, *c_ItNodesKey, c_Node);

      //Important iterator step
      ++s32_Iterator;
   }

   //Toolbox
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdDashboardToolboxPositionHorizontal.toStdString().c_str(),
                        orc_View.GetDashboardToolboxPos().x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdDashboardToolboxPositionVertical.toStdString().c_str(),
                        orc_View.GetDashboardToolboxPos().y());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdDashboardToolboxSizeWidth.toStdString().c_str(),
                        orc_View.GetDashboardToolboxSize().width());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdDashboardToolboxSizeHeight.toStdString().c_str(),
                        orc_View.GetDashboardToolboxSize().height());
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdDashboardToolboxIsMaximized.toStdString().c_str(),
                     orc_View.GetDashboardToolboxMaximized());

   //General
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_ViewIdDashboardSelectedTabIndex.toStdString().c_str(),
                        orc_View.GetDashboardSelectedTabIndex());

   //Dashboard count
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_ViewIdDashboardCount.toStdString().c_str(),
                        rc_DashboardKeyList.size());
   s32_Iterator = 0;
   for (QList<QString>::const_iterator c_ItDashboardKey = rc_DashboardKeyList.begin();
        c_ItDashboardKey != rc_DashboardKeyList.end(); ++c_ItDashboardKey)
   {
      const QString c_DashboardIdBase = static_cast<QString>("%1Dashboard%2").arg(orc_ViewIdBase).arg(s32_Iterator);
      const QString c_DashboardName = *c_ItDashboardKey;
      const C_UsSystemViewDashboard c_Dashboard = orc_View.GetDashboardSettings(c_DashboardName);
      mh_SaveDashboard(orc_Ini, orc_SectionName, c_DashboardIdBase, c_DashboardName, c_Dashboard);

      //Important iterator step
      ++s32_Iterator;
   }

   //Permissions
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdSetupPermission.toStdString().c_str(), orc_View.GetSetupPermission());
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdUpdatePermission.toStdString().c_str(), orc_View.GetUpdatePermission());
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_ViewIdDashboardPermission.toStdString().c_str(), orc_View.GetDashboardPermission());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view update data rates per node part of user settings

   \param[in,out]  orc_Ini                      Ini handler
   \param[in]      orc_SectionName              Section name
   \param[in]      orc_DataRatePerNodeIdBase    View data rate id base name
   \param[in]      orc_Node                     Node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveDataRatesPerNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                        const QString & orc_DataRatePerNodeIdBase, const C_UsSystemViewNode & orc_Node)
{
   int32_t s32_ItDataRate = 0;
   const QString c_DataRateIdCount = static_cast<QString>("%1_count").arg(orc_DataRatePerNodeIdBase);
   const QMap<uint32_t, float64_t > & rc_UpdateDataRateHistory = orc_Node.GetUpdateDataRateHistory();

   //Data rate count
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DataRateIdCount.toStdString().c_str(),
                        rc_UpdateDataRateHistory.size());
   //Per checksum section
   for (QMap<uint32_t, float64_t >::const_iterator c_ItDataRateKey = rc_UpdateDataRateHistory.begin();
        c_ItDataRateKey != rc_UpdateDataRateHistory.end(); ++c_ItDataRateKey)
   {
      const QString c_DataRateIdBase =
         static_cast<QString>("%1DataRate%2").arg(orc_DataRatePerNodeIdBase).arg(s32_ItDataRate);
      const QString c_DataRateIdChecksum = static_cast<QString>("%1_checksum").arg(c_DataRateIdBase);
      const QString c_DataRateIdCurrentValue = static_cast<QString>("%1_value").arg(c_DataRateIdBase);

      //Key
      orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_DataRateIdChecksum.toStdString().c_str(),
                          QString::number(c_ItDataRateKey.key()).toStdString().c_str());
      //Value count
      orc_Ini.WriteFloat(orc_SectionName.toStdString().c_str(), c_DataRateIdCurrentValue.toStdString().c_str(),
                         c_ItDataRateKey.value());

      //Important iterator step
      ++s32_ItDataRate;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save view node part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_ViewNodeIdBase  View node ID base name
   \param[in]      orc_NodeName        View node name
   \param[in]      orc_ViewNode        View node data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveViewNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_ViewNodeIdBase, const QString & orc_NodeName,
                                const C_UsSystemViewNode & orc_ViewNode)
{
   const QString c_NodeIdName = static_cast<QString>("%1Name").arg(orc_ViewNodeIdBase);
   const QString c_NodeIdUpdateDataRateBaseId = static_cast<QString>("%1_update_data_rate").arg(orc_ViewNodeIdBase);
   const QVector<bool> & rc_ExpandedFlags = orc_ViewNode.GetSectionsExpanded();
   int32_t s32_SectionCounter;

   QString c_NodeIdExpandedFlag;

   // Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(),
                       c_NodeIdName.toStdString().c_str(), orc_NodeName.toStdString().c_str());

   //Section expanded flags
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        static_cast<QString>("%1SectionCount").arg(orc_ViewNodeIdBase).toStdString().c_str(),
                        rc_ExpandedFlags.size());
   for (s32_SectionCounter = 0; s32_SectionCounter < rc_ExpandedFlags.size(); ++s32_SectionCounter)
   {
      c_NodeIdExpandedFlag = static_cast<QString>("%1Section%2").arg(orc_ViewNodeIdBase).arg(s32_SectionCounter);

      orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                        c_NodeIdExpandedFlag.toStdString().c_str(),
                        rc_ExpandedFlags.at(s32_SectionCounter));
   }

   //Data rate
   C_UsFiler::mh_SaveDataRatesPerNode(orc_Ini, orc_SectionName, c_NodeIdUpdateDataRateBaseId, orc_ViewNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save view dashboard part of user settings

   \param[in,out]  orc_Ini                Ini handler
   \param[in]      orc_SectionName        Section name
   \param[in]      orc_DashboardIdBase    View dashboard id base name
   \param[in]      orc_DashboardName      View dashboard name
   \param[in]      orc_Dashboard          View dashboard data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveDashboard(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                 const QString & orc_DashboardIdBase, const QString & orc_DashboardName,
                                 const C_UsSystemViewDashboard & orc_Dashboard)
{
   const QString c_DashboardIdName = static_cast<QString>("%1Name").arg(orc_DashboardIdBase);
   const QString c_DashboardIdWindowPosHorizontal = static_cast<QString>("%1_window_x").arg(orc_DashboardIdBase);
   const QString c_DashboardIdWindowPosVertical = static_cast<QString>("%1_window_y").arg(orc_DashboardIdBase);
   const QString c_DashboardIdSizeWidth = static_cast<QString>("%1_width").arg(orc_DashboardIdBase);
   const QString c_DashboardIdSizeHeight = static_cast<QString>("%1_height").arg(orc_DashboardIdBase);
   const QString c_DashboardIdTornOffFlag = static_cast<QString>("%1_torn_off_flag").arg(orc_DashboardIdBase);
   const QString c_DashboardIdMinFlag = static_cast<QString>("%1_min_flag").arg(orc_DashboardIdBase);
   const QString c_DashboardIdMaxFlag = static_cast<QString>("%1_max_flag").arg(orc_DashboardIdBase);
   const QString c_DashboardIdScenePosHorizontal = static_cast<QString>("%1_scene_x").arg(orc_DashboardIdBase);
   const QString c_DashboardIdScenePosVertical = static_cast<QString>("%1_scene_y").arg(orc_DashboardIdBase);
   const QString c_DashboardIdSceneZoom = static_cast<QString>("%1_scene_zoom").arg(orc_DashboardIdBase);

   //Name
   orc_Ini.WriteString(orc_SectionName.toStdString().c_str(), c_DashboardIdName.toStdString().c_str(),
                       orc_DashboardName.toStdString().c_str());
   //Torn off flag
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_DashboardIdTornOffFlag.toStdString().c_str(),
                     orc_Dashboard.q_TornOff);
   //Window pos
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DashboardIdWindowPosHorizontal.toStdString().c_str(),
                        orc_Dashboard.c_TornOffWindowPosition.x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DashboardIdWindowPosVertical.toStdString().c_str(),
                        orc_Dashboard.c_TornOffWindowPosition.y());
   //Size
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_DashboardIdSizeWidth.toStdString().c_str(),
                        orc_Dashboard.c_TornOffWindowSize.width());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_DashboardIdSizeHeight.toStdString().c_str(),
                        orc_Dashboard.c_TornOffWindowSize.height());
   //Flags
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_DashboardIdMinFlag.toStdString().c_str(), orc_Dashboard.q_TornOffWindowMinimized);
   orc_Ini.WriteBool(orc_SectionName.toStdString().c_str(),
                     c_DashboardIdMaxFlag.toStdString().c_str(), orc_Dashboard.q_TornOffWindowMaximized);

   //Scene pos
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DashboardIdScenePosHorizontal.toStdString().c_str(),
                        orc_Dashboard.c_ScenePos.x());
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DashboardIdScenePosVertical.toStdString().c_str(),
                        orc_Dashboard.c_ScenePos.y());
   //Zoom
   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_DashboardIdSceneZoom.toStdString().c_str(),
                        orc_Dashboard.s32_SceneZoom);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save recent languages part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveCommon(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   //Language
   orc_Ini.WriteString("Common", "Language", orc_UserSettings.GetLanguage().toStdString().c_str());

   //Save As
   orc_Ini.WriteString("Common", "SaveAsLocation", orc_UserSettings.GetCurrentSaveAsPath().toStdString().c_str());

   // Performance measurement
   orc_Ini.WriteBool("Common", "PerformanceMeasurementActive", orc_UserSettings.GetPerformanceActive());

   // TSP shortcut
   orc_Ini.WriteBool("Common", "TSPShortcutActive", orc_UserSettings.GetTspShortcutActive());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save recent colors part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveColors(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   // Colors
   int32_t s32_Counter = 0;

   QVector<QColor> c_RecentColors = orc_UserSettings.GetRecentColors();
   QVector<QColor>::const_iterator pc_ItColor;
   for (pc_ItColor = c_RecentColors.begin(); pc_ItColor != c_RecentColors.end(); ++pc_ItColor)
   {
      ++s32_Counter;

      orc_Ini.WriteInteger("RecentColors", "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                           "_Red", pc_ItColor->red());
      orc_Ini.WriteInteger("RecentColors", "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                           "_Green", pc_ItColor->green());
      orc_Ini.WriteInteger("RecentColors", "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                           "_Blue", pc_ItColor->blue());
      orc_Ini.WriteInteger("RecentColors", "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                           "_Alpha", pc_ItColor->alpha());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save next recent color button number part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveNextRecentColorButtonNumber(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   // Next recent color button number
   orc_Ini.WriteInteger("RecentColors", "NextRecentColorButtonNumber",
                        orc_UserSettings.GetNextRecentColorButtonNumber());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save recent projects part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveRecentProjects(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   const QStringList c_List = orc_UserSettings.GetRecentProjects();

   // clear recent projects section (the ini file can only add keys and does not delete keys that do not exist anymore)
   if (orc_Ini.SectionExists("RecentProjects") == true)
   {
      orc_Ini.EraseSection("RecentProjects");
   }

   //Recent projects
   for (uint8_t u8_It = 0; u8_It < c_List.count(); ++u8_It)
   {
      orc_Ini.WriteString("RecentProjects", C_SclString::IntToStr(
                             u8_It), c_List.at(u8_It).toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project independent part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   //Screen position
   orc_Ini.WriteInteger("Screen", "Position_x", orc_UserSettings.GetScreenPos().x());
   orc_Ini.WriteInteger("Screen", "Position_y", orc_UserSettings.GetScreenPos().y());

   // Application size
   orc_Ini.WriteInteger("Screen", "Size_width", orc_UserSettings.GetAppSize().width());
   orc_Ini.WriteInteger("Screen", "Size_height", orc_UserSettings.GetAppSize().height());

   // Application maximizing flag
   orc_Ini.WriteBool("Screen", "Size_maximized", orc_UserSettings.GetAppMaximized());

   // Sys def topology toolbox position
   orc_Ini.WriteInteger("SdTopologyToolbox", "Position_x", orc_UserSettings.GetSdTopologyToolboxPos().x());
   orc_Ini.WriteInteger("SdTopologyToolbox", "Position_y", orc_UserSettings.GetSdTopologyToolboxPos().y());

   // Sys def topology toolbox size
   orc_Ini.WriteInteger("SdTopologyToolbox", "Size_width", orc_UserSettings.GetSdTopologyToolboxSize().width());
   orc_Ini.WriteInteger("SdTopologyToolbox", "Size_height",
                        orc_UserSettings.GetSdTopologyToolboxSize().height());

   // Sys def topology toolbox maximizing flag
   orc_Ini.WriteBool("SdTopologyToolbox", "Size_maximized", orc_UserSettings.GetSdTopologyToolboxMaximized());

   // Sys def node edit splitter
   orc_Ini.WriteInteger("SdNodeEdit", "SplitterX", orc_UserSettings.GetSdNodeEditSplitterHorizontal());

   // Sys def node edit HALC splitter
   orc_Ini.WriteInteger("SdNodeEdit", "HalcSplitterX", orc_UserSettings.GetSdNodeEditHalcSplitterHorizontal());
   // Sys def node edit HALC splitter
   orc_Ini.WriteInteger("SdNodeEdit", "CoManagerSplitterX",
                        orc_UserSettings.GetSdNodeEditCoManagerSplitterHorizontal());

   // Sys def bus edit splitters
   orc_Ini.WriteInteger("SdBusEdit", "TreeSplitterX", orc_UserSettings.GetSdBusEditTreeSplitterHorizontal());
   orc_Ini.WriteInteger("SdBusEdit", "TreeSplitterX2", orc_UserSettings.GetSdBusEditTreeSplitterHorizontal2());
   orc_Ini.WriteInteger("SdBusEdit", "LayoutSplitterX", orc_UserSettings.GetSdBusEditLayoutSplitterHorizontal());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project dependent part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_ActiveProject   Actual project to save project specific settings
                                       Empty string results in saving no informations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveProjectDependentSection(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini,
                                               const QString & orc_ActiveProject)
{
   if (orc_ActiveProject != "")
   {
      int32_t s32_ItBus = 0;
      int32_t s32_ItNode = 0;
      int32_t s32_ItView = 0;
      const QList<QString> c_BusKeyList = orc_UserSettings.GetProjSdBusKeysInternal();
      const QList<QString> c_NodeKeyList = orc_UserSettings.GetProjSdNodeKeysInternal();
      const QList<QString> c_ViewKeyList = orc_UserSettings.GetProjSvSetupViewKeysInternal();
      int32_t s32_SysDefSubMode;
      uint32_t u32_SysDefIndex;
      uint32_t u32_SysDefFlag;
      int32_t s32_SysViewSubMode;
      uint32_t u32_SysViewIndex;
      uint32_t u32_SysViewFlag;

      // project specific settings
      // Mode
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjMode", orc_UserSettings.GetProjLastMode());

      // Navi bar
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "navigation-width",
                           orc_UserSettings.GetNaviBarSize());
      orc_Ini.WriteInteger(
         orc_ActiveProject.toStdString().c_str(), "navigation-node-section-width",
         orc_UserSettings.GetNaviBarNodeSectionSize());

      // Sys def topology view port position
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyView_x",
                           orc_UserSettings.GetProjSdTopologyViewPos().x());
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyView_y",
                           orc_UserSettings.GetProjSdTopologyViewPos().y());
      // Sys def topology view zoom value
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyViewZoom_value",
                           orc_UserSettings.GetProjSdTopologyViewZoom());

      // Last screen mode
      orc_UserSettings.GetProjLastScreenMode(s32_SysDefSubMode, u32_SysDefIndex, u32_SysDefFlag,
                                             s32_SysViewSubMode, u32_SysViewIndex, u32_SysViewFlag);

      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubMode_value",
                           static_cast<int32_t>(s32_SysDefSubMode));
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubIndex_value",
                           static_cast<int32_t>(u32_SysDefIndex));
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubFlag_value",
                           static_cast<int32_t>(u32_SysDefFlag));
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubMode_value",
                           static_cast<int32_t>(s32_SysViewSubMode));
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubIndex_value",
                           static_cast<int32_t>(u32_SysViewIndex));
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubFlag_value",
                           static_cast<int32_t>(u32_SysViewFlag));

      //TSP
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_tsp_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownTspPath().toStdString().c_str());

      //File generation
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_code_export_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownCodeExportPath().toStdString().c_str());

      //Import
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_import_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownImportPath().toStdString().c_str());

      //Import CANopen
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_CANopen_EDS_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownCanOpenEdsPath().toStdString().c_str());

      //Export
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_export_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownExportPath().toStdString().c_str());

      //Last path from where a .syde_devdef file was loaded
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_device_definition_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownDeviceDefPath().toStdString().c_str());

      //RTF File Export
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownRtfPath().toStdString().c_str());
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_company_name",
                          orc_UserSettings.GetProjSdTopologyLastKnownRtfCompanyName().toStdString().c_str());
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_company_logo_path",
                          orc_UserSettings.GetProjSdTopologyLastKnownRtfCompanyLogoPath().toStdString().c_str());

      //HALC Paths
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_def_path",
                          orc_UserSettings.GetLastKnownHalcDefPath().toStdString().c_str());
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_import_path",
                          orc_UserSettings.GetLastKnownHalcImportPath().toStdString().c_str());
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_export_path",
                          orc_UserSettings.GetLastKnownHalcExportPath().toStdString().c_str());
      //Service Project Path
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_service_project_path",
                          orc_UserSettings.GetLastKnownServiceProjectPath().toStdString().c_str());
      //RAMView Project Import Path
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_ramview_project_path",
                          orc_UserSettings.GetLastKnownRamViewProjectPath().toStdString().c_str());

      // J1939 Catalog Path
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_j1939_catalog_path",
                          orc_UserSettings.GetLastKnownJ1939CatalogPath().toStdString().c_str());

      // CSV Export Path
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_csv_export_path",
                          orc_UserSettings.GetLastKnownCsvExportPath().toStdString().c_str());

      // Last tab index in system definition
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdNodeEditTabIndex_value",
                           orc_UserSettings.GetProjLastSysDefNodeTabIndex());
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdBusEditTabIndex_value",
                           orc_UserSettings.GetProjLastSysDefBusTabIndex());

      //System definition
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdNode_count", c_NodeKeyList.size());

      for (QList<QString>::const_iterator c_ItNodeKey = c_NodeKeyList.begin(); c_ItNodeKey != c_NodeKeyList.end();
           ++c_ItNodeKey)
      {
         const QString c_NodeIdBase = static_cast<QString>("SdNode%1").arg(s32_ItNode);
         const QString c_NodeName = *c_ItNodeKey;
         const C_UsNode c_Node = orc_UserSettings.GetProjSdNode(c_NodeName);
         mh_SaveNode(orc_Ini, orc_ActiveProject, c_NodeIdBase, c_NodeName, c_Node);

         //Important iterator step
         ++s32_ItNode;
      }

      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdBus_count", c_BusKeyList.size());

      for (QList<QString>::const_iterator c_ItBusKey = c_BusKeyList.begin(); c_ItBusKey != c_BusKeyList.end();
           ++c_ItBusKey)
      {
         const QString c_BusIdBase = static_cast<QString>("SdBus%1").arg(s32_ItBus);
         const QString c_BusName = *c_ItBusKey;
         const C_UsCommunication c_Bus = orc_UserSettings.GetProjSdBus(c_BusName);
         mh_SaveBus(orc_Ini, orc_ActiveProject, c_BusIdBase, c_BusName, c_Bus);

         //Important iterator step
         ++s32_ItBus;
      }

      //System views
      orc_Ini.WriteInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSetupView_count", c_ViewKeyList.size());

      for (QList<QString>::const_iterator c_ItViewKey = c_ViewKeyList.begin(); c_ItViewKey != c_ViewKeyList.end();
           ++c_ItViewKey)
      {
         const QString c_ViewIdBase = static_cast<QString>("SvSetupView%1").arg(s32_ItView);
         const QString c_ViewName = *c_ItViewKey;
         const C_UsSystemView c_View = orc_UserSettings.GetProjSvSetupView(c_ViewName);
         mh_SaveView(orc_Ini, orc_ActiveProject, c_ViewIdBase, c_ViewName, c_View);

         //Important iterator step
         ++s32_ItView;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_NodeIdBase      Node id base name
   \param[in]      orc_NodeName        Node name
   \param[in,out]  orc_UserSettings    User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_NodeIdBase,
                            const QString & orc_NodeName, C_UsHandler & orc_UserSettings)
{
   QString c_Tmp;

   stw::opensyde_core::C_OscCanProtocol::E_Type e_Tmp;
   uint32_t u32_Tmp;
   std::vector<int32_t> c_Columns;

   const QString c_CanOpenOvColumnId = static_cast<QString>("%1CANopenOverview").arg(orc_NodeIdBase);
   const QString c_CanOpenPdoOvColumnId = static_cast<QString>("%1CANopenPdoOverview").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenManager = static_cast<QString>("%1CANopenManager").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceInterfaceNumber =
      static_cast<QString>("%1CANopenDeviceInterfaceNumber").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceNodeName = static_cast<QString>("%1CANopenDeviceNodeName").arg(
      orc_NodeIdBase);
   const QString c_CanOpenSelectedCanOpenDeviceUseCaseIndex = static_cast<QString>("%1CANopenDeviceUseCase").arg(
      orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenManager =
      static_cast<QString>("%1CANopenManagerExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenManagerCounter =
      static_cast<QString>("%1CANopenManagerExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevices =
      static_cast<QString>("%1CANopenDevicesExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevicesCounter =
      static_cast<QString>("%1CANopenDevicesExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDevice =
      static_cast<QString>("%1CANopenDeviceExpanded#").arg(orc_NodeIdBase);
   const QString c_CanOpenExpandedCanOpenDeviceCounter =
      static_cast<QString>("%1CANopenDeviceExpandedCounter").arg(orc_NodeIdBase);
   const QString c_CanOpenSelectedUseCaseOrInterface =
      static_cast<QString>("%1CANopenSelectedUseCaseOrInterface").arg(orc_NodeIdBase);
   const QString c_HalcOvColumnId = static_cast<QString>("%1HALCOverview").arg(orc_NodeIdBase);
   const QString c_HalcConfigColumnId = static_cast<QString>("%1HALCParamConfig").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedHalcDomain = static_cast<QString>("%1Selected_HALC_domain").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedHalcChannel = static_cast<QString>("%1Selected_HALC_channel").arg(orc_NodeIdBase);
   const QString c_NodeIdDatapoolCount = static_cast<QString>("%1Datapool_count").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedDatapoolName = static_cast<QString>("%1Selected_datapool_name").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedProtocol = static_cast<QString>("%1Selected_protocol").arg(orc_NodeIdBase);
   const QString c_NodeIdSelectedInterface = static_cast<QString>("%1Selected_interface").arg(orc_NodeIdBase);

   //Selected datapool name
   c_Tmp = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                              c_NodeIdSelectedDatapoolName.toStdString().c_str(), "").c_str();
   orc_UserSettings.SetProjSdNodeSelectedDatapoolName(orc_NodeName, c_Tmp);
   //Selected protocol
   e_Tmp = static_cast<stw::opensyde_core::C_OscCanProtocol::E_Type>(
      orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(), c_NodeIdSelectedProtocol.toStdString().c_str(), 0));
   orc_UserSettings.SetProjSdNodeSelectedProtocol(orc_NodeName, e_Tmp);
   //Selected interface
   u32_Tmp = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                 c_NodeIdSelectedInterface.toStdString().c_str(), 0);
   orc_UserSettings.SetProjSdNodeSelectedInterface(orc_NodeName, u32_Tmp);
   //Selected HALC domain & channel
   c_Tmp = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                              c_NodeIdSelectedHalcDomain.toStdString().c_str(), "").c_str();
   orc_UserSettings.SetProjSdNodeSelectedHalcDomain(orc_NodeName, c_Tmp);
   c_Tmp = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                              c_NodeIdSelectedHalcChannel.toStdString().c_str(), "").c_str();
   orc_UserSettings.SetProjSdNodeSelectedHalcChannel(orc_NodeName, c_Tmp);

   //CANopen columns
   c_Columns.clear();
   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_CanOpenOvColumnId, c_Columns);
   orc_UserSettings.SetProjSdNodeCanOpenOverviewColumnWidth(orc_NodeName, c_Columns);
   c_Columns.clear();
   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_CanOpenPdoOvColumnId, c_Columns);
   orc_UserSettings.SetProjSdNodeCanOpenPdoOverviewColumnWidth(orc_NodeName, c_Columns);

   //CANopen
   std::map<uint8_t, bool> c_LoadInterfaces;
   std::map<uint8_t, bool> c_LoadDevices;
   std::map<std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> >, bool> c_LoadDevice;
   u32_Tmp = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                 c_CanOpenExpandedCanOpenManagerCounter.toStdString().c_str(), 0);
   for (uint32_t u32_InterfaceCounter = 0UL; u32_InterfaceCounter < u32_Tmp; u32_InterfaceCounter++)
   {
      c_LoadInterfaces[static_cast<uint8_t>(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                                (c_CanOpenExpandedCanOpenManager +
                                                                 QString::number(u32_InterfaceCounter) +
                                                                 "InterfaceNumber").toStdString().c_str(), 0))] =
         orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                          (c_CanOpenExpandedCanOpenManager +
                           QString::number(u32_InterfaceCounter)).toStdString().c_str(), false);
   }
   const uint32_t u32_TmpDevices = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                       c_CanOpenExpandedCanOpenDevicesCounter.toStdString().c_str(),
                                                       0);
   for (uint32_t u32_DevicesCounter = 0UL; u32_DevicesCounter < u32_TmpDevices; u32_DevicesCounter++)
   {
      c_LoadDevices[static_cast<uint8_t>(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                             (c_CanOpenExpandedCanOpenDevices +
                                                              QString::number(u32_DevicesCounter) +
                                                              "InterfaceNumber").toStdString().c_str(), 0))] =
         orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                          (c_CanOpenExpandedCanOpenDevices +
                           QString::number(u32_DevicesCounter)).toStdString().c_str(), false);
   }
   const uint32_t u32_TmpDevice = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                      c_CanOpenExpandedCanOpenDeviceCounter.toStdString().c_str(),
                                                      0);
   for (uint32_t u32_DeviceCounter = 0UL; u32_DeviceCounter < u32_TmpDevice; u32_DeviceCounter++)
   {
      const std::pair<uint8_t, stw::scl::C_SclString> c_PairInterfaceId(static_cast<uint8_t>(orc_Ini.ReadInteger(
                                                                                                orc_SectionName.toStdString().c_str(),
                                                                                                (c_CanOpenExpandedCanOpenDevice + QString::number(
                                                                                                    u32_DeviceCounter) +
                                                                                                 "DeviceInterfaceNumber").toStdString().c_str(), 0)),
                                                                        orc_Ini.ReadString(
                                                                           orc_SectionName.toStdString().c_str(),
                                                                           (c_CanOpenExpandedCanOpenDevice + QString::number(
                                                                               u32_DeviceCounter) +
                                                                            "DeviceNodeName").toStdString().c_str(),
                                                                           ""));
      const std::pair<uint8_t, std::pair<uint8_t, stw::scl::C_SclString> > c_Pair(
         static_cast<uint8_t>(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                  (c_CanOpenExpandedCanOpenManager +
                                                   QString::number(u32_DeviceCounter) +
                                                   "InterfaceNumber").toStdString().c_str(), 0)),
         c_PairInterfaceId);
      c_LoadDevice[c_Pair] = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                              (c_CanOpenExpandedCanOpenDevice +
                                               QString::number(u32_DeviceCounter)).toStdString().c_str(), false);
   }
   orc_UserSettings.SetProjSdNodeExpandedCanOpenTree(orc_NodeName, c_LoadInterfaces, c_LoadDevices, c_LoadDevice);
   u32_Tmp = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                 c_CanOpenSelectedCanOpenManager.toStdString().c_str(), 0);
   orc_UserSettings.SetProjSdNodeSelectedCanOpenManager(orc_NodeName, static_cast<uint8_t>(u32_Tmp));
   u32_Tmp = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                 c_CanOpenSelectedCanOpenDeviceInterfaceNumber.toStdString().c_str(), 0);
   c_Tmp = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                              c_CanOpenSelectedCanOpenDeviceNodeName.toStdString().c_str(), "").c_str();
   orc_UserSettings.SetProjSdNodeSelectedCanOpenDevice(orc_NodeName, static_cast<uint8_t>(u32_Tmp), c_Tmp);
   u32_Tmp = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                 c_CanOpenSelectedCanOpenDeviceUseCaseIndex.toStdString().c_str(), 0);
   orc_UserSettings.SetProjSdNodeSelectedCanOpenDeviceUseCaseIndex(orc_NodeName, u32_Tmp);

   orc_UserSettings.SetProjSdNodeCanOpenSelectedUseCaseOrInterface(
      orc_NodeName,
      orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                       c_CanOpenSelectedUseCaseOrInterface.toStdString().c_str(), false));

   //HALC columns
   c_Columns.clear();
   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_HalcOvColumnId, c_Columns);
   orc_UserSettings.SetProjSdNodeHalcOverviewColumnWidth(orc_NodeName, c_Columns);
   c_Columns.clear();
   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_HalcConfigColumnId, c_Columns);
   orc_UserSettings.SetProjSdNodeHalcConfigColumnWidth(orc_NodeName, c_Columns);

   //Datapool count
   const int32_t s32_DatapoolCount = orc_Ini.ReadInteger(
      orc_SectionName.toStdString().c_str(), c_NodeIdDatapoolCount.toStdString().c_str(), 0);

   //Datapool
   for (int32_t s32_ItDatapool = 0; s32_ItDatapool < s32_DatapoolCount; ++s32_ItDatapool)
   {
      const QString c_DatapoolIdBase = static_cast<QString>("%1Datapool%2").arg(orc_NodeIdBase).arg(s32_ItDatapool);
      mh_LoadDatapool(orc_Ini, orc_SectionName, c_DatapoolIdBase, orc_NodeName, orc_UserSettings);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_BusIdBase       Bus id base name
   \param[in]      orc_BusName         Bus name
   \param[in,out]  orc_UserSettings    User settings to load
   \param[in]      oq_IsBus            Indicator if this function is used on a bus
   \param[in]      orc_NodeName        If not used on a bus the node name is required
   \param[in]      orc_DataPoolName    If not used on a bus the node data pool name is required
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadBus(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_BusIdBase,
                           const QString & orc_BusName, C_UsHandler & orc_UserSettings, const bool oq_IsBus,
                           const QString & orc_NodeName, const QString & orc_DataPoolName)
{
   const QString c_BusIdSelectedComProtocol =
      static_cast<QString>("%1Selected_com_protocol").arg(orc_BusIdBase);
   const QString c_BusIdMessageOverview = static_cast<QString>("%1MessageOverview").arg(orc_BusIdBase);
   const QString c_BusIdSignalOverview = static_cast<QString>("%1SignalOverview").arg(orc_BusIdBase);
   const QString c_BusIdMessageSelected = static_cast<QString>("%1Message_selected").arg(orc_BusIdBase);
   const QString c_BusIdSelectedMessageName = static_cast<QString>("%1Selected_message_name").arg(orc_BusIdBase);
   const QString c_BusIdSignalSelected = static_cast<QString>("%1Signal_selected").arg(orc_BusIdBase);
   const QString c_BusIdSelectedSignalName = static_cast<QString>("%1Selected_signal_index").arg(orc_BusIdBase);

   stw::opensyde_core::C_OscCanProtocol::E_Type e_SelectedProtocol;
   bool q_MessageSelected;
   QString c_MessageName;
   bool q_SignalSelected;
   QString c_SignalName;
   std::vector<int32_t> c_MessageColumns;
   std::vector<int32_t> c_SignalColumns;

   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_BusIdMessageOverview, c_MessageColumns);
   C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, c_BusIdSignalOverview, c_SignalColumns);

   e_SelectedProtocol = static_cast<stw::opensyde_core::C_OscCanProtocol::E_Type>(orc_Ini.ReadInteger(
                                                                                     orc_SectionName.toStdString().c_str(),
                                                                                     c_BusIdSelectedComProtocol.
                                                                                     toStdString().c_str(), 0));
   q_MessageSelected = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                        c_BusIdMessageSelected.toStdString().c_str(), false);
   c_MessageName = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                      c_BusIdSelectedMessageName.toStdString().c_str(), "").c_str();
   q_SignalSelected = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                       c_BusIdSignalSelected.toStdString().c_str(), false);
   c_SignalName = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                     c_BusIdSelectedSignalName.toStdString().c_str(), "").c_str();

   if (oq_IsBus == true)
   {
      orc_UserSettings.SetProjSdBusCommMessageOverviewColumnWidth(orc_BusName, c_MessageColumns);
      orc_UserSettings.SetProjSdBusCommSignalOverviewColumnWidth(orc_BusName, c_SignalColumns);
      orc_UserSettings.SetProjSdBusSelectedMessage(orc_BusName, e_SelectedProtocol, q_MessageSelected, c_MessageName,
                                                   q_SignalSelected, c_SignalName);
   }
   else
   {
      orc_UserSettings.SetProjSdNodeDatapoolCommMessageOverviewColumnWidth(orc_NodeName, orc_DataPoolName, orc_BusName,
                                                                           c_MessageColumns);
      orc_UserSettings.SetProjSdNodeDatapoolCommSignalOverviewColumnWidth(orc_NodeName, orc_DataPoolName, orc_BusName,
                                                                          c_SignalColumns);
      orc_UserSettings.SetProjSdNodeDatapoolListSelectedMessage(orc_NodeName, orc_DataPoolName, orc_BusName,
                                                                e_SelectedProtocol, q_MessageSelected, c_MessageName,
                                                                q_SignalSelected, c_SignalName);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node datapool part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_DatapoolIdBase  Node datapool id base name
   \param[in]      orc_NodeName        Node name
   \param[in,out]  orc_UserSettings    User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadDatapool(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DatapoolIdBase, const QString & orc_NodeName,
                                C_UsHandler & orc_UserSettings)
{
   const QString c_DatapoolIdName = static_cast<QString>("%1Name").arg(orc_DatapoolIdBase);
   const QString c_DatapoolName = orc_Ini.ReadString(
      orc_SectionName.toStdString().c_str(), c_DatapoolIdName.toStdString().c_str(), "").c_str();

   if (c_DatapoolName.compare("") != 0)
   {
      int32_t s32_SystemBusCount;
      int32_t s32_ListCount;
      const QString c_DatapoolIdExpandedListNameCount =
         static_cast<QString>("%1ExpandedListName_count").arg(orc_DatapoolIdBase);
      const QString c_DatapoolIdSelectedListNameCount =
         static_cast<QString>("%1SelectedListName_count").arg(orc_DatapoolIdBase);
      const QString c_DatapoolIdSelectedVariableNameCount =
         static_cast<QString>("%1SelectedVariableName_count").arg(orc_DatapoolIdBase);
      const QString c_DatapoolIdListCount =
         static_cast<QString>("%1Lists_count").arg(orc_DatapoolIdBase);
      const QString c_DatapoolIdInterfaceCount =
         static_cast<QString>("%1Interface_count").arg(orc_DatapoolIdBase);
      std::vector<QString> c_ExpandedListNames;
      std::vector<QString> c_SelectedListNames;
      std::vector<QString> c_SelectedVariableNames;
      const int32_t s32_ExpandedListNameCount = orc_Ini.ReadInteger(
         orc_SectionName.toStdString().c_str(), c_DatapoolIdExpandedListNameCount.toStdString().c_str(), 0);
      const int32_t s32_SelectedListNameCount = orc_Ini.ReadInteger(
         orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedListNameCount.toStdString().c_str(), 0);
      const int32_t s32_SelectedVariableNameCount = orc_Ini.ReadInteger(
         orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedVariableNameCount.toStdString().c_str(), 0);

      //Expanded lists
      c_ExpandedListNames.reserve(s32_ExpandedListNameCount);
      for (int32_t s32_ItExpandedListName = 0; s32_ItExpandedListName < s32_ExpandedListNameCount;
           ++s32_ItExpandedListName)
      {
         const QString c_DatapoolIdExpandedListNameBaseId =
            static_cast<QString>("%1ExpandedListName%2").arg(orc_DatapoolIdBase).arg(s32_ItExpandedListName);
         const QString c_DatapoolIdExpandedListNameId =
            static_cast<QString>("%1Name").arg(c_DatapoolIdExpandedListNameBaseId);
         const QString c_DatapoolIdExpandedListName = orc_Ini.ReadString(
            orc_SectionName.toStdString().c_str(), c_DatapoolIdExpandedListNameId.toStdString().c_str(), "").c_str();
         if (c_DatapoolIdExpandedListName.compare("") != 0)
         {
            c_ExpandedListNames.push_back(c_DatapoolIdExpandedListName);
         }
      }
      orc_UserSettings.SetProjSdNodeDatapoolOpenListNames(orc_NodeName, c_DatapoolName, c_ExpandedListNames);

      //Selected lists
      c_SelectedListNames.reserve(s32_SelectedListNameCount);
      for (int32_t s32_ItSelectedListName = 0; s32_ItSelectedListName < s32_SelectedListNameCount;
           ++s32_ItSelectedListName)
      {
         const QString c_DatapoolIdSelectedListNameBaseId =
            static_cast<QString>("%1SelectedListName%2").arg(orc_DatapoolIdBase).arg(s32_ItSelectedListName);
         const QString c_DatapoolIdSelectedListNameId =
            static_cast<QString>("%1Name").arg(c_DatapoolIdSelectedListNameBaseId);
         const QString c_DatapoolIdSelectedListName = orc_Ini.ReadString(
            orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedListNameId.toStdString().c_str(), "").c_str();
         if (c_DatapoolIdSelectedListName.compare("") != 0)
         {
            c_SelectedListNames.push_back(c_DatapoolIdSelectedListName);
         }
      }
      orc_UserSettings.SetProjSdNodeDatapoolSelectedListNames(orc_NodeName, c_DatapoolName, c_SelectedListNames);

      //Selected variables
      c_SelectedListNames.reserve(s32_SelectedVariableNameCount);
      for (int32_t s32_ItSelectedVariableName = 0; s32_ItSelectedVariableName < s32_SelectedVariableNameCount;
           ++s32_ItSelectedVariableName)
      {
         const QString c_DatapoolIdSelectedVariableNameBaseId = static_cast<QString>("%1SelectedVariableName%2").arg(
            orc_DatapoolIdBase).arg(
            s32_ItSelectedVariableName);
         const QString c_DatapoolIdSelectedVariableNameId =
            static_cast<QString>("%1Name").arg(c_DatapoolIdSelectedVariableNameBaseId);
         const QString c_DatapoolIdSelectedVariableName = orc_Ini.ReadString(
            orc_SectionName.toStdString().c_str(), c_DatapoolIdSelectedVariableNameId.toStdString().c_str(),
            "").c_str();
         if (c_DatapoolIdSelectedVariableName.compare("") != 0)
         {
            c_SelectedVariableNames.push_back(c_DatapoolIdSelectedVariableName);
         }
      }
      orc_UserSettings.SetProjSdNodeDatapoolSelectedVariableNames(orc_NodeName, c_DatapoolName,
                                                                  c_SelectedVariableNames);

      //Interfaces
      s32_SystemBusCount = orc_Ini.ReadInteger(
         orc_SectionName.toStdString().c_str(), c_DatapoolIdInterfaceCount.toStdString().c_str(), 0);
      for (int32_t s32_ItBus = 0; s32_ItBus < s32_SystemBusCount; ++s32_ItBus)
      {
         const QString c_BusIdBase = static_cast<QString>("%1Interface%2").arg(orc_DatapoolIdBase).arg(s32_ItBus);
         const QString c_BusIdName = static_cast<QString>("%1Name").arg(c_BusIdBase);
         const QString c_BusName = orc_Ini.ReadString(
            orc_SectionName.toStdString().c_str(), c_BusIdName.toStdString().c_str(), "").c_str();
         if (c_BusName.compare("") != 0)
         {
            mh_LoadBus(orc_Ini, orc_SectionName, c_BusIdBase, c_BusName, orc_UserSettings, false, orc_NodeName,
                       c_DatapoolName);
         }
      }

      //Lists
      s32_ListCount = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                          c_DatapoolIdListCount.toStdString().c_str(), 0);
      for (int32_t s32_ItList = 0; s32_ItList < s32_ListCount; ++s32_ItList)
      {
         const QString c_ListIdBase = static_cast<QString>("%1List%2").arg(orc_DatapoolIdBase).arg(s32_ItList);
         mh_LoadList(orc_Ini, orc_SectionName, c_ListIdBase, orc_NodeName, c_DatapoolName, orc_UserSettings);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_ListIdBase      List id base
   \param[in]      orc_NodeName        Node name
   \param[in]      orc_DataPoolName    Data pool name
   \param[in,out]  orc_UserSettings    User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadList(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_ListIdBase,
                            const QString & orc_NodeName, const QString & orc_DataPoolName,
                            C_UsHandler & orc_UserSettings)
{
   const QString c_ListIdName = static_cast<QString>("%1Name").arg(orc_ListIdBase);
   const QString c_ListName = orc_Ini.ReadString(
      orc_SectionName.toStdString().c_str(), c_ListIdName.toStdString().c_str(), "").c_str();

   if (c_ListName.compare("") != 0)
   {
      std::vector<int32_t> c_ColumnWidths;
      C_UsFiler::mh_LoadColumns(orc_Ini, orc_SectionName, orc_ListIdBase, c_ColumnWidths);
      orc_UserSettings.SetProjSdNodeDatapoolListColumnSizes(orc_NodeName, orc_DataPoolName, c_ListName, c_ColumnWidths);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_ViewIdBase      View id base name
   \param[in]      orc_ViewName        View name
   \param[in,out]  orc_UserSettings    User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadView(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_ViewIdBase,
                            const QString & orc_ViewName, C_UsHandler & orc_UserSettings)
{
   const QString c_ViewIdNavigationExpandedStatus =
      static_cast<QString>("%1_navigation_expanded_status").arg(orc_ViewIdBase);
   const QString c_ViewIdNodesCount = static_cast<QString>("%1Node_count").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupPosHorizontal = static_cast<QString>("%1_setup_x").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupPosVertical = static_cast<QString>("%1_setup_y").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupZoom = static_cast<QString>("%1_setup_zoom_value").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePosHorizontal = static_cast<QString>("%1_update_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePosVertical = static_cast<QString>("%1_update_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateZoom = static_cast<QString>("%1_update_zoom_value").arg(orc_ViewIdBase);
   const QString c_ViewIdParamExportPath = static_cast<QString>("%1_param_export_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamImportPath = static_cast<QString>("%1_param_import_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamRecordPath = static_cast<QString>("%1_param_record_path").arg(orc_ViewIdBase);
   const QString c_ViewIdParamRecordFileName = static_cast<QString>("%1_param_record_file_name").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateSplitterHorizontal = static_cast<QString>("%1_update_splitter_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateHorizontalSplitterVertical =
      static_cast<QString>("%1_update_horizontal_splitter_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogPositionHorizontal =
      static_cast<QString>("%1_update_progress_log_x").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogPositionVertical =
      static_cast<QString>("%1_update_progress_log_y").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogSizeWidth =
      static_cast<QString>("%1_update_progress_log_width").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogSizeHeight =
      static_cast<QString>("%1_update_progress_log_height").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateProgressLogIsMaximized =
      static_cast<QString>("%1_update_progress_log_is_maximized").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateSummaryBig = static_cast<QString>("%1_update_summary_is_type_big").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdateEmptyOptionalSectionsVisible =
      static_cast<QString>("%1_empty_optional_sections_visible").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxPositionHorizontal = static_cast<QString>("%1_toolbox_x").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxPositionVertical = static_cast<QString>("%1_toolbox_y").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxSizeWidth = static_cast<QString>("%1_toolbox_width").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxSizeHeight = static_cast<QString>("%1_toolbox_height").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardToolboxIsMaximized = static_cast<QString>("%1_toolbox_is_maximized").arg(
      orc_ViewIdBase);
   const QString c_ViewIdDashboardSelectedTabIndex = static_cast<QString>("%1_selected_tab_index").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardCount = static_cast<QString>("%1Dashboard_count").arg(orc_ViewIdBase);
   const QString c_ViewIdSetupPermission = static_cast<QString>("%1_setup_permission").arg(orc_ViewIdBase);
   const QString c_ViewIdUpdatePermission = static_cast<QString>("%1_update_permission").arg(orc_ViewIdBase);
   const QString c_ViewIdDashboardPermission = static_cast<QString>("%1_dashboard_permission").arg(orc_ViewIdBase);
   QPoint c_Pos;
   QSize c_Size;
   int32_t s32_Value;
   bool q_Value;

   std::array<bool, 3> c_ViewConfigs;

   //Navigation
   q_Value = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                              c_ViewIdNavigationExpandedStatus.toStdString().c_str(), false);
   orc_UserSettings.SetProjSvNavigationExpandedStatus(orc_ViewName, q_Value);

   //Setup pos
   c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdSetupPosHorizontal.toStdString().c_str(), 0));
   c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdSetupPosVertical.toStdString().c_str(), 0));

   //Setup zoom
   s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                   c_ViewIdSetupZoom.toStdString().c_str(), 100);

   //Setup set
   orc_UserSettings.SetProjSvSetupViewZoom(orc_ViewName, s32_Value);
   orc_UserSettings.SetProjSvSetupViewPos(orc_ViewName, c_Pos);

   //Update pos
   c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdUpdatePosHorizontal.toStdString().c_str(), 0));
   c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdUpdatePosVertical.toStdString().c_str(), 0));

   //Update zoom
   s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                   c_ViewIdUpdateZoom.toStdString().c_str(), 100);

   //Update set
   orc_UserSettings.SetProjSvUpdateViewZoom(orc_ViewName, s32_Value);
   orc_UserSettings.SetProjSvUpdateViewPos(orc_ViewName, c_Pos);

   //Param
   orc_UserSettings.SetProjSvParamExport(orc_ViewName, orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                                                          c_ViewIdParamExportPath.toStdString().c_str(),
                                                                          "").c_str());
   orc_UserSettings.SetProjSvParamImport(orc_ViewName, orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                                                          c_ViewIdParamImportPath.toStdString().c_str(),
                                                                          "").c_str());
   orc_UserSettings.SetProjSvParamRecord(orc_ViewName, orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                                                          c_ViewIdParamRecordPath.toStdString().c_str(),
                                                                          "").c_str(),
                                         orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                                            c_ViewIdParamRecordFileName.toStdString().c_str(),
                                                            "").c_str());

   //Splitter
   orc_UserSettings.SetProjSvUpdateSplitterHorizontal(orc_ViewName,
                                                      orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                                          c_ViewIdUpdateSplitterHorizontal.
                                                                          toStdString().
                                                                          c_str(), -1));
   orc_UserSettings.SetProjSvUpdateHorizontalSplitterVertical(
      orc_ViewName, orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                        c_ViewIdUpdateHorizontalSplitterVertical.toStdString().c_str(), -1));

   //Progress log
   c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdUpdateProgressLogPositionHorizontal.toStdString().c_str(), -1));
   c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdUpdateProgressLogPositionVertical.toStdString().c_str(), -1));
   c_Size.setWidth(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                       c_ViewIdUpdateProgressLogSizeWidth.toStdString().c_str(), 600));
   c_Size.setHeight(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                        c_ViewIdUpdateProgressLogSizeHeight.toStdString().c_str(), 400));
   q_Value = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                              c_ViewIdUpdateProgressLogIsMaximized.toStdString().c_str(), false);
   orc_UserSettings.SetProjSvUpdateProgressLog(orc_ViewName, c_Pos, c_Size, q_Value);

   //Update summary type
   q_Value = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                              c_ViewIdUpdateSummaryBig.toStdString().c_str(), true);
   orc_UserSettings.SetProjSvUpdateSummaryBig(orc_ViewName, q_Value);

   // Update package sections visibility of empty optional sections
   q_Value = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                              c_ViewIdUpdateEmptyOptionalSectionsVisible.toStdString().c_str(), true);
   orc_UserSettings.SetProjSvUpdateEmptyOptionalSectionsVisible(orc_ViewName, q_Value);

   // View nodes
   s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                   c_ViewIdNodesCount.toStdString().c_str(), 0);
   for (int32_t s32_ItNodes = 0; s32_ItNodes < s32_Value; ++s32_ItNodes)
   {
      const QString c_DashboardIdBase = static_cast<QString>("%1Node%2").arg(orc_ViewIdBase).arg(s32_ItNodes);
      mh_LoadViewNode(orc_Ini, orc_SectionName, c_DashboardIdBase, orc_ViewName, orc_UserSettings);
   }

   //Toolbox
   c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdDashboardToolboxPositionHorizontal.toStdString().c_str(), -1));
   c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                  c_ViewIdDashboardToolboxPositionVertical.toStdString().c_str(), -1));
   c_Size.setWidth(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                       c_ViewIdDashboardToolboxSizeWidth.toStdString().c_str(), 600));
   c_Size.setHeight(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                        c_ViewIdDashboardToolboxSizeHeight.toStdString().c_str(), 400));
   q_Value = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                              c_ViewIdDashboardToolboxIsMaximized.toStdString().c_str(), false);
   orc_UserSettings.SetProjSvDashboardToolbox(orc_ViewName, c_Pos, c_Size, q_Value);

   //General
   s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                   c_ViewIdDashboardSelectedTabIndex.toStdString().c_str(), -1);
   orc_UserSettings.SetProjSvDashboardSelectedTabIndex(orc_ViewName, s32_Value);

   //Dashboards
   s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                   c_ViewIdDashboardCount.toStdString().c_str(), 0);
   for (int32_t s32_ItDashboard = 0; s32_ItDashboard < s32_Value; ++s32_ItDashboard)
   {
      const QString c_DashboardIdBase = static_cast<QString>("%1Dashboard%2").arg(orc_ViewIdBase).arg(s32_ItDashboard);
      mh_LoadDashboard(orc_Ini, orc_SectionName, c_DashboardIdBase, orc_ViewName, orc_UserSettings);
   }

   //Permissions
   c_ViewConfigs[0] = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                       c_ViewIdSetupPermission.toStdString().c_str(),
                                       false);
   c_ViewConfigs[1] = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                       c_ViewIdUpdatePermission.toStdString().c_str(),
                                       false);
   c_ViewConfigs[2] = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                       c_ViewIdDashboardPermission.toStdString().c_str(),
                                       false);
   orc_UserSettings.SetViewPermission(orc_ViewName, c_ViewConfigs);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data rates per node part of user settings

   \param[in,out]  orc_Ini                      Ini handler
   \param[in]      orc_SectionName              Section name
   \param[in]      orc_DataRatePerNodeIdBase    View update data rate id base name
   \param[in]      orc_ViewName                 View name
   \param[in]      orc_NodeName                 Node name
   \param[in,out]  orc_UserSettings             User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadDataRatesPerNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                        const QString & orc_DataRatePerNodeIdBase, const QString & orc_ViewName,
                                        const QString & orc_NodeName, C_UsHandler & orc_UserSettings)
{
   const QString c_DataRateIdCount = static_cast<QString>("%1_count").arg(orc_DataRatePerNodeIdBase);
   const int32_t s32_ItDataRate = orc_Ini.ReadInteger(
      orc_SectionName.toStdString().c_str(), c_DataRateIdCount.toStdString().c_str(), 0);

   //Data rate count
   //Per checksum section
   for (int32_t s32_It = 0; s32_It < s32_ItDataRate; ++s32_It)
   {
      const QString c_DataRateIdBase = static_cast<QString>("%1DataRate%2").arg(orc_DataRatePerNodeIdBase).arg(s32_It);
      const QString c_DataRateIdChecksum = static_cast<QString>("%1_checksum").arg(c_DataRateIdBase);
      const QString c_DataRateIdCurrentValue = static_cast<QString>("%1_value").arg(c_DataRateIdBase);
      //Key
      const QString c_Checksum = orc_Ini.ReadString(
         orc_SectionName.toStdString().c_str(), c_DataRateIdChecksum.toStdString().c_str(), "").c_str();
      //Value count
      const float64_t f64_Value = orc_Ini.ReadFloat(
         orc_SectionName.toStdString().c_str(), c_DataRateIdCurrentValue.toStdString().c_str(), 0.0);
      //String to uint32_t
      if (c_Checksum.compare("") != 0)
      {
         bool q_Ok1;
         const uint32_t u32_Checksum = c_Checksum.toULong(&q_Ok1);
         if (q_Ok1 == true)
         {
            //Apply
            orc_UserSettings.AddProjSvNodeUpdateDataRate(orc_ViewName, orc_NodeName, u32_Checksum, f64_Value);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node view part of user settings

   \param[in,out]  orc_Ini             Ini handler
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_ViewNodeIdBase  View node ID base name
   \param[in]      orc_ViewName        View name (not a copy paste error)
   \param[in,out]  orc_UserSettings    User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadViewNode(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_ViewNodeIdBase, const QString & orc_ViewName,
                                C_UsHandler & orc_UserSettings)
{
   const QString c_ViewNodeIdName = static_cast<QString>("%1Name").arg(orc_ViewNodeIdBase);
   const QString c_NodeIdUpdateDataRateBaseId = static_cast<QString>("%1_update_data_rate").arg(orc_ViewNodeIdBase);
   const QString c_ViewNodeName = orc_Ini.ReadString(orc_SectionName.toStdString().c_str(),
                                                     c_ViewNodeIdName.toStdString().c_str(), "").c_str();

   if (c_ViewNodeName.compare("") != 0)
   {
      QVector<bool> c_ExpandedFlags;
      int32_t s32_SectionCounter;
      const int32_t s32_SectionCount = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                           static_cast<QString>("%1SectionCount").
                                                           arg(orc_ViewNodeIdBase).toStdString().c_str(), 1);
      c_ExpandedFlags.resize(s32_SectionCount);

      //Section expanded flags
      for (s32_SectionCounter = 0; s32_SectionCounter < s32_SectionCount; ++s32_SectionCounter)
      {
         c_ExpandedFlags[s32_SectionCounter] =
            orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                             static_cast<QString>("%1Section%2").
                             arg(orc_ViewNodeIdBase).
                             arg(s32_SectionCounter).toStdString().c_str(), true);
      }

      // Append
      orc_UserSettings.SetProjSvUpdateSectionsExpandedFlags(orc_ViewName, c_ViewNodeName, c_ExpandedFlags);

      //Data rate
      C_UsFiler::mh_LoadDataRatesPerNode(orc_Ini, orc_SectionName, c_NodeIdUpdateDataRateBaseId, orc_ViewName,
                                         c_ViewNodeName,
                                         orc_UserSettings);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load view dashboard part of user settings

   \param[in,out]  orc_Ini                Ini handler
   \param[in]      orc_SectionName        Section name
   \param[in]      orc_DashboardIdBase    View dashboard id base name
   \param[in]      orc_ViewName           View name (not a copy paste error)
   \param[in,out]  orc_UserSettings       User settings to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadDashboard(C_SclIniFile & orc_Ini, const QString & orc_SectionName,
                                 const QString & orc_DashboardIdBase, const QString & orc_ViewName,
                                 C_UsHandler & orc_UserSettings)
{
   const QString c_DashboardIdName = static_cast<QString>("%1Name").arg(orc_DashboardIdBase);
   const QString c_DashboardName = orc_Ini.ReadString(
      orc_SectionName.toStdString().c_str(), c_DashboardIdName.toStdString().c_str(), "").c_str();

   if (c_DashboardName.compare("") != 0)
   {
      QPoint c_Pos;
      QSize c_Size;
      int32_t s32_Value;
      const QString c_DashboardIdTornOffFlag = static_cast<QString>("%1_torn_off_flag").arg(orc_DashboardIdBase);
      const QString c_DashboardIdScenePosHorizontal = static_cast<QString>("%1_scene_x").arg(orc_DashboardIdBase);
      const QString c_DashboardIdScenePosVertical = static_cast<QString>("%1_scene_y").arg(orc_DashboardIdBase);
      const QString c_DashboardIdSceneZoom = static_cast<QString>("%1_scene_zoom").arg(orc_DashboardIdBase);

      //Torn off flag
      if (orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                           c_DashboardIdTornOffFlag.toStdString().c_str(), false) == true)
      {
         const QString c_DashboardIdWindowPosHorizontal = static_cast<QString>("%1_window_x").arg(orc_DashboardIdBase);
         const QString c_DashboardIdWindowPosVertical = static_cast<QString>("%1_window_y").arg(orc_DashboardIdBase);
         const QString c_DashboardIdSizeWidth = static_cast<QString>("%1_width").arg(orc_DashboardIdBase);
         const QString c_DashboardIdSizeHeight = static_cast<QString>("%1_height").arg(orc_DashboardIdBase);
         const QString c_DashboardIdMinFlag = static_cast<QString>("%1_min_flag").arg(orc_DashboardIdBase);
         const QString c_DashboardIdMaxFlag = static_cast<QString>("%1_max_flag").arg(orc_DashboardIdBase);
         bool q_Min;
         bool q_Max;
         //Window pos
         c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                        c_DashboardIdWindowPosHorizontal.toStdString().c_str(), 0));
         c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                        c_DashboardIdWindowPosVertical.toStdString().c_str(), 0));
         //Size
         c_Size.setWidth(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                             c_DashboardIdSizeWidth.toStdString().c_str(), 0));
         c_Size.setHeight(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                              c_DashboardIdSizeHeight.toStdString().c_str(), 0));
         //Flags
         q_Min = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                  c_DashboardIdMinFlag.toStdString().c_str(), false);
         q_Max = orc_Ini.ReadBool(orc_SectionName.toStdString().c_str(),
                                  c_DashboardIdMaxFlag.toStdString().c_str(), false);

         //Apply
         orc_UserSettings.SetProjSvDashboardTearOffPosition(orc_ViewName, c_DashboardName, c_Pos, c_Size, q_Min,
                                                            q_Max);
      }
      else
      {
         orc_UserSettings.SetProjSvDashboardMainTab(orc_ViewName, c_DashboardName);
      }
      //Scene pos
      c_Pos.setX(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                     c_DashboardIdScenePosHorizontal.toStdString().c_str(), 0));
      c_Pos.setY(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                     c_DashboardIdScenePosVertical.toStdString().c_str(), 0));

      //Zoom
      s32_Value = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                      c_DashboardIdSceneZoom.toStdString().c_str(), 100);
      //Apply
      orc_UserSettings.SetProjSvDashboardScenePositionAndZoom(orc_ViewName, c_DashboardName, c_Pos, s32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI language section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadCommon(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   QString c_Tmp;

   //Language
   c_Tmp = orc_Ini.ReadString("Common", "Language", "American english").c_str();
   if (C_UsHandler::h_CheckLanguageExists(c_Tmp) != 0)
   {
      c_Tmp = "American english";
   }
   orc_UserSettings.SetLanguage(c_Tmp);

   //Save As
   orc_UserSettings.SetCurrentSaveAsPath(orc_Ini.ReadString("Common", "SaveAsLocation", "").c_str());

   // Performance measurement
   orc_UserSettings.SetPerformanceActive(orc_Ini.ReadBool("Common", "PerformanceMeasurementActive", false));

   // Enable|Disable TSP shortcut
   orc_UserSettings.SetTspShortcutActive(orc_Ini.ReadBool("Common", "TSPShortcutActive", true));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI recent colors section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadColors(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   QVector<QColor> c_RecentColorsVector;

   //Colors
   for (int32_t s32_Counter = 1; s32_Counter <= 6; s32_Counter++)
   {
      QColor c_Color;
      c_Color.setRed(orc_Ini.ReadInteger("RecentColors",
                                         "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                                         "_Red", 255));
      c_Color.setGreen(orc_Ini.ReadInteger("RecentColors",
                                           "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                                           "_Green", 255));
      c_Color.setBlue(orc_Ini.ReadInteger("RecentColors",
                                          "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                                          "_Blue", 255));
      c_Color.setAlpha(orc_Ini.ReadInteger("RecentColors",
                                           "ColorNr" + C_SclString::IntToStr(s32_Counter) +
                                           "_Alpha", 255));
      c_RecentColorsVector.push_back(c_Color);
   }

   orc_UserSettings.SetRecentColors(c_RecentColorsVector);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI recent colors section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadNextRecentColorButtonNumber(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   //Next recent color button
   orc_UserSettings.SetNextRecentColorButtonNumber(orc_Ini.ReadInteger("RecentColors",
                                                                       "NextRecentColorButtonNumber", 1));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI recent projects section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadRecentProjects(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   QStringList c_List;
   QString c_Cur;

   //Recent projects
   c_List.clear();
   for (uint8_t u8_It = 0; u8_It < C_UsHandler::h_GetMaxRecentProjects(); ++u8_It)
   {
      c_Cur =
         orc_Ini.ReadString("RecentProjects", C_SclString::IntToStr(u8_It), "").c_str();
      if (c_Cur.compare("") != 0)
      {
         QFileInfo c_File;
         if (c_Cur.startsWith(".") == true)
         {
            c_File.setFile(C_Uti::h_GetExePath() + c_Cur);
         }
         else
         {
            c_File.setFile(c_Cur);
         }
         if (c_File.exists() == true)
         {
            c_List.append(c_File.absoluteFilePath());
         }
      }
   }
   orc_UserSettings.SetRecentProjects(c_List);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI project independent section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   QPoint c_Pos;
   QSize c_Size;
   bool q_Flag;
   int32_t s32_Value;

   //Screen position
   c_Pos.setX(orc_Ini.ReadInteger("Screen", "Position_x", 50));
   c_Pos.setY(orc_Ini.ReadInteger("Screen", "Position_y", 50));
   orc_UserSettings.SetScreenPos(c_Pos);

   // Application size
   c_Size.setWidth(orc_Ini.ReadInteger("Screen", "Size_width", 1000));
   c_Size.setHeight(orc_Ini.ReadInteger("Screen", "Size_height", 700));
   orc_UserSettings.SetAppSize(c_Size);

   // Application maximizing flag
   q_Flag = orc_Ini.ReadBool("Screen", "Size_maximized", true);
   orc_UserSettings.SetAppMaximized(q_Flag);

   // Sys def topology toolbox position
   c_Pos.setX(orc_Ini.ReadInteger("SdTopologyToolbox", "Position_x", -1));
   c_Pos.setY(orc_Ini.ReadInteger("SdTopologyToolbox", "Position_y", -1));
   orc_UserSettings.SetSdTopologyToolboxPos(c_Pos);

   // Sys def topology toolbox size
   c_Size.setWidth(orc_Ini.ReadInteger("SdTopologyToolbox", "Size_width", 600));
   c_Size.setHeight(orc_Ini.ReadInteger("SdTopologyToolbox", "Size_height", 400));
   orc_UserSettings.SetSdTopologyToolboxSize(c_Size);

   // Sys def topology toolbox maximizing flag
   q_Flag = orc_Ini.ReadBool("SdTopologyToolbox", "Size_maximized", true);
   orc_UserSettings.SetSdTopologyToolboxMaximized(q_Flag);

   // Sys def node edit splitter (Default: 1242; Fits perfect when using full hd resolution)
   s32_Value = orc_Ini.ReadInteger("SdNodeEdit", "SplitterX", 1242);
   orc_UserSettings.SetSdNodeEditSplitterHorizontal(s32_Value);

   // Sys def node edit HALC splitter
   s32_Value = orc_Ini.ReadInteger("SdNodeEdit", "HalcSplitterX", 400);
   orc_UserSettings.SetSdNodeEditHalcSplitterHorizontal(s32_Value);

   // Sys def node edit CANopen Manager splitter
   s32_Value = orc_Ini.ReadInteger("SdNodeEdit", "CoManagerSplitterX", 400);
   orc_UserSettings.SetSdNodeEditCoManagerSplitterHorizontal(s32_Value);

   // Sys def bus edit splitters
   s32_Value = orc_Ini.ReadInteger("SdBusEdit", "TreeSplitterX", 0);
   orc_UserSettings.SetSdBusEditTreeSplitterHorizontal(s32_Value);
   s32_Value = orc_Ini.ReadInteger("SdBusEdit", "TreeSplitterX2", 0);
   orc_UserSettings.SetSdBusEditTreeSplitterHorizontal2(s32_Value);
   s32_Value = orc_Ini.ReadInteger("SdBusEdit", "LayoutSplitterX", 0);
   orc_UserSettings.SetSdBusEditLayoutSplitterHorizontal(s32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI project independent section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
   \param[in]      orc_ActiveProject   Actual project to load project specific settings.
                                       Empty string results in default values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadProjectDependentSection(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini,
                                               const QString & orc_ActiveProject)
{
   QPoint c_Pos;

   if (orc_ActiveProject != "")
   {
      // project specific settings
      int32_t s32_Value;
      int32_t s32_SystemBusCount;
      int32_t s32_SystemNodeCount;
      int32_t s32_SystemViewCount;
      int32_t s32_SysDefSubMode;
      uint32_t u32_SysDefIndex;
      uint32_t u32_SysDefFlag;
      int32_t s32_SysViewSubMode;
      uint32_t u32_SysViewIndex;
      uint32_t u32_SysViewFlag;

      // Mode
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjMode", 0);
      orc_UserSettings.SetProjLastMode(s32_Value);

      // Navi bar
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "navigation-width", 300);
      orc_UserSettings.SetNaviBarSize(s32_Value);
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "navigation-node-section-width", 200);
      orc_UserSettings.SetNaviBarNodeSectionSize(s32_Value);

      // Sys def topology view port position
      c_Pos.setX(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyView_x", 0));
      c_Pos.setY(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyView_y", 0));
      orc_UserSettings.SetProjSdTopologyViewPos(c_Pos);

      // Sys def topology view zoom value
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "SdTopologyViewZoom_value", 100);
      orc_UserSettings.SetProjSdTopologyViewZoom(s32_Value);

      // Last screen mode
      s32_SysDefSubMode =
         static_cast<int32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubMode_value", 0));
      u32_SysDefIndex =
         static_cast<uint32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubIndex_value", 0));
      u32_SysDefFlag =
         static_cast<uint32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdSubFlag_value", 0));
      s32_SysViewSubMode =
         static_cast<int32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubMode_value", 0));
      u32_SysViewIndex =
         static_cast<uint32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubIndex_value", 0));
      u32_SysViewFlag =
         static_cast<uint32_t>(orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSvSubFlag_value", 0));

      orc_UserSettings.SetProjLastScreenMode(s32_SysDefSubMode, u32_SysDefIndex, u32_SysDefFlag,
                                             s32_SysViewSubMode, u32_SysViewIndex, u32_SysViewFlag);

      //TSP
      orc_UserSettings.SetProjSdTopologyLastKnownTspPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_tsp_path", "").c_str());

      //File generation
      orc_UserSettings.SetProjSdTopologyLastKnownCodeExportPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_code_export_path", "").c_str());

      //Import
      orc_UserSettings.SetProjSdTopologyLastKnownImportPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_import_path", "").c_str());

      //Import CANopen
      orc_UserSettings.SetProjSdTopologyLastKnownCanOpenEdsPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_CANopen_EDS_path", "").c_str());

      //Export
      orc_UserSettings.SetProjSdTopologyLastKnownExportPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_export_path", "").c_str());

      //Last path from where a .syde_devdef file was loaded
      orc_UserSettings.SetProjSdTopologyLastKnownDeviceDefPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_device_definition_path", "").c_str());

      //RTF File Export
      orc_UserSettings.SetProjSdTopologyLastKnownRtfPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_path", "").c_str());
      orc_UserSettings.SetProjSdTopologyLastKnownRtfCompanyName(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_company_name", "").c_str());
      orc_UserSettings.SetProjSdTopologyLastKnownRtfCompanyLogoPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSdTopology_last_known_rtf_company_logo_path", "").c_str());

      //HALC Paths
      orc_UserSettings.SetLastKnownHalcDefPath(
         orc_Ini.ReadString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_def_path", "").c_str());
      orc_UserSettings.SetLastKnownHalcImportPath(
         orc_Ini.ReadString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_import_path", "").c_str());
      orc_UserSettings.SetLastKnownHalcExportPath(
         orc_Ini.ReadString(orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_halc_export_path", "").c_str());

      //Service Project Path
      orc_UserSettings.SetLastKnownServiceProjectPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_service_project_path", "").c_str());

      //RAMView Project Import Path
      orc_UserSettings.SetLastKnownRamViewProjectPath(
         orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), "ProjSd_last_known_ramview_project_path", "").c_str());

      // J1939 Catalog Path
      orc_UserSettings.SetLastKnownJ1939CatalogPath(orc_Ini.ReadString(
                                                       orc_ActiveProject.toStdString().c_str(),
                                                       "ProjSd_last_known_j1939_catalog_path", "").c_str());

      // Chart data as CSV Export
      orc_UserSettings.SetLastKnownCsvExportPath(orc_Ini.ReadString(
                                                    orc_ActiveProject.toStdString().c_str(),
                                                    "ProjSd_last_known_csv_export_path", "").c_str());

      // Last tab index in system definition
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdNodeEditTabIndex_value", 0);
      orc_UserSettings.SetProjLastSysDefNodeTabIndex(s32_Value);
      s32_Value = orc_Ini.ReadInteger(orc_ActiveProject.toStdString().c_str(), "ProjSdBusEditTabIndex_value", 0);
      orc_UserSettings.SetProjLastSysDefBusTabIndex(s32_Value);

      //System nodes
      s32_SystemNodeCount = orc_Ini.ReadInteger(
         orc_ActiveProject.toStdString().c_str(), "ProjSdNode_count", 0);
      for (int32_t s32_ItNode = 0; s32_ItNode < s32_SystemNodeCount; ++s32_ItNode)
      {
         const QString c_NodeIdBase = static_cast<QString>("SdNode%1").arg(s32_ItNode);
         const QString c_NodeIdName = static_cast<QString>("%1Name").arg(c_NodeIdBase);
         const QString c_NodeName = orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), c_NodeIdName.toStdString().c_str(), "").c_str();
         if (c_NodeName.compare("") != 0)
         {
            mh_LoadNode(orc_Ini, orc_ActiveProject, c_NodeIdBase, c_NodeName, orc_UserSettings);
         }
      }

      //System buses
      s32_SystemBusCount = orc_Ini.ReadInteger(
         orc_ActiveProject.toStdString().c_str(), "ProjSdBus_count", 0);
      for (int32_t s32_ItBus = 0; s32_ItBus < s32_SystemBusCount; ++s32_ItBus)
      {
         const QString c_BusIdBase = static_cast<QString>("SdBus%1").arg(s32_ItBus);
         const QString c_BusIdName = static_cast<QString>("%1Name").arg(c_BusIdBase);
         const QString c_BusName = orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), c_BusIdName.toStdString().c_str(), "").c_str();
         if (c_BusName.compare("") != 0)
         {
            mh_LoadBus(orc_Ini, orc_ActiveProject, c_BusIdBase, c_BusName, orc_UserSettings, true, "", "");
         }
      }

      //System views
      s32_SystemViewCount = orc_Ini.ReadInteger(
         orc_ActiveProject.toStdString().c_str(), "ProjSvSetupView_count", 0);
      for (int32_t s32_ItView = 0; s32_ItView < s32_SystemViewCount; ++s32_ItView)
      {
         const QString c_ViewIdBase = static_cast<QString>("SvSetupView%1").arg(s32_ItView);
         const QString c_ViewIdName = static_cast<QString>("%1Name").arg(c_ViewIdBase);
         const QString c_ViewName = orc_Ini.ReadString(
            orc_ActiveProject.toStdString().c_str(), c_ViewIdName.toStdString().c_str(), "").c_str();
         if (c_ViewName.compare("") != 0)
         {
            mh_LoadView(orc_Ini, orc_ActiveProject, c_ViewIdBase, c_ViewName, orc_UserSettings);
         }
      }
   }
   else
   {
      // Mode
      orc_UserSettings.SetProjLastMode(0); // default is SD (network topology)

      // Fill default values in case of new project
      orc_UserSettings.SetNaviBarSize(300);
      orc_UserSettings.SetNaviBarNodeSectionSize(200);

      // Sys def topology view port position
      c_Pos.setX(0);
      c_Pos.setY(0);
      orc_UserSettings.SetProjSdTopologyViewPos(c_Pos);

      // Sys def topology view zoom value
      orc_UserSettings.SetProjSdTopologyViewZoom(100);

      // Last screen mode
      orc_UserSettings.SetProjLastScreenMode(0, 0, 0, 0, 0, 0);

      //File generation
      orc_UserSettings.SetProjSdTopologyLastKnownCodeExportPath("");

      //Import
      orc_UserSettings.SetProjSdTopologyLastKnownImportPath("");

      //Import CANopen
      orc_UserSettings.SetProjSdTopologyLastKnownCanOpenEdsPath("");

      //Export
      orc_UserSettings.SetProjSdTopologyLastKnownExportPath("");

      //RTF File Export
      orc_UserSettings.SetProjSdTopologyLastKnownRtfPath("");
      orc_UserSettings.SetProjSdTopologyLastKnownRtfCompanyName("");
      orc_UserSettings.SetProjSdTopologyLastKnownRtfCompanyLogoPath("");

      // Last tab index in system definition
      orc_UserSettings.SetProjLastSysDefNodeTabIndex(0);
      orc_UserSettings.SetProjLastSysDefBusTabIndex(0);

      //System nodes, System buses, System views
      orc_UserSettings.ClearMaps();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save columns

   \param[in,out]  orc_Ini             Ini
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_IdBase          Id base
   \param[in]      orc_ColumnWidths    Column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveColumns(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_IdBase,
                               const std::vector<int32_t> & orc_ColumnWidths)
{
   const QString c_IdColumnCount = static_cast<QString>("%1Column_Count").arg(orc_IdBase);

   orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(),
                        c_IdColumnCount.toStdString().c_str(), orc_ColumnWidths.size());
   for (uint32_t u32_ItCol = 0; u32_ItCol < orc_ColumnWidths.size(); ++u32_ItCol)
   {
      const QString c_IdColumn = static_cast<QString>("%1Column%2").arg(orc_IdBase).arg(u32_ItCol);
      orc_Ini.WriteInteger(orc_SectionName.toStdString().c_str(), c_IdColumn.toStdString().c_str(),
                           orc_ColumnWidths[u32_ItCol]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load columns

   \param[in,out]  orc_Ini             Ini
   \param[in]      orc_SectionName     Section name
   \param[in]      orc_IdBase          Id base
   \param[in,out]  orc_ColumnWidths    Column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadColumns(C_SclIniFile & orc_Ini, const QString & orc_SectionName, const QString & orc_IdBase,
                               std::vector<int32_t> & orc_ColumnWidths)
{
   const QString c_IdColumnCount = static_cast<QString>("%1Column_Count").arg(orc_IdBase);
   const int32_t s32_ColumnCount = orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                       c_IdColumnCount.toStdString().c_str(), 0);

   orc_ColumnWidths.reserve(s32_ColumnCount);
   for (int32_t s32_ItCol = 0; s32_ItCol < s32_ColumnCount; ++s32_ItCol)
   {
      const QString c_IdColumn = static_cast<QString>("%1Column%2").arg(orc_IdBase).arg(s32_ItCol);
      orc_ColumnWidths.push_back(orc_Ini.ReadInteger(orc_SectionName.toStdString().c_str(),
                                                     c_IdColumn.toStdString().c_str(), 0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load Screenshot GIF Play timer from user settings .ini

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadScreenshotGifSucessTimeout(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   orc_UserSettings.SetScreenshotGifSucessTimeout(orc_Ini.ReadInteger("Common", "ScreenshotGifSucessTimeout", 3000));
}
