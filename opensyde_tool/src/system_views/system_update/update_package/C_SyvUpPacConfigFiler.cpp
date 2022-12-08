//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Update package configuration file reader/writer (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_SyvUpPacConfigFiler.hpp"

#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSvHandlerFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load update package configuration

   \param[in]  orc_FilePath   File path for configuration
   \param[in]  orc_Config     Update package configuration

   \return
   C_NO_ERR   File load
   C_RD_WR    Specified file does not exist
   C_NOACT    Could not read data from file
   C_CONFIG   Content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacConfigFiler::h_LoadConfig(const QString & orc_FilePath, C_SyvUpPacConfig & orc_Config)
{
   int32_t s32_Return = C_RD_WR;

   if (stw::tgl::TglFileExists(orc_FilePath.toStdString().c_str()) == true)
   {
      C_OscXmlParserLog c_XmlParser;

      c_XmlParser.SetLogHeading("Loading Update Package Configuration");

      s32_Return = c_XmlParser.LoadFromFile(orc_FilePath.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-update-package-configuration")
         {
            if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
            {
               uint16_t u16_FileVersion = 0U;
               try
               {
                  u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
               }
               catch (...)
               {
                  osc_write_log_error("Loading Update Package Configuration",
                                      "\"file-version\" could not be converted to a number.");
                  s32_Return = C_CONFIG;
               }

               //is the file version one we know ?
               if (s32_Return == C_NO_ERR)
               {
                  osc_write_log_info("Loading Update Package Configuration", "Value of \"file-version\": " +
                                     C_SclString::IntToStr(u16_FileVersion));
                  //Check file version
                  if (u16_FileVersion != hu16_FILE_VERSION_LATEST)
                  {
                     osc_write_log_error("Loading Update Package Configuration",
                                         "Version defined by \"file-version\" is not supported.");
                     s32_Return = C_CONFIG;
                  }
               }

               //Return
               tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-update-package-configuration");

               // Get all nodes
               if (mh_LoadNodes(c_XmlParser, orc_Config) == true)
               {
                  //Return
                  tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-update-package-configuration");
               }
               else
               {
                  s32_Return = C_CONFIG;
               }
            }
            else
            {
               osc_write_log_error("Loading Update Package Configuration", "Could not find \"file-version\" node.");
               s32_Return = C_CONFIG;
            }
         }
         else
         {
            s32_Return = C_CONFIG;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save update package configuration

   \param[in]   orc_FilePath  File path for configuration
   \param[out]  orc_Config    Update package configuration

   \return
   C_NO_ERR    File save
   C_RANGE     File already exists
   C_NOACT     Could not write data to file
   C_CONFIG    Error on creating XML file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacConfigFiler::h_SaveConfig(const QString & orc_FilePath, const C_SyvUpPacConfig & orc_Config)
{
   int32_t s32_Return = C_RANGE;
   C_OscXmlParser c_XmlParser;

   if (stw::tgl::TglFileExists(orc_FilePath.toStdString().c_str()) == false)
   {
      bool q_Success;

      // Main node
      q_Success =
         (c_XmlParser.CreateAndSelectNodeChild("opensyde-update-package-configuration") ==
          "opensyde-update-package-configuration");

      if (q_Success == true)
      {
         // File Version
         q_Success = (c_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");

         if (q_Success == true)
         {
            c_XmlParser.SetNodeContent(C_SclString::IntToStr(hu16_FILE_VERSION_LATEST));
            //Return
            tgl_assert(c_XmlParser.SelectNodeParent() == "opensyde-update-package-configuration");
         }
      }

      if (q_Success == true)
      {
         uint32_t u32_NodeCounter;

         // Save each update package node configuration
         q_Success = (c_XmlParser.CreateAndSelectNodeChild("nodes") == "nodes");

         if (q_Success == true)
         {
            for (u32_NodeCounter = 0U; u32_NodeCounter < orc_Config.c_NodeConfigs.size(); ++u32_NodeCounter)
            {
               q_Success = mh_SaveNode(c_XmlParser, orc_Config.c_NodeConfigs[u32_NodeCounter]);

               if (q_Success == false)
               {
                  break;
               }
            }
         }
      }

      if (q_Success == true)
      {
         // Save file
         s32_Return = c_XmlParser.SaveToFile(orc_FilePath.toStdString().c_str());
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacConfigFiler::C_SyvUpPacConfigFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_SaveNode(C_OscXmlParser & orc_XmlParser, const C_SyvUpPacConfigNode & orc_NodeConfig)
{
   bool q_Success;

   q_Success = (orc_XmlParser.CreateAndSelectNodeChild("node") == "node");

   if (q_Success == true)
   {
      uint32_t u32_Counter;

      orc_XmlParser.CreateNodeChild("name", orc_NodeConfig.c_Name.toStdString().c_str());
      orc_XmlParser.CreateNodeChild("type", orc_NodeConfig.c_DeviceType.toStdString().c_str());

      q_Success = (orc_XmlParser.CreateAndSelectNodeChild("applications") == "applications");

      if (q_Success == true)
      {
         // Save all application configurations
         for (u32_Counter = 0U; u32_Counter < orc_NodeConfig.c_AppConfigs.size(); ++u32_Counter)
         {
            q_Success = mh_SaveApp(orc_XmlParser, orc_NodeConfig.c_AppConfigs[u32_Counter]);

            if (q_Success == false)
            {
               break;
            }
         }

         //Only continue with xml saving if no error
         if (q_Success == true)
         {
            // Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
         }
      }

      if (q_Success == true)
      {
         q_Success = (orc_XmlParser.CreateAndSelectNodeChild("parameter-set-images") == "parameter-set-images");
      }
      if (q_Success == true)
      {
         // Save all parameter set configurations
         for (u32_Counter = 0U; u32_Counter < orc_NodeConfig.c_ParamSetConfigs.size(); ++u32_Counter)
         {
            q_Success = mh_SaveParamSet(orc_XmlParser, orc_NodeConfig.c_ParamSetConfigs[u32_Counter]);

            if (q_Success == false)
            {
               break;
            }
         }

         // Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
      }

      if (q_Success == true)
      {
         q_Success = (orc_XmlParser.CreateAndSelectNodeChild("files") == "files");
      }
      if (q_Success == true)
      {
         // Save all file configurations
         for (u32_Counter = 0U; u32_Counter < orc_NodeConfig.c_FileConfigs.size(); ++u32_Counter)
         {
            q_Success = mh_SaveFile(orc_XmlParser, orc_NodeConfig.c_FileConfigs[u32_Counter]);

            if (q_Success == false)
            {
               break;
            }
         }

         //Only continue with xml saving if no error
         if (q_Success == true)
         {
            // Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
         }
      }

      if (q_Success == true)
      {
         mh_SaveNodeUpdateInformationPem(orc_NodeConfig, orc_XmlParser);
      }

      // Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "nodes");
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_SaveApp(C_OscXmlParser & orc_XmlParser, const C_SyvUpPacConfigNodeApp & orc_AppConfig)
{
   bool q_Success;

   q_Success = (orc_XmlParser.CreateAndSelectNodeChild("application") == "application");

   if (q_Success == true)
   {
      orc_XmlParser.CreateNodeChild("type", C_OscNodeApplication::h_ApplicationToString(orc_AppConfig.e_Type));
      orc_XmlParser.CreateNodeChild("name", orc_AppConfig.c_Name.toStdString().c_str());
      orc_XmlParser.CreateNodeChild("path", orc_AppConfig.c_Path.toStdString().c_str());

      // Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "applications");
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_SaveParamSet(C_OscXmlParser & orc_XmlParser, const QString & orc_Path)
{
   bool q_Success;

   q_Success = (orc_XmlParser.CreateAndSelectNodeChild("parameter-set-image") == "parameter-set-image");

   if (q_Success == true)
   {
      orc_XmlParser.CreateNodeChild("path", orc_Path.toStdString().c_str());

      // Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "parameter-set-images");
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_SaveFile(C_OscXmlParser & orc_XmlParser, const QString & orc_Path)
{
   bool q_Success;

   q_Success = (orc_XmlParser.CreateAndSelectNodeChild("file") == "file");

   if (q_Success == true)
   {
      orc_XmlParser.CreateNodeChild("path", orc_Path.toStdString().c_str());

      // Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "files");
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node update information pem

   \param[in]      orc_NodeConfig   Node config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacConfigFiler::mh_SaveNodeUpdateInformationPem(const C_SyvUpPacConfigNode & orc_NodeConfig,
                                                            C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("pem-file");
   orc_XmlParser.CreateNodeChild("path", orc_NodeConfig.c_PemFilePath.toStdString());
   orc_XmlParser.CreateAndSelectNodeChild("states");
   orc_XmlParser.CreateNodeChild("security", C_PuiSvHandlerFiler::h_PemFileStateSecurityToString(
                                    orc_NodeConfig.e_StateSecurity).toStdString());
   orc_XmlParser.CreateNodeChild("debugger", C_PuiSvHandlerFiler::h_PemFileStateDebuggerToString(
                                    orc_NodeConfig.e_StateDebugger).toStdString());
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_LoadNodes(C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfig & orc_Config)
{
   bool q_Success = true;

   if (orc_XmlParser.SelectNodeChild("nodes") == "nodes")
   {
      if (orc_XmlParser.SelectNodeChild("node") == "node")
      {
         do
         {
            C_SyvUpPacConfigNode c_NodeConfig;

            // Name of node
            if (orc_XmlParser.SelectNodeChild("name") == "name")
            {
               c_NodeConfig.c_Name = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
            }
            else
            {
               q_Success = false;
            }

            // Type of node
            if (orc_XmlParser.SelectNodeChild("type") == "type")
            {
               c_NodeConfig.c_DeviceType = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
            }
            else
            {
               q_Success = false;
            }

            if (q_Success == true)
            {
               // Get all applications for this node
               q_Success = mh_LoadApps(orc_XmlParser, c_NodeConfig);

               //Only continue with xml parsing if no error
               if (q_Success == true)
               {
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
               }
            }

            if (q_Success == true)
            {
               // Get all applications for this node
               q_Success = mh_LoadParamSets(orc_XmlParser, c_NodeConfig);

               //Only continue with xml parsing if no error
               if (q_Success == true)
               {
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
               }
            }

            if (q_Success == true)
            {
               // Get all applications for this node
               q_Success = mh_LoadFiles(orc_XmlParser, c_NodeConfig);

               //Only continue with xml parsing if no error
               if (q_Success == true)
               {
                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
               }
            }

            if (q_Success == true)
            {
               if (mh_LoadNodeUpdateInformationPem(c_NodeConfig, orc_XmlParser) != C_NO_ERR)
               {
                  q_Success = false;
               }
            }

            if (q_Success == true)
            {
               // Node finished
               orc_Config.c_NodeConfigs.push_back(c_NodeConfig);
            }
            else
            {
               break;
            }
         }
         while (orc_XmlParser.SelectNodeNext("node") == "node");

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "nodes");
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_LoadApps(C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfigNode & orc_NodeConfig)
{
   bool q_Success = true;

   if (orc_XmlParser.SelectNodeChild("applications") == "applications")
   {
      if (orc_XmlParser.SelectNodeChild("application") == "application")
      {
         do
         {
            C_SyvUpPacConfigNodeApp c_AppConfig;

            // Type of application
            if (orc_XmlParser.SelectNodeChild("type") == "type")
            {
               C_OscNodeApplication::h_StringToApplication(orc_XmlParser.GetNodeContent(), c_AppConfig.e_Type);

               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               q_Success = false;
            }

            // Name of application
            if (orc_XmlParser.SelectNodeChild("name") == "name")
            {
               c_AppConfig.c_Name = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               q_Success = false;
            }

            // Path of application
            if (orc_XmlParser.SelectNodeChild("path") == "path")
            {
               c_AppConfig.c_Path = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               q_Success = false;
            }

            if (q_Success == true)
            {
               // Application finished
               orc_NodeConfig.c_AppConfigs.push_back(c_AppConfig);
            }
            else
            {
               break;
            }
         }
         while (orc_XmlParser.SelectNodeNext("application") == "application");

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "applications");
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_LoadParamSets(C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfigNode & orc_NodeConfig)
{
   bool q_Success = true;

   if (orc_XmlParser.SelectNodeChild("parameter-set-images") == "parameter-set-images")
   {
      if (orc_XmlParser.SelectNodeChild("parameter-set-image") == "parameter-set-image")
      {
         do
         {
            QString c_Path;

            // Path of application
            if (orc_XmlParser.SelectNodeChild("path") == "path")
            {
               c_Path = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "parameter-set-image");
            }
            else
            {
               q_Success = false;
            }

            if (q_Success == true)
            {
               // Parameter set image finished
               orc_NodeConfig.c_ParamSetConfigs.push_back(c_Path);
            }
            else
            {
               break;
            }
         }
         while (orc_XmlParser.SelectNodeNext("parameter-set-image") == "parameter-set-image");

         //Only continue with xml parsing if no error
         if (q_Success == true)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "parameter-set-images");
         }
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacConfigFiler::mh_LoadFiles(C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfigNode & orc_NodeConfig)
{
   bool q_Success = true;

   if (orc_XmlParser.SelectNodeChild("files") == "files")
   {
      if (orc_XmlParser.SelectNodeChild("file") == "file")
      {
         do
         {
            QString c_Path;

            // Path of application
            if (orc_XmlParser.SelectNodeChild("path") == "path")
            {
               c_Path = orc_XmlParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "file");
            }
            else
            {
               q_Success = false;
            }

            if (q_Success == true)
            {
               // Parameter set image finished
               orc_NodeConfig.c_FileConfigs.push_back(c_Path);
            }
            else
            {
               break;
            }
         }
         while (orc_XmlParser.SelectNodeNext("file") == "file");

         //Only continue with xml parsing if no error
         if (q_Success == true)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "files");
         }
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem

   \param[in,out]  orc_NodeConfig   Node config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacConfigFiler::mh_LoadNodeUpdateInformationPem(C_SyvUpPacConfigNode & orc_NodeConfig,
                                                               C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("pem-file") == "pem-file")
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("path");
      if (s32_Retval == C_NO_ERR)
      {
         orc_NodeConfig.c_PemFilePath = orc_XmlParser.GetNodeContent().c_str();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
         s32_Retval = C_SyvUpPacConfigFiler::mh_LoadNodeUpdateInformationPemStates(orc_NodeConfig,
                                                                                   orc_XmlParser);
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node update information pem states

   \param[in,out]  orc_NodeConfig   Node config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   STW error codes

   \retval   C_NO_ERR   Information loaded
   \retval   C_CONFIG   Error loading information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacConfigFiler::mh_LoadNodeUpdateInformationPemStates(C_SyvUpPacConfigNode & orc_NodeConfig,
                                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError("states");

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("security");
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSvNodeUpdate::E_StateSecurity e_StateSecurity = C_PuiSvNodeUpdate::eST_SEC_NO_CHANGE;
         s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateSecurity(
            orc_XmlParser.GetNodeContent().c_str(), e_StateSecurity);
         if (s32_Retval == C_NO_ERR)
         {
            orc_NodeConfig.e_StateSecurity = e_StateSecurity;
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "states");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         C_PuiSvNodeUpdate::E_StateDebugger e_StateDebugger = C_PuiSvNodeUpdate::eST_DEB_NO_CHANGE;
         s32_Retval = orc_XmlParser.SelectNodeChildError("debugger");
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_PuiSvHandlerFiler::h_StringToPemFileStateDebugger(
               orc_XmlParser.GetNodeContent().c_str(), e_StateDebugger);
            if (s32_Retval == C_NO_ERR)
            {
               orc_NodeConfig.e_StateDebugger = e_StateDebugger;
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "states");
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "pem-file");
      }
   }
   return s32_Retval;
}
