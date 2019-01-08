//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Update package configuration file reader/writer (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_SyvUpUpdatePackageConfigFiler.h"

#include "TGLFile.h"
#include "TGLUtils.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Load update package configuration

   \param[in]     orc_FilePath       File path for configuration
   \param[in]     orc_Config         Update package configuration

   \return
   C_NO_ERR   File load
   C_RD_WR    Specified file does not exist
   C_NOACT    Could not read data from file
   C_CONFIG   Content of file is invalid or incomplete

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageConfigFiler::h_LoadConfig(const QString & orc_FilePath,
                                                     C_SyvUpUpdatePackageConfig & orc_Config)
{
   sint32 s32_Return = C_RD_WR;

   if (stw_tgl::TGL_FileExists(orc_FilePath.toStdString().c_str()) == true)
   {
      C_OSCXMLParser c_XMLParser;

      s32_Return = c_XMLParser.LoadFromFile(orc_FilePath.toStdString().c_str());

      if (s32_Return == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-update-package-configuration")
         {
            if (c_XMLParser.SelectNodeChild("file-version") == "file-version")
            {
               //nothing to do with the file version yet ...
               //Return
               tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-update-package-configuration");

               // Get all nodes
               if (mh_LoadNodes(c_XMLParser, orc_Config) == true)
               {
                  //Return
                  tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-update-package-configuration");
               }
               else
               {
                  s32_Return = C_CONFIG;
               }
            }
            else
            {
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

//-----------------------------------------------------------------------------
/*!
   \brief   Load update package configuration

   \param[in]     orc_FilePath       File path for configuration
   \param[out]    orc_Config         Update package configuration

   \return
   C_NO_ERR    File save
   C_RANGE     File already exists
   C_NOACT     Could not write data to file
   C_CONFIG    Error on creating XML file

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageConfigFiler::h_SaveConfig(const QString & orc_FilePath,
                                                     const C_SyvUpUpdatePackageConfig & orc_Config)
{
   sint32 s32_Return = C_RANGE;
   C_OSCXMLParser c_XMLParser;

   if (stw_tgl::TGL_FileExists(orc_FilePath.toStdString().c_str()) == false)
   {
      bool q_Success;

      // Main node
      q_Success =
         (c_XMLParser.CreateAndSelectNodeChild("opensyde-update-package-configuration") ==
          "opensyde-update-package-configuration");

      if (q_Success == true)
      {
         // File Version
         q_Success = (c_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");

         if (q_Success == true)
         {
            c_XMLParser.SetNodeContent(C_SCLString::IntToStr(hu16_FILE_VERSION_LATEST));
            //Return
            tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-update-package-configuration");
         }
      }

      if (q_Success == true)
      {
         uint32 u32_NodeCounter;

         // Save each update package node configuration
         q_Success = (c_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");

         if (q_Success == true)
         {
            for (u32_NodeCounter = 0U; u32_NodeCounter < orc_Config.c_NodeConfigs.size(); ++u32_NodeCounter)
            {
               q_Success = mh_SaveNode(c_XMLParser, orc_Config.c_NodeConfigs[u32_NodeCounter]);

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
         s32_Return = c_XMLParser.SaveToFile(orc_FilePath.toStdString().c_str());
      }
      else
      {
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageConfigFiler::C_SyvUpUpdatePackageConfigFiler()
{
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageConfigFiler::mh_SaveNode(C_OSCXMLParser & orc_XMLParser,
                                                  const C_SyvUpUpdatePackageConfigNode & orc_NodeConfig)
{
   bool q_Success;

   q_Success = (orc_XMLParser.CreateAndSelectNodeChild("node") == "node");

   if (q_Success == true)
   {
      uint32 u32_AppCounter;

      orc_XMLParser.CreateNodeChild("name", orc_NodeConfig.c_Name.toStdString().c_str());
      orc_XMLParser.CreateNodeChild("type", orc_NodeConfig.c_DeviceType.toStdString().c_str());

      q_Success = (orc_XMLParser.CreateAndSelectNodeChild("applications") == "applications");

      if (q_Success == true)
      {
         // Save all application configurations
         for (u32_AppCounter = 0U; u32_AppCounter < orc_NodeConfig.c_AppConfigs.size(); ++u32_AppCounter)
         {
            q_Success = mh_SaveApp(orc_XMLParser, orc_NodeConfig.c_AppConfigs[u32_AppCounter]);

            if (q_Success == false)
            {
               break;
            }
         }

         // Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      }

      // Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
   }

   return q_Success;
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageConfigFiler::mh_SaveApp(C_OSCXMLParser & orc_XMLParser,
                                                 const C_SyvUpUpdatePackageConfigNodeApp & orc_AppConfig)
{
   bool q_Success;

   q_Success = (orc_XMLParser.CreateAndSelectNodeChild("application") == "application");

   if (q_Success == true)
   {
      orc_XMLParser.CreateNodeChild("type", C_OSCNodeApplication::h_ApplicationToString(orc_AppConfig.e_Type));
      orc_XMLParser.CreateNodeChild("name", orc_AppConfig.c_Name.toStdString().c_str());
      orc_XMLParser.CreateNodeChild("path", orc_AppConfig.c_Path.toStdString().c_str());

      // Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
   }

   return q_Success;
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageConfigFiler::mh_LoadNodes(C_OSCXMLParser & orc_XMLParser,
                                                   C_SyvUpUpdatePackageConfig & orc_Config)
{
   bool q_Success = true;

   if (orc_XMLParser.SelectNodeChild("nodes") == "nodes")
   {
      if (orc_XMLParser.SelectNodeChild("node") == "node")
      {
         do
         {
            C_SyvUpUpdatePackageConfigNode c_NodeConfig;

            // Name of node
            if (orc_XMLParser.SelectNodeChild("name") == "name")
            {
               c_NodeConfig.c_Name = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
            }
            else
            {
               q_Success = false;
            }

            // Type of node
            if (orc_XMLParser.SelectNodeChild("type") == "type")
            {
               c_NodeConfig.c_DeviceType = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
            }
            else
            {
               q_Success = false;
            }

            if (q_Success == true)
            {
               // Get all applications for this node
               q_Success = mh_LoadApps(orc_XMLParser, c_NodeConfig);

               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
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
         while (orc_XMLParser.SelectNodeNext("node") == "node");

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}

//-----------------------------------------------------------------------------
bool C_SyvUpUpdatePackageConfigFiler::mh_LoadApps(C_OSCXMLParser & orc_XMLParser,
                                                  C_SyvUpUpdatePackageConfigNode & orc_NodeConfig)
{
   bool q_Success = true;

   if (orc_XMLParser.SelectNodeChild("applications") == "applications")
   {
      if (orc_XMLParser.SelectNodeChild("application") == "application")
      {
         do
         {
            C_SyvUpUpdatePackageConfigNodeApp c_AppConfig;

            // Type of application
            if (orc_XMLParser.SelectNodeChild("type") == "type")
            {
               if (C_OSCNodeApplication::h_StringToApplication(orc_XMLParser.GetNodeContent(),
                                                               c_AppConfig.e_Type) != C_NO_ERR)
               {
                  q_Success = false;
               }

               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               q_Success = false;
            }

            // Name of application
            if (orc_XMLParser.SelectNodeChild("name") == "name")
            {
               c_AppConfig.c_Name = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               q_Success = false;
            }

            // Path of application
            if (orc_XMLParser.SelectNodeChild("path") == "path")
            {
               c_AppConfig.c_Path = orc_XMLParser.GetNodeContent().c_str();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
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
         while (orc_XMLParser.SelectNodeNext("application") == "application");

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
      }
   }
   else
   {
      q_Success = false;
   }

   return q_Success;
}
