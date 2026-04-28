//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for manifest data

   Filer for manifest data

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscXcoManifestFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscXcoManifestFiler::hc_FILE_NAME = "manifest.syde_pkg";
const uint16_t C_OscXcoManifestFiler::mhu16_FILE_VERSION_1 = 1;
const uint16_t C_OscXcoManifestFiler::mhu16_PACKAGE_VERSION_1 = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manifest

   \param[in,out]  orc_Config    Config
   \param[in]      orc_Path      Path

   \return
   C_NO_ERR    data read
   C_RANGE     specified manifest file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    manifest file content is invalid or incomplete
               manifest file could not be loaded
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoManifestFiler::h_LoadFile(C_OscXcoManifest & orc_Config, const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading manifest data");
      s32_Retval = c_XmlParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-update-package-manifest")
         {
            s32_Retval = h_LoadData(orc_Config, c_XmlParser);
         }
         else
         {
            osc_write_log_error("Loading manifest data",
                                "Could not find \"opensyde-update-package-manifest\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading manifest data", "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading manifest data", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data loggers

   \param[in]  orc_Config  Config
   \param[in]  orc_Path    Path

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoManifestFiler::h_SaveFile(const C_OscXcoManifest & orc_Config, const stw::scl::C_SclString & orc_Path)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_Path,
                                                                    "opensyde-update-package-manifest");

   if (s32_Retval == C_NO_ERR)
   {
      //node
      C_OscXcoManifestFiler::h_SaveData(orc_Config, c_XmlParser);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_Path) != C_NO_ERR)
      {
         osc_write_log_error("Saving manifest data", "Could not create file.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load manifest

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    manifest file content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscXcoManifestFiler::h_LoadData(C_OscXcoManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   stw::scl::C_SclString c_Types;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_CheckVersion(orc_XmlParser, mhu16_FILE_VERSION_1, "file-version",
                                                             "Loading manifest data");

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("package");
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeStringError("types", c_Types);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_Types != "x-app-config")
         {
            orc_XmlParser.ReportErrorForAttributeContentStartingWithXmlContext("types",
                                                                               "expecting content to be \"x-app-config\"");
            s32_Retval = C_CONFIG;
         }
         orc_XmlParser.SelectNodeParent();
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.SelectNodeChildError("x-app-config");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OscSystemFilerUtil::h_CheckVersion(orc_XmlParser, mhu16_PACKAGE_VERSION_1, "package-version",
                                                           "Loading manifest data");
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.SelectNodeChildError("x-app-node");
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.GetAttributeStringError("name", orc_Config.c_NodeName);
         orc_XmlParser.SelectNodeParent();
      }
      orc_XmlParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save manifest

   \param[in]      orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXcoManifestFiler::h_SaveData(const C_OscXcoManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser)
{
   //File version
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mhu16_FILE_VERSION_1));
   //Return
   orc_XmlParser.SelectNodeParent();
   //Package
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("package") == "package");
   orc_XmlParser.SetAttributeString("types", "x-app-config");
   //Return
   orc_XmlParser.SelectNodeParent();
   //Config
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("x-app-config") == "x-app-config");
   orc_XmlParser.CreateNodeChild("package-version", stw::scl::C_SclString::IntToStr(mhu16_PACKAGE_VERSION_1));
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("x-app-node") == "x-app-node");
   orc_XmlParser.SetAttributeString("name", orc_Config.c_NodeName);
   //Return
   orc_XmlParser.SelectNodeParent();
   //Return
   orc_XmlParser.SelectNodeParent();

   //Return
   orc_XmlParser.SelectNodeParent();
}
